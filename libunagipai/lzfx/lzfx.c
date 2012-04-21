/*
 * Copyright (c) 2009 Andrew Collette <andrew.collette at gmail.com>
 * http://lzfx.googlecode.com
 *
 * Implements an LZF-compatible compressor/decompressor based on the liblzf
 * codebase written by Marc Lehmann.  This code is released under the BSD
 * license.  License and original copyright statement follow.
 *
 * 
 * Copyright (c) 2000-2008 Marc Alexander Lehmann <schmorp@schmorp.de>
 * 
 * Redistribution and use in source and binary forms, with or without modifica-
 * tion, are permitted provided that the following conditions are met:
 * 
 *   1.  Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 * 
 *   2.  Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MER-
 * CHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPE-
 * CIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTH-
 * ERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "lzfx.h"

#define LZFX_HSIZE (1 << (LZFX_HLOG))

/* We need this for memset */
#ifdef __cplusplus
# include <cstring>
#else
# include <string.h>
#endif

#if __GNUC__ >= 3
# define fx_expect_false(expr)  __builtin_expect((expr) != 0, 0)
# define fx_expect_true(expr)   __builtin_expect((expr) != 0, 1)
#else
# define fx_expect_false(expr)  (expr)
# define fx_expect_true(expr)   (expr)
#endif

typedef unsigned char u8;
typedef const u8 *LZSTATE[LZFX_HSIZE];

/* Define the hash function */
#define LZFX_FRST(p)     (((p[0]) << 8) | p[1])
#define LZFX_NEXT(v,p)   (((v) << 8) | p[2])
#define LZFX_IDX(h)      ((( h >> (3*8 - LZFX_HLOG)) - h  ) & (LZFX_HSIZE - 1))

/* These cannot be changed, as they are related to the compressed format. */
#define LZFX_MAX_LIT        (1 <<  5)
#define LZFX_MAX_OFF        (1 << 13)
#define LZFX_MAX_REF        ((1 << 8) + (1 << 3))

static
int lzfx_getsize(const void* ibuf, unsigned int ilen, unsigned int *olen);

/* Compressed format

 There are two kinds of structures in LZF/LZFX: literal runs and back
 references. The length of a literal run is encoded as L - 1, as it must
 contain at least one byte.  Literals are encoded as follows:

 000LLLLL <L+1 bytes>

 Back references are encoded as follows.  The smallest possible encoded
 length value is 1, as otherwise the control byte would be recognized as
 a literal run.  Since at least three bytes must match for a back reference
 to be inserted, the length is encoded as L - 2 instead of L - 1.  The
 offset (distance to the desired data in the output buffer) is encoded as
 o - 1, as all offsets are at least 1.  The binary format is:

 LLLooooo oooooooo           for backrefs of real length < 9   (1 <= L < 7)
 111ooooo LLLLLLLL oooooooo  for backrefs of real length >= 9  (L > 7)
 */
int lzfx_compress(const void * const ibuf, const unsigned int ilen, void *obuf, unsigned int * const olen) {

	/* Hash table; an array of u8*'s which point
	 to various locations in the input buffer */
	const u8 *htab[LZFX_HSIZE];

	const u8 **hslot; /* Pointer to entry in hash table */
	unsigned int hval; /* Hash value generated by macros above */
	const u8 *ref; /* Pointer to candidate match location in input */

	const u8 *ip = (const u8 *) ibuf;
	const u8 * const in_end = ip + ilen;

	u8 *op = (u8 *) obuf;
	const u8 * const out_end = (olen == NULL ? NULL : op + *olen);

	int lit; /* # of bytes in current literal run */

#if defined (WIN32) && defined (_M_X64)
	unsigned _int64 off; /* workaround for missing POSIX compliance */
#else
	unsigned long off;
#endif

	if (olen == NULL)
		return LZFX_EARGS;
	if (ibuf == NULL) {
		if (ilen != 0)
			return LZFX_EARGS;
		*olen = 0;
		return 0;
	}
	if (obuf == NULL)
		return LZFX_EARGS;

	memset(htab, 0, sizeof(htab));

	/*  Start a literal run.  Whenever we do this the output pointer is
	 advanced because the current byte will hold the encoded length. */
	lit = 0;
	op++;

	hval = LZFX_FRST(ip);

	while (ip + 2 < in_end) { /* The NEXT macro reads 2 bytes ahead */

		hval = LZFX_NEXT(hval, ip);
		hslot = htab + LZFX_IDX(hval);

		ref = *hslot;
		*hslot = ip;

		if (ref < ip && (off = ip - ref - 1) < LZFX_MAX_OFF && ip + 4 < in_end /* Backref takes up to 3 bytes, so don't bother */
		&& ref > (u8 *) ibuf && ref[0] == ip[0] && ref[1] == ip[1] && ref[2] == ip[2]) {

			unsigned int len = 3; /* We already know 3 bytes match */
			const unsigned int maxlen = in_end - ip - 2 > LZFX_MAX_REF ? LZFX_MAX_REF : in_end - ip - 2;

			/* lit == 0:  op + 3 must be < out_end (because we undo the run)
			 lit != 0:  op + 3 + 1 must be < out_end */
			if (fx_expect_false(op - !lit + 3 + 1 >= out_end))
				return LZFX_ESIZE;

			op[-lit - 1] = lit - 1; /* Terminate literal run */
			op -= !lit; /* Undo run if length is zero */

			/*  Start checking at the fourth byte */
			while (len < maxlen && ref[len] == ip[len])
				len++;

			len -= 2; /* We encode the length as #octets - 2 */

			/* Format 1: [LLLooooo oooooooo] */
			if (len < 7) {
				*op++ = (off >> 8) + (len << 5);
				*op++ = off;

				/* Format 2: [111ooooo LLLLLLLL oooooooo] */
			}
			else {
				*op++ = (off >> 8) + (7 << 5);
				*op++ = len - 7;
				*op++ = off;
			}

			lit = 0;
			op++;

			ip += len + 1; /* ip = initial ip + #octets -1 */

			if (fx_expect_false (ip + 3 >= in_end)) {
				ip++; /* Code following expects exit at bottom of loop */
				break;
			}

			hval = LZFX_FRST (ip);
			hval = LZFX_NEXT (hval, ip);
			htab[LZFX_IDX (hval)] = ip;

			ip++; /* ip = initial ip + #octets */

		}
		else {
			/* Keep copying literal bytes */

			if (fx_expect_false (op >= out_end))
				return LZFX_ESIZE;

			lit++;
			*op++ = *ip++;

			if (fx_expect_false (lit == LZFX_MAX_LIT)) {
				op[-lit - 1] = lit - 1; /* stop run */
				lit = 0;
				op++; /* start run */
			}

		} /* if() found match in htab */

	} /* while(ip < ilen -2) */

	/*  At most 3 bytes remain in input.  We therefore need 4 bytes available
	 in the output buffer to store them (3 data + ctrl byte).*/
	if (op + 3 > out_end)
		return LZFX_ESIZE;

	while (ip < in_end) {

		lit++;
		*op++ = *ip++;

		if (fx_expect_false (lit == LZFX_MAX_LIT)) {
			op[-lit - 1] = lit - 1;
			lit = 0;
			op++;
		}
	}

	op[-lit - 1] = lit - 1;
	op -= !lit;

	*olen = op - (u8 *) obuf;
	return 0;
}

/* Decompressor */
int lzfx_decompress(const void* ibuf, unsigned int ilen, void* obuf, unsigned int *olen) {

	u8 const *ip = (const u8 *) ibuf;
	u8 const * const in_end = ip + ilen;
	u8 *op = (u8 *) obuf;
	u8 const * const out_end = (olen == NULL ? NULL : op + *olen);

	unsigned int remain_len = 0;
	int rc;

	if (olen == NULL)
		return LZFX_EARGS;
	if (ibuf == NULL) {
		if (ilen != 0)
			return LZFX_EARGS;
		*olen = 0;
		return 0;
	}
	if (obuf == NULL) {
		if (*olen != 0)
			return LZFX_EARGS;
		return lzfx_getsize(ibuf, ilen, olen);
	}

	do {
		unsigned int ctrl = *ip++;

		/* Format 000LLLLL: a literal byte string follows, of length L+1 */
		if (ctrl < (1 << 5)) {

			ctrl++;

			if (fx_expect_false(op + ctrl > out_end)) {
				--ip; /* Rewind to control byte */
				goto guess;
			}
			if (fx_expect_false(ip + ctrl > in_end))
				return LZFX_ECORRUPT;

			do
				*op++ = *ip++;
			while (--ctrl);

			/*  Format #1 [LLLooooo oooooooo]: backref of length L+1+2
			 ^^^^^ ^^^^^^^^
			 A      B
			 #2 [111ooooo LLLLLLLL oooooooo] backref of length L+7+2
			 ^^^^^          ^^^^^^^^
			 A               B
			 In both cases the location of the backref is computed from the
			 remaining part of the data as follows:

			 location = op - A*256 - B - 1
			 */
		}
		else {

			unsigned int len = (ctrl >> 5);
			u8 *ref = op - ((ctrl & 0x1f) << 8) - 1;

			if (len == 7)
				len += *ip++; /* i.e. format #2 */

			len += 2; /* len is now #octets */

			if (fx_expect_false(op + len > out_end)) {
				ip -= (len >= 9) ? 2 : 1; /* Rewind to control byte */
				goto guess;
			}
			if (fx_expect_false(ip >= in_end))
				return LZFX_ECORRUPT;

			ref -= *ip++;

			if (fx_expect_false(ref < (u8*)obuf))
				return LZFX_ECORRUPT;

			do
				*op++ = *ref++;
			while (--len);
		}

	} while (ip < in_end);

	*olen = op - (u8 *) obuf;

	return 0;

	guess: rc = lzfx_getsize(ip, ilen - (ip - (u8*) ibuf), &remain_len);
	if (rc >= 0)
		*olen = remain_len + (op - (u8*) obuf);
	return rc;
}

/* Guess len. No parameters may be NULL; this is not checked. */
static
int lzfx_getsize(const void* ibuf, unsigned int ilen, unsigned int *olen) {

	u8 const *ip = (const u8 *) ibuf;
	u8 const * const in_end = ip + ilen;
	int tot_len = 0;

	while (ip < in_end) {

		unsigned int ctrl = *ip++;

		if (ctrl < (1 << 5)) {

			ctrl++;

			if (ip + ctrl > in_end)
				return LZFX_ECORRUPT;

			tot_len += ctrl;
			ip += ctrl;

		}
		else {

			unsigned int len = (ctrl >> 5);

			if (len == 7) { /* i.e. format #2 */
				len += *ip++;
			}

			len += 2; /* len is now #octets */

			if (ip >= in_end)
				return LZFX_ECORRUPT;

			ip++; /* skip the ref byte */

			tot_len += len;

		}

	}

	*olen = tot_len;

	return 0;
}

