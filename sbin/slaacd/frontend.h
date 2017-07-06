/*	$OpenBSD: frontend.h,v 1.2 2017/07/06 15:02:53 florian Exp $	*/

/*
 * Copyright (c) 2004, 2005 Esben Norby <norby@openbsd.org>
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

#ifndef	SMALL
TAILQ_HEAD(ctl_conns, ctl_conn)	ctl_conns;
#endif	/* SMALL */

void		 frontend(int, int, char *);
void		 frontend_dispatch_main(int, short, void *);
void		 frontend_dispatch_engine(int, short, void *);
int		 frontend_imsg_compose_main(int, pid_t, void *, uint16_t);
int		 frontend_imsg_compose_engine(int, uint32_t, pid_t, void *,
		     uint16_t);
