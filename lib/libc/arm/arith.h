/*
 * MD header for contrib/gdtoa
 *
 * $FreeBSD$
 */

/*
 * NOTE: The definitions in this file must be correct or strtod(3) and
 * floating point formats in printf(3) will break!  The file can be
 * generated by running contrib/gdtoa/arithchk.c on the target
 * architecture.  See contrib/gdtoa/gdtoaimp.h for details.
 */

#ifndef __ARMEB__
#define IEEE_8087
#define Arith_Kind_ASL 1
#define Sudden_Underflow
#else
#define IEEE_MC68k
#define Arith_Kind_ASL 2
#endif
