/*	$OpenBSD: log.h,v 1.3 2016/07/18 21:14:30 benno Exp $ */

/*
 * Copyright (c) 2003, 2004 Henning Brauer <henning@openbsd.org>
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

#ifndef _LOG_H_
#define	_LOG_H_

#include <stdarg.h>

void	log_init(int);
void	log_verbose(int);
void	logit(int, const char *, ...)
		__attribute__((__format__ (printf, 2, 3)));
void	vlog(int, const char *, va_list)
		__attribute__((__format__ (printf, 2, 0)));
void	log_warn(const char *, ...)
		__attribute__((__format__ (printf, 1, 2)));
void	log_warnx(const char *, ...)
		__attribute__((__format__ (printf, 1, 2)));
void	log_info(const char *, ...)
		__attribute__((__format__ (printf, 1, 2)));
void	log_debug(const char *, ...)
		__attribute__((__format__ (printf, 1, 2)));
void	fatal(const char *) __dead
		__attribute__((__format__ (printf, 1, 0)));
void	fatalx(const char *) __dead
		__attribute__((__format__ (printf, 1, 0)));

const char	*log_in6addr(const struct in6_addr *);
const char	*log_in6addr_scope(const struct in6_addr *, unsigned int);
const char	*log_sockaddr(void *);
const char	*log_addr(int, union eigrpd_addr *);
const char	*log_prefix(struct rt_node *);
const char	*log_route_origin(int, struct rde_nbr *);
const char	*opcode_name(uint8_t);
const char	*af_name(int);
const char	*if_type_name(enum iface_type);
const char	*dual_state_name(int);
const char	*ext_proto_name(int);

#endif /* _LOG_H_ */
