/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 */

#include "includes.h"
RCSID("$OpenBSD: auth-options.c,v 1.23 2002/03/19 10:35:39 markus Exp $");

#include "packet.h"
#include "xmalloc.h"
#include "match.h"
#include "log.h"
#include "canohost.h"
#include "channels.h"
#include "auth-options.h"
#include "servconf.h"
#include "bufaux.h"
#include "misc.h"
#include "monitor_wrap.h"

/* Debugging messages */
Buffer auth_debug;
int auth_debug_init;

/* Flags set authorized_keys flags */
int no_port_forwarding_flag = 0;
int no_agent_forwarding_flag = 0;
int no_x11_forwarding_flag = 0;
int no_pty_flag = 0;

/* "command=" option. */
char *forced_command = NULL;

/* "environment=" options. */
struct envstring *custom_environment = NULL;

extern ServerOptions options;

static void
auth_send_debug(Buffer *m)
{
	char *msg;

	while (buffer_len(m)) {
		msg = buffer_get_string(m, NULL);
		packet_send_debug("%s", msg);
		xfree(msg);
	}
}

void
auth_clear_options(void)
{
	if (auth_debug_init)
		buffer_clear(&auth_debug);
	else {
		buffer_init(&auth_debug);
		auth_debug_init = 1;
	}

	no_agent_forwarding_flag = 0;
	no_port_forwarding_flag = 0;
	no_pty_flag = 0;
	no_x11_forwarding_flag = 0;
	while (custom_environment) {
		struct envstring *ce = custom_environment;
		custom_environment = ce->next;
		xfree(ce->s);
		xfree(ce);
	}
	if (forced_command) {
		xfree(forced_command);
		forced_command = NULL;
	}
	channel_clear_permitted_opens();
}

/*
 * return 1 if access is granted, 0 if not.
 * side effect: sets key option flags
 */
int
auth_parse_options(struct passwd *pw, char *opts, char *file, u_long linenum)
{
	char tmp[1024];
	const char *cp;
	int i;

	/* reset options */
	auth_clear_options();

	if (!opts)
		return 1;

	while (*opts && *opts != ' ' && *opts != '\t') {
		cp = "no-port-forwarding";
		if (strncasecmp(opts, cp, strlen(cp)) == 0) {
			snprintf(tmp, sizeof(tmp), "Port forwarding disabled.");
			buffer_put_cstring(&auth_debug, tmp);
			no_port_forwarding_flag = 1;
			opts += strlen(cp);
			goto next_option;
		}
		cp = "no-agent-forwarding";
		if (strncasecmp(opts, cp, strlen(cp)) == 0) {
			snprintf(tmp, sizeof(tmp), "Agent forwarding disabled.");
			buffer_put_cstring(&auth_debug, tmp);
			no_agent_forwarding_flag = 1;
			opts += strlen(cp);
			goto next_option;
		}
		cp = "no-X11-forwarding";
		if (strncasecmp(opts, cp, strlen(cp)) == 0) {
			snprintf(tmp, sizeof(tmp), "X11 forwarding disabled.");
			buffer_put_cstring(&auth_debug, tmp);
			no_x11_forwarding_flag = 1;
			opts += strlen(cp);
			goto next_option;
		}
		cp = "no-pty";
		if (strncasecmp(opts, cp, strlen(cp)) == 0) {
			snprintf(tmp, sizeof(tmp), "Pty allocation disabled.");
			buffer_put_cstring(&auth_debug, tmp);
			no_pty_flag = 1;
			opts += strlen(cp);
			goto next_option;
		}
		cp = "command=\"";
		if (strncasecmp(opts, cp, strlen(cp)) == 0) {
			opts += strlen(cp);
			forced_command = xmalloc(strlen(opts) + 1);
			i = 0;
			while (*opts) {
				if (*opts == '"')
					break;
				if (*opts == '\\' && opts[1] == '"') {
					opts += 2;
					forced_command[i++] = '"';
					continue;
				}
				forced_command[i++] = *opts++;
			}
			if (!*opts) {
				debug("%.100s, line %lu: missing end quote",
				    file, linenum);
				snprintf(tmp, sizeof(tmp), "%.100s, line %lu: missing end quote",
				    file, linenum);
				buffer_put_cstring(&auth_debug, tmp);
				xfree(forced_command);
				forced_command = NULL;
				goto bad_option;
			}
			forced_command[i] = 0;
			snprintf(tmp, sizeof(tmp), "Forced command: %.900s", forced_command);
			buffer_put_cstring(&auth_debug, tmp);
			opts++;
			goto next_option;
		}
		cp = "environment=\"";
		if (strncasecmp(opts, cp, strlen(cp)) == 0) {
			char *s;
			struct envstring *new_envstring;

			opts += strlen(cp);
			s = xmalloc(strlen(opts) + 1);
			i = 0;
			while (*opts) {
				if (*opts == '"')
					break;
				if (*opts == '\\' && opts[1] == '"') {
					opts += 2;
					s[i++] = '"';
					continue;
				}
				s[i++] = *opts++;
			}
			if (!*opts) {
				debug("%.100s, line %lu: missing end quote",
				    file, linenum);
				snprintf(tmp, sizeof(tmp), "%.100s, line %lu: missing end quote",
				    file, linenum);
				buffer_put_cstring(&auth_debug, tmp);
				xfree(s);
				goto bad_option;
			}
			s[i] = 0;
			snprintf(tmp, sizeof(tmp), "Adding to environment: %.900s", s);
			buffer_put_cstring(&auth_debug, tmp);
			debug("Adding to environment: %.900s", s);
			opts++;
			new_envstring = xmalloc(sizeof(struct envstring));
			new_envstring->s = s;
			new_envstring->next = custom_environment;
			custom_environment = new_envstring;
			goto next_option;
		}
		cp = "from=\"";
		if (strncasecmp(opts, cp, strlen(cp)) == 0) {
			const char *remote_ip = get_remote_ipaddr();
			const char *remote_host = get_canonical_hostname(
			    options.verify_reverse_mapping);
			char *patterns = xmalloc(strlen(opts) + 1);

			opts += strlen(cp);
			i = 0;
			while (*opts) {
				if (*opts == '"')
					break;
				if (*opts == '\\' && opts[1] == '"') {
					opts += 2;
					patterns[i++] = '"';
					continue;
				}
				patterns[i++] = *opts++;
			}
			if (!*opts) {
				debug("%.100s, line %lu: missing end quote",
				    file, linenum);
				snprintf(tmp, sizeof(tmp), "%.100s, line %lu: missing end quote",
				    file, linenum);
				buffer_put_cstring(&auth_debug, tmp);
				xfree(patterns);
				goto bad_option;
			}
			patterns[i] = 0;
			opts++;
			if (match_host_and_ip(remote_host, remote_ip,
			    patterns) != 1) {
				xfree(patterns);
				log("Authentication tried for %.100s with "
				    "correct key but not from a permitted "
				    "host (host=%.200s, ip=%.200s).",
				    pw->pw_name, remote_host, remote_ip);
				snprintf(tmp, sizeof(tmp),
				    "Your host '%.200s' is not "
				    "permitted to use this key for login.",
				    remote_host);
				buffer_put_cstring(&auth_debug, tmp);
				/* deny access */
				return 0;
			}
			xfree(patterns);
			/* Host name matches. */
			goto next_option;
		}
		cp = "permitopen=\"";
		if (strncasecmp(opts, cp, strlen(cp)) == 0) {
			char host[256], sport[6];
			u_short port;
			char *patterns = xmalloc(strlen(opts) + 1);

			opts += strlen(cp);
			i = 0;
			while (*opts) {
				if (*opts == '"')
					break;
				if (*opts == '\\' && opts[1] == '"') {
					opts += 2;
					patterns[i++] = '"';
					continue;
				}
				patterns[i++] = *opts++;
			}
			if (!*opts) {
				debug("%.100s, line %lu: missing end quote",
				    file, linenum);
				snprintf(tmp, sizeof(tmp), "%.100s, line %lu: missing end quote",
				    file, linenum);
				buffer_put_cstring(&auth_debug, tmp);
				xfree(patterns);
				goto bad_option;
			}
			patterns[i] = 0;
			opts++;
			if (sscanf(patterns, "%255[^:]:%5[0-9]", host, sport) != 2 &&
			    sscanf(patterns, "%255[^/]/%5[0-9]", host, sport) != 2) {
				debug("%.100s, line %lu: Bad permitopen specification "
				    "<%.100s>", file, linenum, patterns);
				snprintf(tmp, sizeof(tmp), "%.100s, line %lu: "
				    "Bad permitopen specification", file, linenum);
				buffer_put_cstring(&auth_debug, tmp);
				xfree(patterns);
				goto bad_option;
			}
			if ((port = a2port(sport)) == 0) {
				debug("%.100s, line %lu: Bad permitopen port <%.100s>",
				    file, linenum, sport);
				snprintf(tmp, sizeof(tmp), "%.100s, line %lu: "
				    "Bad permitopen port", file, linenum);
				buffer_put_cstring(&auth_debug, tmp);
				xfree(patterns);
				goto bad_option;
			}
			if (options.allow_tcp_forwarding)
				channel_add_permitted_opens(host, port);
			xfree(patterns);
			goto next_option;
		}
next_option:
		/*
		 * Skip the comma, and move to the next option
		 * (or break out if there are no more).
		 */
		if (!*opts)
			fatal("Bugs in auth-options.c option processing.");
		if (*opts == ' ' || *opts == '\t')
			break;		/* End of options. */
		if (*opts != ',')
			goto bad_option;
		opts++;
		/* Process the next option. */
	}

	if (!use_privsep)
		auth_send_debug(&auth_debug);

	/* grant access */
	return 1;

bad_option:
	log("Bad options in %.100s file, line %lu: %.50s",
	    file, linenum, opts);
	snprintf(tmp, sizeof(tmp),
	    "Bad options in %.100s file, line %lu: %.50s",
	    file, linenum, opts);
	buffer_put_cstring(&auth_debug, tmp);

	if (!use_privsep)
		auth_send_debug(&auth_debug);

	/* deny access */
	return 0;
}
