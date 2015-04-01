/*	$OpenBSD: mem.c,v 1.4 2015/04/01 20:58:13 millert Exp $	*/

/*-
 * Copyright (C) 2009 Gabor Kovesdan <gabor@FreeBSD.org>
 * Copyright (C) 2012 Oleg Moskalenko <mom040267@gmail.com>
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "mem.h"

/*
 * malloc() wrapper.
 */
void *
sort_malloc(size_t size)
{
	void *ptr;

	if ((ptr = malloc(size)) == NULL)
		err(2, NULL);
	return ptr;
}

/*
 * calloc() wrapper.
 */
void *
sort_calloc(size_t nmemb, size_t size)
{
	void *ptr;

	if ((ptr = calloc(nmemb, size)) == NULL)
		err(2, NULL);
	return ptr;
}

/*
 * free() wrapper.
 */
void
sort_free(void *ptr)
{
	free(ptr);
}

/*
 * realloc() wrapper.
 */
void *
sort_realloc(void *ptr, size_t size)
{
	if ((ptr = realloc(ptr, size)) == NULL)
		err(2, NULL);
	return ptr;
}

/*
 * reallocarray() wrapper.
 */
void *
sort_reallocarray(void *ptr, size_t nmemb, size_t size)
{
	if ((ptr = reallocarray(ptr, nmemb, size)) == NULL)
		err(2, NULL);
	return ptr;
}

char *
sort_strdup(const char *str)
{
	char *dup;

	if ((dup = strdup(str)) == NULL)
		err(2, NULL);
	return dup;
}

int
sort_asprintf(char **ret, const char *fmt, ...)
{
	int len;
	va_list ap;

	va_start(ap, fmt);
	len = vasprintf(ret, fmt, ap);
	va_end(ap);

	if (len == -1)
		err(2, NULL);
	return len;
}
