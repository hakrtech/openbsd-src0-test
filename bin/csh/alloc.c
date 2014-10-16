/*	$OpenBSD: alloc.c,v 1.11 2014/10/16 18:23:26 deraadt Exp $	*/
/*	$NetBSD: alloc.c,v 1.6 1995/03/21 09:02:23 cgd Exp $	*/

/*-
 * Copyright (c) 1983, 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>

#include "csh.h"
#include "extern.h"

ptr_t
Malloc(size_t n)
{
    ptr_t   ptr;

    if ((ptr = malloc(n)) == (ptr_t) 0) {
	child++;
	stderror(ERR_NOMEM);
    }
    return (ptr);
}

ptr_t
Realloc(ptr_t p, size_t n)
{
    ptr_t   ptr;

    if ((ptr = realloc(p, n)) == (ptr_t) 0) {
	child++;
	stderror(ERR_NOMEM);
    }
    return (ptr);
}

ptr_t
Calloc(size_t s, size_t n)
{
    ptr_t   ptr;

    if ((ptr = calloc(s, n)) == (ptr_t) 0) {
	child++;
	stderror(ERR_NOMEM);
    }

    return (ptr);
}

void
Free(ptr_t p)
{
    if (p)
	free(p);
}
