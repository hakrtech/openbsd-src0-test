/*
 * Copyright (c) 1999 - 2001 Kungliga Tekniska H�gskolan
 * (Royal Institute of Technology, Stockholm, Sweden).
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
RCSID("$KTH: getnameinfo.c,v 1.3 2001/02/09 14:45:30 assar Exp $");
#endif

#include "roken.h"

static int
doit (int af,
      const void *addr,
      size_t addrlen,
      int port,
      char *host, size_t hostlen,
      char *serv, size_t servlen,
      int flags)
{
    if (host != NULL) {
	if (flags & NI_NUMERICHOST) {
	    if (inet_ntop (af, addr, host, hostlen) == NULL)
		return EAI_SYSTEM;
	} else {
	    struct hostent *he = gethostbyaddr (addr,
						addrlen,
						af);
	    if (he != NULL) {
		strlcpy (host, he->h_name, hostlen);
		if (flags & NI_NOFQDN) {
		    char *dot = strchr (host, '.');
		    if (dot != NULL)
			*dot = '\0';
		}
	    } else if (flags & NI_NAMEREQD) {
		return EAI_NONAME;
	    } else if (inet_ntop (af, addr, host, hostlen) == NULL)
		return EAI_SYSTEM;
	}
    }

    if (serv != NULL) {
	if (flags & NI_NUMERICSERV) {
	    snprintf (serv, servlen, "%u", ntohs(port));
	} else {
	    const char *proto = "tcp";
	    struct servent *se;

	    if (flags & NI_DGRAM)
		proto = "udp";

	    se = getservbyport (port, proto);
	    if (se == NULL) {
		snprintf (serv, servlen, "%u", ntohs(port));
	    } else {
		strlcpy (serv, se->s_name, servlen);
	    }
	}
    }
    return 0;
}

/*
 *
 */

int
getnameinfo(const struct sockaddr *sa, socklen_t salen,
	    char *host, size_t hostlen,
	    char *serv, size_t servlen,
	    int flags)
{
    switch (sa->sa_family) {
#ifdef HAVE_IPV6
    case AF_INET6 : {
	const struct sockaddr_in6 *sin6 = (const struct sockaddr_in6 *)sa;

	return doit (AF_INET6, &sin6->sin6_addr, sizeof(sin6->sin6_addr),
		     sin6->sin6_port,
		     host, hostlen,
		     serv, servlen,
		     flags);
    }
#endif
    case AF_INET : {
	const struct sockaddr_in *sin = (const struct sockaddr_in *)sa;

	return doit (AF_INET, &sin->sin_addr, sizeof(sin->sin_addr),
		     sin->sin_port,
		     host, hostlen,
		     serv, servlen,
		     flags);
    }
    default :
	return EAI_FAMILY;
    }
}
