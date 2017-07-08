/*	$OpenBSD: options.c,v 1.96 2017/07/08 00:36:10 krw Exp $	*/

/* DHCP options parsing and reassembly. */

/*
 * Copyright (c) 1995, 1996, 1997, 1998 The Internet Software Consortium.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of The Internet Software Consortium nor the names
 *    of its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INTERNET SOFTWARE CONSORTIUM AND
 * CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE INTERNET SOFTWARE CONSORTIUM OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This software has been written for the Internet Software Consortium
 * by Ted Lemon <mellon@fugue.com> in cooperation with Vixie
 * Enterprises.  To learn more about the Internet Software Consortium,
 * see ``http://www.vix.com/isc''.  To learn more about Vixie
 * Enterprises, see ``http://www.vix.com''.
 */

#include <sys/queue.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include <net/if.h>

#include <netinet/in.h>
#include <netinet/if_ether.h>

#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vis.h>

#include "dhcp.h"
#include "dhcpd.h"
#include "log.h"

int parse_option_buffer(struct option_data *, unsigned char *, int);
int expand_search_domain_name(unsigned char *, size_t, int *, unsigned char *);

/*
 * Parse options out of the specified buffer, storing addresses of
 * option values in options. Return 0 if errors, 1 if not.
 */
int
parse_option_buffer(struct option_data *options, unsigned char *buffer,
    int length)
{
	unsigned char *s, *t, *end = buffer + length;
	int len, code;

	for (s = buffer; *s != DHO_END && s < end; ) {
		code = s[0];

		/* Pad options don't have a length - just skip them. */
		if (code == DHO_PAD) {
			s++;
			continue;
		}

		/*
		 * All options other than DHO_PAD and DHO_END have a one-byte
		 * length field. It could be 0! Make sure that the length byte
		 * is present, and all the data is available.
		 */
		if (s + 1 < end) {
			len = s[1];
			if (s + 1 + len < end) {
				; /* option data is all there. */
			} else {
				log_warnx("option %s (%d) larger than buffer.",
				    dhcp_options[code].name, len);
				return (0);
			}
		} else {
			log_warnx("option %s has no length field.",
			    dhcp_options[code].name);
			return (0);
		}

		/*
		 * Strip trailing NULs from ascii ('t') options. They
		 * will be treated as DHO_PAD options. i.e. ignored. RFC 2132
		 * says "Options containing NVT ASCII data SHOULD NOT include
		 * a trailing NULL; however, the receiver of such options
		 * MUST be prepared to delete trailing nulls if they exist."
		 */
		if (dhcp_options[code].format[0] == 't') {
			while (len > 0 && s[len + 1] == '\0')
				len--;
		}

		/*
		 * If we haven't seen this option before, just make
		 * space for it and copy it there.
		 */
		if (!options[code].data) {
			if (!(t = calloc(1, len + 1)))
				fatalx("Can't allocate storage for option %s.",
				    dhcp_options[code].name);
			/*
			 * Copy and NUL-terminate the option (in case
			 * it's an ASCII string).
			 */
			memcpy(t, &s[2], len);
			t[len] = 0;
			options[code].len = len;
			options[code].data = t;
		} else {
			/*
			 * If it's a repeat, concatenate it to whatever
			 * we last saw.
			 */
			t = calloc(1, len + options[code].len + 1);
			if (!t)
				fatalx("Can't expand storage for option %s.",
				    dhcp_options[code].name);
			memcpy(t, options[code].data, options[code].len);
			memcpy(t + options[code].len, &s[2], len);
			options[code].len += len;
			t[options[code].len] = 0;
			free(options[code].data);
			options[code].data = t;
		}
		s += len + 2;
	}

	return (1);
}

/*
 * Pack as many options as fit in buflen bytes of buf. Return the
 * offset of the start of the last option copied. A caller can check
 * to see if it's DHO_END to decide if all the options were copied.
 */
int
pack_options(unsigned char *buf, int buflen, struct option_data *options)
{
	int ix, incr, length, bufix, code, lastopt = -1;

	memset(buf, 0, buflen);

	memcpy(buf, DHCP_OPTIONS_COOKIE, 4);
	if (options[DHO_DHCP_MESSAGE_TYPE].data) {
		memcpy(&buf[4], DHCP_OPTIONS_MESSAGE_TYPE, 3);
		buf[6] = options[DHO_DHCP_MESSAGE_TYPE].data[0];
		bufix = 7;
	} else
		bufix = 4;

	for (code = DHO_SUBNET_MASK; code < DHO_END; code++) {
		if (!options[code].data || code == DHO_DHCP_MESSAGE_TYPE)
			continue;

		length = options[code].len;
		if (bufix + length + 2*((length+254)/255) >= buflen)
			return (lastopt);

		lastopt = bufix;
		ix = 0;

		while (length) {
			incr = length > 255 ? 255 : length;

			buf[bufix++] = code;
			buf[bufix++] = incr;
			memcpy(buf + bufix, options[code].data + ix, incr);

			length -= incr;
			ix += incr;
			bufix += incr;
		}
	}

	if (bufix < buflen) {
		buf[bufix] = DHO_END;
		lastopt = bufix;
	}

	return (lastopt);
}

/*
 * Use vis() to encode characters of src and append encoded characters onto
 * dst. Also encode ", ', $, ` and \, to ensure resulting strings can be
 * represented as '"' delimited strings and safely passed to scripts. Surround
 * result with double quotes if emit_punct is true.
 */
char *
pretty_print_string(unsigned char *src, size_t srclen, int emit_punct)
{
	static char string[8196];
	char visbuf[5];
	unsigned char *origsrc = src;
	size_t rslt = 0;

	memset(string, 0, sizeof(string));

	if (emit_punct)
		rslt = strlcat(string, "\"", sizeof(string));

	for (; src < origsrc + srclen; src++) {
		if (*src && strchr("\"'$`\\", *src))
			vis(visbuf, *src, VIS_ALL | VIS_OCTAL, *src+1);
		else
			vis(visbuf, *src, VIS_OCTAL, *src+1);
		rslt = strlcat(string, visbuf, sizeof(string));
	}

	if (emit_punct)
		rslt = strlcat(string, "\"", sizeof(string));

	if (rslt >= sizeof(string))
		return (NULL);

	return (string);
}

/*
 * Must special case *_CLASSLESS_* route options due to the variable size
 * of the CIDR element in its CIA format.
 */
char *
pretty_print_classless_routes(unsigned char *src, size_t srclen)
{
	static char string[8196];
	char bitsbuf[5];	/* to hold "/nn " */
	struct in_addr net, gateway;
	unsigned int bytes;
	int bits, rslt;

	memset(string, 0, sizeof(string));

	while (srclen) {
		bits = *src;
		src++;
		srclen--;

		bytes = (bits + 7) / 8;
		if (srclen < (bytes + sizeof(gateway.s_addr)) ||
		    bytes > sizeof(net.s_addr))
			return (NULL);
		rslt = snprintf(bitsbuf, sizeof(bitsbuf), "/%d ", bits);
		if (rslt == -1 || (unsigned int)rslt >= sizeof(bitsbuf))
			return (NULL);

		memset(&net, 0, sizeof(net));
		memcpy(&net.s_addr, src, bytes);
		src += bytes;
		srclen -= bytes;

		memcpy(&gateway.s_addr, src, sizeof(gateway.s_addr));
		src += sizeof(gateway.s_addr);
		srclen -= sizeof(gateway.s_addr);

		if (strlen(string) > 0)
			strlcat(string, ", ", sizeof(string));
		strlcat(string, inet_ntoa(net), sizeof(string));
		strlcat(string, bitsbuf, sizeof(string));
		if (strlcat(string, inet_ntoa(gateway), sizeof(string)) >=
		    sizeof(string))
			return (NULL);
	}

	return (string);
}

int
expand_search_domain_name(unsigned char *src, size_t srclen, int *offset,
    unsigned char *domain_search)
{
	unsigned int i;
	int domain_name_len, label_len, pointer, pointed_len;
	char *cursor;

	cursor = domain_search + strlen(domain_search);
	domain_name_len = 0;

	i = *offset;
	while (i <= srclen) {
		label_len = src[i];
		if (label_len == 0) {
			/*
			 * A zero-length label marks the end of this
			 * domain name.
			 */
			*offset = i + 1;
			return (domain_name_len);
		} else if (label_len & 0xC0) {
			/* This is a pointer to another list of labels. */
			if (i + 1 >= srclen) {
				/* The pointer is truncated. */
				log_warnx("Truncated pointer in DHCP Domain "
				    "Search option.");
				return (-1);
			}

			pointer = ((label_len & ~(0xC0)) << 8) + src[i + 1];
			if (pointer >= *offset) {
				/*
				 * The pointer must indicates a prior
				 * occurance.
				 */
				log_warnx("Invalid forward pointer in DHCP "
				    "Domain Search option compression.");
				return (-1);
			}

			pointed_len = expand_search_domain_name(src, srclen,
			    &pointer, domain_search);
			domain_name_len += pointed_len;

			*offset = i + 2;
			return (domain_name_len);
		}
		if (i + label_len + 1 > srclen) {
			log_warnx("Truncated label in DHCP Domain Search "
			    "option.");
			return (-1);
		}
		/*
		 * Update the domain name length with the length of the
		 * current label, plus a trailing dot ('.').
		 */
		domain_name_len += label_len + 1;

		if (strlen(domain_search) + domain_name_len >=
		    DHCP_DOMAIN_SEARCH_LEN) {
			log_warnx("Domain search list too long.");
			return (-1);
		}

		/* Copy the label found. */
		memcpy(cursor, src + i + 1, label_len);
		cursor[label_len] = '.';

		/* Move cursor. */
		i += label_len + 1;
		cursor += label_len + 1;
	}

	log_warnx("Truncated DHCP Domain Search option.");

	return (-1);
}

/*
 * Must special case DHO_DOMAIN_SEARCH because it is encoded as described
 * in RFC 1035 section 4.1.4.
 */
char *
pretty_print_domain_search(unsigned char *src, size_t srclen)
{
	static char domain_search[DHCP_DOMAIN_SEARCH_LEN];
	unsigned int offset;
	int len, expanded_len, domains;
	unsigned char *cursor;

	memset(domain_search, 0, sizeof(domain_search));

	/* Compute expanded length. */
	expanded_len = len = 0;
	domains = 0;
	offset = 0;
	while (offset < srclen) {
		cursor = domain_search + strlen(domain_search);
		if (domain_search[0]) {
			*cursor = ' ';
			expanded_len++;
		}
		len = expand_search_domain_name(src, srclen, &offset,
		    domain_search);
		if (len == -1)
			return (NULL);
		domains++;
		expanded_len += len;
		if (domains > DHCP_DOMAIN_SEARCH_CNT)
			return (NULL);
	}

	return (domain_search);
}

/*
 * Format the specified option so that a human can easily read it.
 */
char *
pretty_print_option(unsigned int code, struct option_data *option,
    int emit_punct)
{
	static char	 optbuf[8192]; /* XXX */
	char		 fmtbuf[32];
	struct in_addr	 foo;
	unsigned char	*data = option->data;
	unsigned char	*dp = data;
	char		*op = optbuf, *buf;
	int		 hunksize = 0, numhunk = -1, numelem = 0;
	int		 i, j, k, opleft = sizeof(optbuf);
	int		 len = option->len;
	int		 opcount = 0;
	int32_t		 int32val;
	uint32_t	 uint32val;
	uint16_t	 uint16val;
	char		 comma;

	memset(optbuf, 0, sizeof(optbuf));

	/* Code should be between 0 and 255. */
	if (code > 255) {
		log_warnx("pretty_print_option: bad code %d", code);
		goto done;
	}

	if (emit_punct)
		comma = ',';
	else
		comma = ' ';

	/* Handle the princess class options with weirdo formats. */
	switch (code) {
	case DHO_CLASSLESS_STATIC_ROUTES:
	case DHO_CLASSLESS_MS_STATIC_ROUTES:
		buf = pretty_print_classless_routes(dp, len);
		if (buf == NULL)
			goto toobig;
		strlcat(optbuf, buf, sizeof(optbuf));
		goto done;
	default:
		break;
	}

	/* Figure out the size of the data. */
	for (i = 0; dhcp_options[code].format[i]; i++) {
		if (!numhunk) {
			log_warnx("%s: Excess information in format string: "
			    "%s", dhcp_options[code].name,
			    &(dhcp_options[code].format[i]));
			goto done;
		}
		numelem++;
		fmtbuf[i] = dhcp_options[code].format[i];
		switch (dhcp_options[code].format[i]) {
		case 'A':
			--numelem;
			fmtbuf[i] = 0;
			numhunk = 0;
			if (hunksize == 0) {
				log_warnx("%s: no size indicator before A"
				    " in format string: %s",
				    dhcp_options[code].name,
				    dhcp_options[code].format);
				goto done;
			}
			break;
		case 'X':
			for (k = 0; k < len; k++)
				if (!isascii(data[k]) ||
				    !isprint(data[k]))
					break;
			if (k == len) {
				fmtbuf[i] = 't';
				numhunk = -2;
			} else {
				hunksize++;
				comma = ':';
				numhunk = 0;
			}
			fmtbuf[i + 1] = 0;
			break;
		case 't':
			fmtbuf[i + 1] = 0;
			numhunk = -2;
			break;
		case 'I':
		case 'l':
		case 'L':
			hunksize += 4;
			break;
		case 'S':
			hunksize += 2;
			break;
		case 'B':
		case 'f':
			hunksize++;
			break;
		case 'e':
			break;
		default:
			log_warnx("%s: garbage in format string: %s",
			    dhcp_options[code].name,
			    &(dhcp_options[code].format[i]));
			goto done;
		}
	}

	/* Check for too few bytes. */
	if (hunksize > len) {
		log_warnx("%s: expecting at least %d bytes; got %d",
		    dhcp_options[code].name, hunksize, len);
		goto done;
	}
	/* Check for too many bytes. */
	if (numhunk == -1 && hunksize < len) {
		log_warnx("%s: expecting only %d bytes: got %d",
		    dhcp_options[code].name, hunksize, len);
		goto done;
	}

	/* If this is an array, compute its size. */
	if (!numhunk)
		numhunk = len / hunksize;
	/* See if we got an exact number of hunks. */
	if (numhunk > 0 && numhunk * hunksize != len) {
		log_warnx("%s: expecting %d bytes: got %d",
		    dhcp_options[code].name, numhunk * hunksize, len);
		goto done;
	}

	/* A one-hunk array prints the same as a single hunk. */
	if (numhunk < 0)
		numhunk = 1;

	/* Cycle through the array (or hunk) printing the data. */
	for (i = 0; i < numhunk; i++) {
		for (j = 0; j < numelem; j++) {
			switch (fmtbuf[j]) {
			case 't':
				buf = pretty_print_string(dp, len, emit_punct);
				if (buf == NULL)
					opcount = -1;
				else
					opcount = strlcat(op, buf, opleft);
				break;
			case 'I':
				memcpy(&foo.s_addr, dp, sizeof(foo.s_addr));
				opcount = snprintf(op, opleft, "%s",
				    inet_ntoa(foo));
				dp += sizeof(foo.s_addr);
				break;
			case 'l':
				memcpy(&int32val, dp, sizeof(int32val));
				opcount = snprintf(op, opleft, "%d",
				    ntohl(int32val));
				dp += sizeof(int32val);
				break;
			case 'L':
				memcpy(&uint32val, dp, sizeof(uint32val));
				opcount = snprintf(op, opleft, "%u",
				    ntohl(uint32val));
				dp += sizeof(uint32val);
				break;
			case 'S':
				memcpy(&uint16val, dp, sizeof(uint16val));
				opcount = snprintf(op, opleft, "%hu",
				    ntohs(uint16val));
				dp += sizeof(uint16val);
				break;
			case 'B':
				opcount = snprintf(op, opleft, "%u", *dp);
				dp++;
				break;
			case 'X':
				opcount = snprintf(op, opleft, "%x", *dp);
				dp++;
				break;
			case 'f':
				opcount = snprintf(op, opleft, "%s",
				    *dp ? "true" : "false");
				dp++;
				break;
			default:
				log_warnx("Unexpected format code %c",
				    fmtbuf[j]);
				goto toobig;
			}
			if (opcount >= opleft || opcount == -1)
				goto toobig;
			opleft -= opcount;
			op += opcount;
			if (j + 1 < numelem && comma != ':') {
				opcount = snprintf(op, opleft, " ");
				if (opcount >= opleft || opcount == -1)
					goto toobig;
				opleft -= opcount;
				op += opcount;
			}
		}
		if (i + 1 < numhunk) {
			opcount = snprintf(op, opleft, "%c", comma);
			if (opcount >= opleft || opcount == -1)
				goto toobig;
			opleft -= opcount;
			op += opcount;
		}
	}

done:
	return (optbuf);

toobig:
	memset(optbuf, 0, sizeof(optbuf));
	return (optbuf);
}

struct option_data *
unpack_options(struct dhcp_packet *packet)
{
	static struct option_data options[DHO_COUNT];
	int i;

	for (i = 0; i < DHO_COUNT; i++) {
		free(options[i].data);
		options[i].data = NULL;
		options[i].len = 0;
	}

	if (memcmp(&packet->options, DHCP_OPTIONS_COOKIE, 4) == 0) {
		/* Parse the BOOTP/DHCP options field. */
		parse_option_buffer(options, &packet->options[4],
		    sizeof(packet->options) - 4);

		/* DHCP packets can also use overload areas for options. */
		if (options[DHO_DHCP_MESSAGE_TYPE].data &&
		    options[DHO_DHCP_OPTION_OVERLOAD].data) {
			if (options[DHO_DHCP_OPTION_OVERLOAD].data[0] & 1)
				parse_option_buffer(options,
				    (unsigned char *)packet->file,
				    sizeof(packet->file));
			if (options[DHO_DHCP_OPTION_OVERLOAD].data[0] & 2)
				parse_option_buffer(options,
				    (unsigned char *)packet->sname,
				    sizeof(packet->sname));
		}
	}

	return options;
}
