/*
 * Copyright (c) 2016 Thomas Pornin <pornin@bolet.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining 
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be 
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "inner.h"

static const unsigned char P521_N[] = {
	0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFA, 0x51, 0x86, 0x87, 0x83, 0xBF, 0x2F,
	0x96, 0x6B, 0x7F, 0xCC, 0x01, 0x48, 0xF7, 0x09,
	0xA5, 0xD0, 0x3B, 0xB5, 0xC9, 0xB8, 0x89, 0x9C,
	0x47, 0xAE, 0xBB, 0x6F, 0xB7, 0x1E, 0x91, 0x38,
	0x64, 0x09
};

static const unsigned char P521_G[] = {
	0x04, 0x00, 0xC6, 0x85, 0x8E, 0x06, 0xB7, 0x04,
	0x04, 0xE9, 0xCD, 0x9E, 0x3E, 0xCB, 0x66, 0x23,
	0x95, 0xB4, 0x42, 0x9C, 0x64, 0x81, 0x39, 0x05,
	0x3F, 0xB5, 0x21, 0xF8, 0x28, 0xAF, 0x60, 0x6B,
	0x4D, 0x3D, 0xBA, 0xA1, 0x4B, 0x5E, 0x77, 0xEF,
	0xE7, 0x59, 0x28, 0xFE, 0x1D, 0xC1, 0x27, 0xA2,
	0xFF, 0xA8, 0xDE, 0x33, 0x48, 0xB3, 0xC1, 0x85,
	0x6A, 0x42, 0x9B, 0xF9, 0x7E, 0x7E, 0x31, 0xC2,
	0xE5, 0xBD, 0x66, 0x01, 0x18, 0x39, 0x29, 0x6A,
	0x78, 0x9A, 0x3B, 0xC0, 0x04, 0x5C, 0x8A, 0x5F,
	0xB4, 0x2C, 0x7D, 0x1B, 0xD9, 0x98, 0xF5, 0x44,
	0x49, 0x57, 0x9B, 0x44, 0x68, 0x17, 0xAF, 0xBD,
	0x17, 0x27, 0x3E, 0x66, 0x2C, 0x97, 0xEE, 0x72,
	0x99, 0x5E, 0xF4, 0x26, 0x40, 0xC5, 0x50, 0xB9,
	0x01, 0x3F, 0xAD, 0x07, 0x61, 0x35, 0x3C, 0x70,
	0x86, 0xA2, 0x72, 0xC2, 0x40, 0x88, 0xBE, 0x94,
	0x76, 0x9F, 0xD1, 0x66, 0x50
};

/* see inner.h */
const br_ec_curve_def br_secp521r1 = {
	BR_EC_secp521r1,
	P521_N, sizeof P521_N,
	P521_G, sizeof P521_G
};
