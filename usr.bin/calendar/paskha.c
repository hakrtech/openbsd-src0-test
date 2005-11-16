/*	$OpenBSD: paskha.c,v 1.5 2005/11/16 16:45:11 deraadt Exp $	*/

/*
 * Copyright (C) 1993-1996 by Andrey A. Chernov, Moscow, Russia.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND
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

#ifndef lint
static const char rcsid[] = "$OpenBSD: paskha.c,v 1.5 2005/11/16 16:45:11 deraadt Exp $";
#endif /* not lint */

#include <stdio.h>
#include <tzfile.h>

#include "calendar.h"

/* return year day for Orthodox Easter using Gauss formula */
/* (new style result); subtract 13 for old style */

int
paskha(int R)  /*year*/
{
	int a, b, c, d, e;
	static int x = 15;
	static int y = 6;
	int cumdays;

	a = R % 19;
	b = R % 4;
	c = R % 7;
	d = (19*a + x) % 30;
	e = (2*b + 4*c + 6*d + y) % 7;
	cumdays = 31 + 28;
	if (isleap(R))
		cumdays++;
	return ((cumdays + 22) + (d + e) + 13);
}
