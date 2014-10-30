/*	$OpenBSD: preconv.c,v 1.1 2014/10/30 00:05:02 schwarze Exp $ */
/*
 * Copyright (c) 2011 Kristaps Dzonsons <kristaps@bsd.lv>
 * Copyright (c) 2014 Ingo Schwarze <schwarze@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/types.h>

#include <stdio.h>
#include <string.h>
#include "mandoc.h"
#include "libmandoc.h"

int
preconv_encode(struct buf *ib, struct buf *ob, int *filenc)
{
	size_t		 i;
	const long	 one = 1L;
	int		 state, be;
	unsigned int	 accum;
	unsigned char	 cu;

	if ( ! (*filenc & MPARSE_UTF8))
		goto latin;

	state = 0;
	accum = 0U;
	be = 0;

	/* Quick test for big-endian value. */

	if ( ! (*((const char *)(&one))))
		be = 1;

	for (i = ib->offs; i < ib->sz; i++) {
		cu = ib->buf[i];
		if (state) {
			if ( ! (cu & 128) || (cu & 64)) {
				/* Bad sequence header. */
				break;
			}

			/* Accept only legitimate bit patterns. */

			if (cu > 191 || cu < 128) {
				/* Bad in-sequence bits. */
				break;
			}

			accum |= (cu & 63) << --state * 6;

			if (state)
				continue;

			/*
			 * Accum is held in little-endian order as
			 * stipulated by the UTF-8 sequence coding.  We
			 * need to convert to a native big-endian if our
			 * architecture requires it.
			 */

			if (be)
				accum = (accum >> 24) |
					((accum << 8) & 0x00FF0000) |
					((accum >> 8) & 0x0000FF00) |
					(accum << 24);

			if (accum < 0x80)
				ob->buf[ob->offs++] = accum;
			else
				ob->offs += snprintf(ob->buf + ob->offs,
				    11, "\\[u%.4X]", accum);
			ib->offs = i + 1;
			*filenc &= ~MPARSE_LATIN1;
			return(1);
		} else {
			/*
			 * Entering a UTF-8 state:  if we encounter a
			 * UTF-8 bitmask, calculate the expected UTF-8
			 * state from it.
			 */
			for (state = 0; state < 7; state++)
				if ( ! (cu & (1 << (7 - state))))
					break;

			/* Accept only legitimate bit patterns. */

			switch (state--) {
			case (4):
				if (cu <= 244 && cu >= 240) {
					accum = (cu & 7) << 18;
					continue;
				}
				/* Bad 4-sequence start bits. */
				break;
			case (3):
				if (cu <= 239 && cu >= 224) {
					accum = (cu & 15) << 12;
					continue;
				}
				/* Bad 3-sequence start bits. */
				break;
			case (2):
				if (cu <= 223 && cu >= 194) {
					accum = (cu & 31) << 6;
					continue;
				}
				/* Bad 2-sequence start bits. */
				break;
			default:
				/* Bad sequence bit mask. */
				break;
			}
			break;
		}
	}

	/* FALLTHROUGH: Invalid or incomplete UTF-8 sequence. */

latin:
	if ( ! (*filenc & MPARSE_LATIN1))
		return(0);

	ob->offs += snprintf(ob->buf + ob->offs, 11,
	    "\\[u%.4X]", (unsigned char)ib->buf[ib->offs++]);

	*filenc &= ~MPARSE_UTF8;
	return(1);
}

int
preconv_cue(const struct buf *b)
{
	const char	*ln, *eoln, *eoph;
	size_t		 sz, phsz;

	ln = b->buf + b->offs;
	sz = b->sz - b->offs;

	/* Look for the end-of-line. */

	if (NULL == (eoln = memchr(ln, '\n', sz)))
		eoln = ln + sz;

	/* Check if we have the correct header/trailer. */

	if ((sz = (size_t)(eoln - ln)) < 10 ||
	    memcmp(ln, ".\\\" -*-", 7) || memcmp(eoln - 3, "-*-", 3))
		return(MPARSE_UTF8 | MPARSE_LATIN1);

	/* Move after the header and adjust for the trailer. */

	ln += 7;
	sz -= 10;

	while (sz > 0) {
		while (sz > 0 && ' ' == *ln) {
			ln++;
			sz--;
		}
		if (0 == sz)
			break;

		/* Find the end-of-phrase marker (or eoln). */

		if (NULL == (eoph = memchr(ln, ';', sz)))
			eoph = eoln - 3;
		else
			eoph++;

		/* Only account for the "coding" phrase. */

		if ((phsz = eoph - ln) < 7 ||
		    strncasecmp(ln, "coding:", 7)) {
			sz -= phsz;
			ln += phsz;
			continue;
		}

		sz -= 7;
		ln += 7;

		while (sz > 0 && ' ' == *ln) {
			ln++;
			sz--;
		}
		if (0 == sz)
			return(0);

		/* Check us against known encodings. */

		if (phsz > 4 && !strncasecmp(ln, "utf-8", 5))
			return(MPARSE_UTF8);
		if (phsz > 10 && !strncasecmp(ln, "iso-latin-1", 11))
			return(MPARSE_LATIN1);
		return(0);
	}
	return(MPARSE_UTF8 | MPARSE_LATIN1);
}
