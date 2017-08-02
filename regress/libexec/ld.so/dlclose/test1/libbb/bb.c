/*	$OpenBSD: bb.c,v 1.3 2017/08/02 21:04:50 kettenis Exp $	*/

/*
 * Copyright (c) 2005 Kurt Miller <kurt@openbsd.org>
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
 *
 */

#include <dlfcn.h>
#include <stdio.h>

extern void bbLazyFun();
extern int duplicateFun();

int bbSymbol;

int
bbTest1()
{
	int ret = 0;

	/* RTLD_DEFAULT should see bbSymbol */
	if (dlsym(RTLD_DEFAULT, "bbSymbol") == NULL) {
		printf("dlsym(RTLD_DEFAULT, \"bbSymbol\") == NULL\n");
		ret = 1;
	}

	bbLazyFun();

	return (ret);
}

int
bbTest2()
{
	return(duplicateFun());
}
