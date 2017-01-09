/*	$OpenBSD: util.c,v 1.7 2017/01/09 14:49:22 reyk Exp $	*/
/*
 * Copyright (c) 2014 Bret Stephen Lambert <blambert@openbsd.org>
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

#include <sys/types.h>
#include <sys/queue.h>
#include <sys/socket.h>

#include <net/if.h>

#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <event.h>

#include "ber.h"
#include "snmp.h"
#include "snmpd.h"

/*
 * Convert variable bindings from AgentX to SNMP dialect.
 */
int
varbind_convert(struct agentx_pdu *pdu, struct agentx_varbind_hdr *vbhdr,
    struct ber_element **varbind, struct ber_element **iter)
{
	struct ber_oid			 oid;
	u_int32_t			 d;
	u_int64_t			 l;
	int				 slen;
	char				*str;
	struct ber_element		*a;
	int				 ret = AGENTX_ERR_NONE;

	if (snmp_agentx_read_oid(pdu, (struct snmp_oid *)&oid) == -1) {
		ret = AGENTX_ERR_PARSE_ERROR;
		goto done;
	}

	*iter = ber_add_sequence(*iter);
	if (*varbind == NULL)
		*varbind = *iter;

	a = ber_add_oid(*iter, &oid);

	switch (vbhdr->type) {
	case AGENTX_NO_SUCH_OBJECT:
	case AGENTX_NO_SUCH_INSTANCE:
	case AGENTX_END_OF_MIB_VIEW:
	case AGENTX_NULL:
		a = ber_add_null(a);
		break;

	case AGENTX_IP_ADDRESS:
	case AGENTX_OPAQUE:
	case AGENTX_OCTET_STRING:
		str = snmp_agentx_read_octetstr(pdu, &slen);
		if (str == NULL) {
			ret = AGENTX_ERR_PARSE_ERROR;
			goto done;
		}
		a = ber_add_nstring(a, str, slen);
		break;

	case AGENTX_OBJECT_IDENTIFIER:
		if (snmp_agentx_read_oid(pdu,
		    (struct snmp_oid *)&oid) == -1) {
			ret = AGENTX_ERR_PARSE_ERROR;
			goto done;
		}
		a = ber_add_oid(a, &oid);
		break;

	case AGENTX_INTEGER:
	case AGENTX_COUNTER32:
	case AGENTX_GAUGE32:
	case AGENTX_TIME_TICKS:
		if (snmp_agentx_read_int(pdu, &d) == -1) {
			ret = AGENTX_ERR_PARSE_ERROR;
			goto done;
		}
		a = ber_add_integer(a, d);
		break;

	case AGENTX_COUNTER64:
		if (snmp_agentx_read_int64(pdu, &l) == -1) {
			ret = AGENTX_ERR_PARSE_ERROR;
			goto done;
		}
		a = ber_add_integer(a, l);
		break;

	default:
		log_debug("unknown data type '%i'", vbhdr->type);
		ret = AGENTX_ERR_PARSE_ERROR;
		goto done;
	}

	/* AgentX types correspond to BER types */
	switch (vbhdr->type) {
	case BER_TYPE_INTEGER:
	case BER_TYPE_BITSTRING:
	case BER_TYPE_OCTETSTRING:
	case BER_TYPE_NULL:
	case BER_TYPE_OBJECT:
		/* universal types */
		break;

	/* Convert AgentX error types to SNMP error types */
	case AGENTX_NO_SUCH_OBJECT:
		ber_set_header(a, BER_CLASS_CONTEXT, 0);
		break;
	case AGENTX_NO_SUCH_INSTANCE:
		ber_set_header(a, BER_CLASS_CONTEXT, 1);
		break;

	case AGENTX_COUNTER32:
		ber_set_header(a, BER_CLASS_APPLICATION, SNMP_COUNTER32);
		break;

	case AGENTX_GAUGE32:
		ber_set_header(a, BER_CLASS_APPLICATION, SNMP_GAUGE32);
		break;

	case AGENTX_COUNTER64:
		ber_set_header(a, BER_CLASS_APPLICATION, SNMP_COUNTER64);
		break;

	case AGENTX_IP_ADDRESS:
		/* application 0 implicit 4-byte octet string per SNMPv2-SMI */
		break;

	default:
		/* application-specific types */
		ber_set_header(a, BER_CLASS_APPLICATION, vbhdr->type);
		break;
	}
 done:
	return (ret);
}

ssize_t
sendtofrom(int s, void *buf, size_t len, int flags, struct sockaddr *to,
    socklen_t tolen, struct sockaddr *from, socklen_t fromlen)
{
	struct iovec		 iov;
	struct msghdr		 msg;
	struct cmsghdr		*cmsg;
	struct in6_pktinfo	*pkt6;
	struct sockaddr_in	*in;
	struct sockaddr_in6	*in6;
	union {
		struct cmsghdr	hdr;
		char		inbuf[CMSG_SPACE(sizeof(struct in_addr))];
		char		in6buf[CMSG_SPACE(sizeof(struct in6_pktinfo))];
	} cmsgbuf;

	bzero(&msg, sizeof(msg));
	bzero(&cmsgbuf, sizeof(cmsgbuf));

	iov.iov_base = buf;
	iov.iov_len = len;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_name = to;
	msg.msg_namelen = tolen;
	msg.msg_control = &cmsgbuf;
	msg.msg_controllen = sizeof(cmsgbuf);

	cmsg = CMSG_FIRSTHDR(&msg);
	switch (to->sa_family) {
	case AF_INET:
		msg.msg_controllen = sizeof(cmsgbuf.inbuf);
		cmsg->cmsg_len = CMSG_LEN(sizeof(struct in_addr));
		cmsg->cmsg_level = IPPROTO_IP;
		cmsg->cmsg_type = IP_SENDSRCADDR;
		in = (struct sockaddr_in *)from;
		memcpy(CMSG_DATA(cmsg), &in->sin_addr, sizeof(struct in_addr));
		break;
	case AF_INET6:
		msg.msg_controllen = sizeof(cmsgbuf.in6buf);
		cmsg->cmsg_len = CMSG_LEN(sizeof(struct in6_pktinfo));
		cmsg->cmsg_level = IPPROTO_IPV6;
		cmsg->cmsg_type = IPV6_PKTINFO;
		in6 = (struct sockaddr_in6 *)from;
		pkt6 = (struct in6_pktinfo *)CMSG_DATA(cmsg);
		pkt6->ipi6_addr = in6->sin6_addr;
		break;
	}

	return sendmsg(s, &msg, flags);
}

ssize_t
recvfromto(int s, void *buf, size_t len, int flags, struct sockaddr *from,
    socklen_t *fromlen, struct sockaddr *to, socklen_t *tolen)
{
	struct iovec		 iov;
	struct msghdr		 msg;
	struct cmsghdr		*cmsg;
	struct in6_pktinfo	*pkt6;
	struct sockaddr_in	*in;
	struct sockaddr_in6	*in6;
	ssize_t			 ret;
	union {
		struct cmsghdr hdr;
		char	buf[CMSG_SPACE(sizeof(struct sockaddr_storage))];
	} cmsgbuf;

	bzero(&msg, sizeof(msg));
	bzero(&cmsgbuf.buf, sizeof(cmsgbuf.buf));

	iov.iov_base = buf;
	iov.iov_len = len;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_name = from;
	msg.msg_namelen = *fromlen;
	msg.msg_control = &cmsgbuf.buf;
	msg.msg_controllen = sizeof(cmsgbuf.buf);

	if ((ret = recvmsg(s, &msg, flags)) == -1)
		return (-1);

	*fromlen = from->sa_len;
	*tolen = 0;

	if (getsockname(s, to, tolen) != 0)
		*tolen = 0;

	for (cmsg = CMSG_FIRSTHDR(&msg); cmsg != NULL;
	    cmsg = CMSG_NXTHDR(&msg, cmsg)) {
		switch (from->sa_family) {
		case AF_INET:
			if (cmsg->cmsg_level == IPPROTO_IP &&
			    cmsg->cmsg_type == IP_RECVDSTADDR) {
				in = (struct sockaddr_in *)to;
				in->sin_family = AF_INET;
				in->sin_len = *tolen = sizeof(*in);
				memcpy(&in->sin_addr, CMSG_DATA(cmsg),
				    sizeof(struct in_addr));
			}
			break;
		case AF_INET6:
			if (cmsg->cmsg_level == IPPROTO_IPV6 &&
			    cmsg->cmsg_type == IPV6_PKTINFO) {
				in6 = (struct sockaddr_in6 *)to;
				in6->sin6_family = AF_INET6;
				in6->sin6_len = *tolen = sizeof(*in6);
				pkt6 = (struct in6_pktinfo *)CMSG_DATA(cmsg);
				memcpy(&in6->sin6_addr, &pkt6->ipi6_addr,
				    sizeof(struct in6_addr));
				if (IN6_IS_ADDR_LINKLOCAL(&in6->sin6_addr))
					in6->sin6_scope_id =
					    pkt6->ipi6_ifindex;
			}
			break;
		}
	}

	return (ret);
}

void
print_debug(const char *emsg, ...)
{
	va_list	 ap;

	if (log_getverbose() > 2) {
		va_start(ap, emsg);
		vfprintf(stderr, emsg, ap);
		va_end(ap);
	}
}

void
print_verbose(const char *emsg, ...)
{
	va_list	 ap;

	if (log_getverbose()) {
		va_start(ap, emsg);
		vfprintf(stderr, emsg, ap);
		va_end(ap);
	}
}

const char *
log_in6addr(const struct in6_addr *addr)
{
	static char		buf[NI_MAXHOST];
	struct sockaddr_in6	sa_in6;
	u_int16_t		tmp16;

	bzero(&sa_in6, sizeof(sa_in6));
	sa_in6.sin6_len = sizeof(sa_in6);
	sa_in6.sin6_family = AF_INET6;
	memcpy(&sa_in6.sin6_addr, addr, sizeof(sa_in6.sin6_addr));

	/* XXX thanks, KAME, for this ugliness... adopted from route/show.c */
	if (IN6_IS_ADDR_LINKLOCAL(&sa_in6.sin6_addr) ||
	    IN6_IS_ADDR_MC_LINKLOCAL(&sa_in6.sin6_addr)) {
		memcpy(&tmp16, &sa_in6.sin6_addr.s6_addr[2], sizeof(tmp16));
		sa_in6.sin6_scope_id = ntohs(tmp16);
		sa_in6.sin6_addr.s6_addr[2] = 0;
		sa_in6.sin6_addr.s6_addr[3] = 0;
	}

	return (print_host((struct sockaddr_storage *)&sa_in6, buf,
	    NI_MAXHOST));
}

const char *
print_host(struct sockaddr_storage *ss, char *buf, size_t len)
{
	if (getnameinfo((struct sockaddr *)ss, ss->ss_len,
	    buf, len, NULL, 0, NI_NUMERICHOST) != 0) {
		buf[0] = '\0';
		return (NULL);
	}
	return (buf);
}
