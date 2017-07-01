/*	$OpenBSD: privsep.c,v 1.53 2017/07/01 23:27:56 krw Exp $ */

/*
 * Copyright (c) 2004 Henning Brauer <henning@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF MIND, USE, DATA OR PROFITS, WHETHER IN
 * AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE, ABUSE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/queue.h>
#include <sys/socket.h>

#include <net/if.h>

#include <netinet/in.h>
#include <netinet/if_ether.h>

#include <errno.h>
#include <imsg.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

#include "dhcp.h"
#include "dhcpd.h"
#include "log.h"
#include "privsep.h"

int
dispatch_imsg(struct interface_info *ifi, int ioctlfd, int routefd,
    struct imsgbuf *ibuf)
{
	struct imsg	imsg;
	ssize_t		n;

	for (;;) {
		if ((n = imsg_get(ibuf, &imsg)) == -1)
			fatal("dispatch_imsg: imsg_get failure");

		if (n == 0)
			break;

		switch (imsg.hdr.type) {
		case IMSG_DELETE_ADDRESS:
			if (imsg.hdr.len != IMSG_HEADER_SIZE +
			    sizeof(struct imsg_delete_address))
				log_warnx("bad IMSG_DELETE_ADDRESS");
			else
				priv_delete_address(ifi->name, ioctlfd,
				    imsg.data);
			break;

		case IMSG_ADD_ADDRESS:
			if (imsg.hdr.len != IMSG_HEADER_SIZE +
			    sizeof(struct imsg_add_address))
				log_warnx("bad IMSG_ADD_ADDRESS");
			else
				priv_add_address(ifi->name, ioctlfd, imsg.data);
			break;

		case IMSG_FLUSH_ROUTES:
			if (imsg.hdr.len != IMSG_HEADER_SIZE)
				log_warnx("bad IMSG_FLUSH_ROUTES");
			else
				priv_flush_routes(ifi->name, routefd,
				    ifi->rdomain);
			break;

		case IMSG_ADD_ROUTE:
			if (imsg.hdr.len != IMSG_HEADER_SIZE +
			    sizeof(struct imsg_add_route))
				log_warnx("bad IMSG_ADD_ROUTE");
			else
				priv_add_route(ifi->rdomain, imsg.data);
			break;

		case IMSG_SET_INTERFACE_MTU:
			if (imsg.hdr.len != IMSG_HEADER_SIZE +
			    sizeof(struct imsg_set_interface_mtu))
				log_warnx("bad IMSG_SET_INTERFACE_MTU");
			else
				priv_set_interface_mtu(ifi->name, ioctlfd,
				    imsg.data);
			break;

		case IMSG_WRITE_RESOLV_CONF:
			if (imsg.hdr.len <= IMSG_HEADER_SIZE)
				log_warnx("short IMSG_WRITE_RESOLV_CONF");
			else if (resolv_conf_priority(ifi->rdomain))
				priv_write_resolv_conf(imsg.data,
				    imsg.hdr.len - IMSG_HEADER_SIZE);
			break;

		case IMSG_HUP:
			if (imsg.hdr.len != IMSG_HEADER_SIZE)
				log_warnx("bad IMSG_HUP");
			else {
				imsg_free(&imsg);
				return 1;
			}
			break;

		default:
			log_warnx("received unknown message, code %u",
			    imsg.hdr.type);
		}

		imsg_free(&imsg);
	}
	return 0;
}
