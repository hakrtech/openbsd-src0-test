/*	$OpenBSD: Pledge.xs,v 1.2 2017/09/09 14:53:57 afresh1 Exp $	*/

/*
 * Copyright (c) 2015 Andrew Fresh <afresh1@openbsd.org>
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

#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#define PLEDGENAMES
#include <sys/pledge.h>

MODULE = OpenBSD::Pledge		PACKAGE = OpenBSD::Pledge

AV *
pledgenames()
    INIT:
	int i;
    CODE:
	for (i = 0; pledgenames[i].bits != 0; i++)
		XPUSHs( sv_2mortal(
		     newSVpv(pledgenames[i].name, strlen(pledgenames[i].name))
		) );
	XSRETURN(i);

int
_pledge(const char * promises)
    CODE:
	RETVAL = pledge(promises, NULL) != -1;
    OUTPUT:
	RETVAL
