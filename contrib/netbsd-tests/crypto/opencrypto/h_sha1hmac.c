/* $NetBSD: h_sha1hmac.c,v 1.2 2014/01/18 02:31:14 joerg Exp $ */

/*-
 * Copyright (c) 2014 The NetBSD Foundation, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include <sys/ioctl.h>
#include <sys/time.h>

#include <crypto/cryptodev.h>

#define SHA1_HMAC_KEYLEN	20	/* Only key-length we support */

/* Test data from RFC2202 */
const struct {
	int num;
        size_t key_len;
        size_t len;
        unsigned char key[80];
        unsigned char data[80]; 
        unsigned char mac[20];
} tests[] = {
        /* Test #1 */
	{ 1, 20, 8,
	  { 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
	    0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
	    0x0b, 0x0b, 0x0b, 0x0b },
	  "Hi There",
	  { 0xb6, 0x17, 0x31, 0x86, 0x55, 0x05, 0x72, 0x64,
	    0xe2, 0x8b, 0xc0, 0xb6, 0xfb, 0x37, 0x8c, 0x8e,
	    0xf1, 0x46, 0xbe, 0x00 }
	},
        /* Test #2 */
	{ 2, 4, 28,
	  "Jefe",
	  "what do ya want for nothing?",
	  { 0xef, 0xfc, 0xdf, 0x6a, 0xe5, 0xeb, 0x2f, 0xa2,
	    0xd2, 0x74, 0x16, 0xd5, 0xf1, 0x84, 0xdf, 0x9c,
	    0x25, 0x9a, 0x7c, 0x79 }
	},
        /* Test #3 */
	{ 3, 20, 50,
	  { 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
	    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
	    0xaa, 0xaa, 0xaa, 0xaa },
	  { 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
	    0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
	    0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
	    0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
	    0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
	    0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
	    0xdd, 0xdd },
	  { 0x12, 0x5d, 0x73, 0x42, 0xb9, 0xac, 0x11, 0xcd,
	    0x91, 0xa3, 0x9a, 0xf4, 0x8a, 0xa1, 0x7b, 0x4f,
	    0x63, 0xf1, 0x75, 0xd3 }
	},
        /* Test #4 */
	{ 4, 25, 50,
	  { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
	    0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
	    0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
	    0x19 },
	  { 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
	    0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
	    0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
	    0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
	    0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
	    0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
	    0xcd, 0xcd },
	  { 0x4c, 0x90, 0x07, 0xf4, 0x02, 0x62, 0x50, 0xc6,
	    0xbc, 0x84, 0x14, 0xf9, 0xbf, 0x50, 0xc8, 0x6c,
	    0x2d, 0x72, 0x35, 0xda }
	},
        /* Test #5 */
	{ 5, 20, 20,
	  { 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
	    0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
	    0x0c, 0x0c, 0x0c, 0x0c },
	  "Test With Truncation",
	  { 0x4c, 0x1a, 0x03, 0x42, 0x4b, 0x55, 0xe0, 0x7f,
	    0xe7, 0xf2, 0x7b, 0xe1, 0xd5, 0x8b, 0xb9, 0x32,
	    0x4a, 0x9a, 0x5a, 0x04 }
	},
        /* Test #6 */
	{ 6, 80, 54,
	  { 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
	    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
	    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
	    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
	    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
	    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
	    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
	    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
	    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
	    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa },
	  "Test Using Larger Than Block-Size Key - Hash Key First",
	  { 0xaa, 0x4a, 0xe5, 0xe1, 0x52, 0x72, 0xd0, 0x0e,
	    0x95, 0x70, 0x56, 0x37, 0xce, 0x8a, 0x3b, 0x55,
	    0xed, 0x40, 0x21, 0x12 }
	},
        /* Test #7 */
	{ 7, 80, 73,
	  { 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
	    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
	    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
	    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
	    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
	    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
	    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
	    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
	    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
	    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa },
	  "Test Using Larger Than Block-Size Key and Larger Than "
		"One Block-Size Data",
	  { 0xe8, 0xe9, 0x9d, 0x0f, 0x45, 0x23, 0x7d, 0x78,
	    0x6d, 0x6b, 0xba, 0xa7, 0x96, 0x5c, 0x78, 0x08,
	    0xbb, 0xff, 0x1a, 0x91 }
	},
};

int
main(void)
{
	size_t i;
	int fd, res;
	struct session_op cs;
	struct crypt_op co;
	unsigned char buf[20];

	fd = open("/dev/crypto", O_RDWR, 0);
	if (fd < 0)
		err(1, "open");
	for (i = 0; i < __arraycount(tests); i++) {
		if (tests[i].key_len != SHA1_HMAC_KEYLEN)
			continue;

		memset(&cs, 0, sizeof(cs));
		cs.mac = CRYPTO_SHA1_HMAC;
		cs.mackeylen = tests[i].key_len;
		cs.mackey = __UNCONST(&tests[i].key);
		res = ioctl(fd, CIOCGSESSION, &cs);
		if (res < 0)
			err(1, "CIOCGSESSION test %d", tests[i].num);

		memset(&co, 0, sizeof(co));
		memset(buf, 0, sizeof(buf));
		co.ses = cs.ses;
		co.op = COP_ENCRYPT;
		co.len = tests[i].len;
		co.src = __UNCONST(&tests[i].data);
		co.mac = buf;
		res = ioctl(fd, CIOCCRYPT, &co);
		if (res < 0)
			err(1, "CIOCCRYPT test %d", tests[i].num);
	
		if (memcmp(co.mac, &tests[i].mac, sizeof(tests[i].mac)))
			errx(1, "verification failed test %d", tests[i].num);

		res = ioctl(fd, CIOCFSESSION, &cs.ses);
		if (res < 0)
			err(1, "CIOCFSESSION test %d", tests[i].num);
	}
	return 0;
}
