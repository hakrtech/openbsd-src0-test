/*	$OpenBSD: wwclose.c,v 1.2 1996/06/26 05:43:37 deraadt Exp $	*/
/*	$NetBSD: wwclose.c,v 1.3 1995/09/28 10:35:14 tls Exp $	*/

/*
 * Copyright (c) 1983, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Edward Wang at The University of California, Berkeley.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
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

#ifndef lint
#if 0
static char sccsid[] = "@(#)wwclose.c	8.1 (Berkeley) 6/6/93";
#else
static char rcsid[] = "$OpenBSD: wwclose.c,v 1.2 1996/06/26 05:43:37 deraadt Exp $";
#endif
#endif /* not lint */

#include "ww.h"

wwclose(w)
register struct ww *w;
{
	wwindex[w->ww_index] = 0;
	if (w->ww_pty >= 0)
		(void) close(w->ww_pty);
	if (w->ww_socket >= 0)
		(void) close(w->ww_socket);
	wwfree((char **)w->ww_win, w->ww_w.t);
	wwfree((char **)w->ww_buf, w->ww_b.t);
	if (w->ww_fmap != 0)
		wwfree((char **)w->ww_fmap, w->ww_w.t);
	free((char *)(w->ww_nvis + w->ww_w.t));
	if (w->ww_ob != 0)
		free(w->ww_ob);
	free((char *)w);
}
