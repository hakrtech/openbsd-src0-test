/*	$OpenBSD: doalloc.c,v 1.4 1999/05/08 20:29:01 millert Exp $	*/

/****************************************************************************
 * Copyright (c) 1998 Free Software Foundation, Inc.                        *
 *                                                                          *
 * Permission is hereby granted, free of charge, to any person obtaining a  *
 * copy of this software and associated documentation files (the            *
 * "Software"), to deal in the Software without restriction, including      *
 * without limitation the rights to use, copy, modify, merge, publish,      *
 * distribute, distribute with modifications, sublicense, and/or sell       *
 * copies of the Software, and to permit persons to whom the Software is    *
 * furnished to do so, subject to the following conditions:                 *
 *                                                                          *
 * The above copyright notice and this permission notice shall be included  *
 * in all copies or substantial portions of the Software.                   *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS  *
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF               *
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.   *
 * IN NO EVENT SHALL THE ABOVE COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,   *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR    *
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR    *
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.                               *
 *                                                                          *
 * Except as contained in this notice, the name(s) of the above copyright   *
 * holders shall not be used in advertising or otherwise to promote the     *
 * sale, use or other dealings in this Software without prior written       *
 * authorization.                                                           *
 ****************************************************************************/

/****************************************************************************
 *  Author: Thomas E. Dickey <dickey@clark.net> 1998                        *
 ****************************************************************************/


/*
 * Wrapper for malloc/realloc.  Standard implementations allow realloc with
 * a null pointer, but older libraries may not (e.g., SunOS).
 *
 * Also if realloc fails, we discard the old memory to avoid leaks.
 */

#include <curses.priv.h>

MODULE_ID("$From: doalloc.c,v 1.5 1999/03/14 12:25:27 tom Exp $")

void *_nc_doalloc(void *oldp, size_t amount)
{
	void *newp;

	if (oldp != 0) {
		if ((newp = realloc(oldp, amount)) == 0) {
			free(oldp);
			errno = ENOMEM;		/* just in case 'free' reset */
		}
	} else {
		newp = typeMalloc(char, amount);
	}
	return newp;
}

#if !HAVE_STRDUP
char *_nc_strdup(const char *src)
{
    char *dst;
    if (src != 0) {
	dst = typeMalloc(char, strlen(src) + 1);
	if (dst != 0) {
	    (void)strcpy(dst, src);
	}
    } else {
	dst = 0;
    }
    return dst;
}
#endif
