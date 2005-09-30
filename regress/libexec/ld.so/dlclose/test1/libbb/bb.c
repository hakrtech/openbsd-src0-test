/*	$OpenBSD: bb.c,v 1.2 2005/09/30 14:57:35 kurt Exp $	*/

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

int bbSymbol;

void
bbLazyFun()
{

}

int
duplicateFun()
{
	return (1);
}

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
