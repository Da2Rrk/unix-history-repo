/*
 * Copyright (c) 2001 Daniel Eischen <deischen@freebsd.org>
 * Copyright (c) 2000-2001 Jason Evans <jasone@freebsd.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD$
 */
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/queue.h>
#include <stdlib.h>
#include <pthread.h>
#include "thr_private.h"

/* Spare thread stack. */
struct stack {
	LIST_ENTRY(stack)	qe;		/* Stack queue linkage. */
	size_t			stacksize;	/* Stack size (rounded up). */
	size_t			guardsize;	/* Guard size. */
	void			*stackaddr;	/* Stack address. */
};

/*
 * Default sized (stack and guard) spare stack queue.  Stacks are cached
 * to avoid additional complexity managing mmap()ed stack regions.  Spare
 * stacks are used in LIFO order to increase cache locality.
 */
static LIST_HEAD(, stack)	dstackq = LIST_HEAD_INITIALIZER(dstackq);

/*
 * Miscellaneous sized (non-default stack and/or guard) spare stack queue.
 * Stacks are cached to avoid additional complexity managing mmap()ed
 * stack regions.  This list is unordered, since ordering on both stack
 * size and guard size would be more trouble than it's worth.  Stacks are
 * allocated from this cache on a first size match basis.
 */
static LIST_HEAD(, stack)	mstackq = LIST_HEAD_INITIALIZER(mstackq);

/**
 * Base address of the last stack allocated (including its red zone, if
 * there is one).  Stacks are allocated contiguously, starting beyond the
 * top of the main stack.  When a new stack is created, a red zone is
 * typically created (actually, the red zone is mapped with PROT_NONE) above
 * the top of the stack, such that the stack will not be able to grow all
 * the way to the bottom of the next stack.  This isn't fool-proof.  It is
 * possible for a stack to grow by a large amount, such that it grows into
 * the next stack, and as long as the memory within the red zone is never
 * accessed, nothing will prevent one thread stack from trouncing all over
 * the next.
 *
 * low memory
 *     . . . . . . . . . . . . . . . . . . 
 *    |                                   |
 *    |             stack 3               | start of 3rd thread stack
 *    +-----------------------------------+
 *    |                                   |
 *    |       Red Zone (guard page)       | red zone for 2nd thread
 *    |                                   |
 *    +-----------------------------------+
 *    |  stack 2 - PTHREAD_STACK_DEFAULT  | top of 2nd thread stack
 *    |                                   |
 *    |                                   |
 *    |                                   |
 *    |                                   |
 *    |             stack 2               |
 *    +-----------------------------------+ <-- start of 2nd thread stack
 *    |                                   |
 *    |       Red Zone                    | red zone for 1st thread
 *    |                                   |
 *    +-----------------------------------+
 *    |  stack 1 - PTHREAD_STACK_DEFAULT  | top of 1st thread stack
 *    |                                   |
 *    |                                   |
 *    |                                   |
 *    |                                   |
 *    |             stack 1               |
 *    +-----------------------------------+ <-- start of 1st thread stack
 *    |                                   |   (initial value of last_stack)
 *    |       Red Zone                    |
 *    |                                   | red zone for main thread
 *    +-----------------------------------+
 *    | USRSTACK - PTHREAD_STACK_INITIAL  | top of main thread stack
 *    |                                   | ^
 *    |                                   | |
 *    |                                   | |
 *    |                                   | | stack growth
 *    |                                   |
 *    +-----------------------------------+ <-- start of main thread stack
 *                                              (USRSTACK)
 * high memory
 *
 */
static void *last_stack = NULL;

/*
 * Round size up to the nearest multiple of
 * _thr_page_size.
 */
static inline size_t
round_up(size_t size)
{
	if (size % _thr_page_size != 0)
		size = ((size / _thr_page_size) + 1) *
		    _thr_page_size;
	return size;
}

int
_thr_stack_alloc(struct pthread_attr *attr)
{
	struct stack *spare_stack;
	struct kse *curkse;
	kse_critical_t crit;
	size_t stacksize;
	size_t guardsize;
	char *stackaddr;

	/*
	 * Round up stack size to nearest multiple of _thr_page_size so
	 * that mmap() * will work.  If the stack size is not an even
	 * multiple, we end up initializing things such that there is
	 * unused space above the beginning of the stack, so the stack
	 * sits snugly against its guard.
	 */
	stacksize = round_up(attr->stacksize_attr);
	guardsize = round_up(attr->guardsize_attr);

	attr->stackaddr_attr = NULL;
	attr->flags &= ~THR_STACK_USER;

	/*
	 * Use the garbage collector lock for synchronization of the
	 * spare stack lists and allocations from usrstack.
	 */
	crit = _kse_critical_enter();
	curkse = _get_curkse();
	KSE_LOCK_ACQUIRE(curkse, &_thread_list_lock);
	/*
	 * If the stack and guard sizes are default, try to allocate a stack
	 * from the default-size stack cache:
	 */
	if ((stacksize == THR_STACK_DEFAULT) &&
	    (guardsize == _thr_guard_default)) {
		if ((spare_stack = LIST_FIRST(&dstackq)) != NULL) {
			/* Use the spare stack. */
			LIST_REMOVE(spare_stack, qe);
			attr->stackaddr_attr = spare_stack->stackaddr;
		}
	}
	/*
	 * The user specified a non-default stack and/or guard size, so try to
	 * allocate a stack from the non-default size stack cache, using the
	 * rounded up stack size (stack_size) in the search:
	 */
	else {
		LIST_FOREACH(spare_stack, &mstackq, qe) {
			if (spare_stack->stacksize == stacksize &&
			    spare_stack->guardsize == guardsize) {
				LIST_REMOVE(spare_stack, qe);
				attr->stackaddr_attr = spare_stack->stackaddr;
				break;
			}
		}
	}
	if (attr->stackaddr_attr != NULL) {
		/* A cached stack was found.  Release the lock. */
		KSE_LOCK_RELEASE(curkse, &_thread_list_lock);
		_kse_critical_leave(crit);
	}
	else {
		/* Allocate a stack from usrstack. */
		if (last_stack == NULL)
			last_stack = _usrstack - THR_STACK_INITIAL -
			    _thr_guard_default;

		/* Allocate a new stack. */
		stackaddr = last_stack - stacksize - guardsize;

		/*
		 * Even if stack allocation fails, we don't want to try to
		 * use this location again, so unconditionally decrement
		 * last_stack.  Under normal operating conditions, the most
		 * likely reason for an mmap() error is a stack overflow of
		 * the adjacent thread stack.
		 */
		last_stack -= (stacksize + guardsize);

		/* Release the lock before mmap'ing it. */
		KSE_LOCK_RELEASE(curkse, &_thread_list_lock);
		_kse_critical_leave(crit);

		/* Map the stack and guard page together, and split guard
		   page from allocated space: */
		if ((stackaddr = mmap(stackaddr, stacksize+guardsize,
		     PROT_READ | PROT_WRITE, MAP_STACK,
		     -1, 0)) != MAP_FAILED &&
		    (guardsize == 0 ||
		     mprotect(stackaddr, guardsize, PROT_NONE) == 0)) {
			stackaddr += guardsize;
		} else {
			if (stackaddr != MAP_FAILED)
				munmap(stackaddr, stacksize + guardsize);
			stackaddr = NULL;
		}
		attr->stackaddr_attr = stackaddr;
	}
	if (attr->stackaddr_attr != NULL)
		return (0);
	else
		return (-1);
}

/* This function must be called with _thread_list_lock held. */
void
_thr_stack_free(struct pthread_attr *attr)
{
	struct stack *spare_stack;

	if ((attr != NULL) && ((attr->flags & THR_STACK_USER) == 0)
	    && (attr->stackaddr_attr != NULL)) {
		spare_stack = (attr->stackaddr_attr + attr->stacksize_attr
		    - sizeof(struct stack));
		spare_stack->stacksize = round_up(attr->stacksize_attr);
		spare_stack->guardsize = round_up(attr->guardsize_attr);
		spare_stack->stackaddr = attr->stackaddr_attr;

		if (spare_stack->stacksize == THR_STACK_DEFAULT &&
		    spare_stack->guardsize == _thr_guard_default) {
			/* Default stack/guard size. */
			LIST_INSERT_HEAD(&dstackq, spare_stack, qe);
		} else {
			/* Non-default stack/guard size. */
			LIST_INSERT_HEAD(&mstackq, spare_stack, qe);
		}
		attr->stackaddr_attr = NULL;
	}
}
