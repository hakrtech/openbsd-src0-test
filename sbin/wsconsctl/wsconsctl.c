/*	$OpenBSD: wsconsctl.c,v 1.5 2001/06/30 02:12:57 mickey Exp $	*/
/*	$NetBSD: wsconsctl.c,v 1.2 1998/12/29 22:40:20 hannken Exp $ */

/*-
 * Copyright (c) 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Juergen Hannken-Illjes.
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
 *	This product includes software developed by the NetBSD
 *	Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <fcntl.h>
#include <err.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "wsconsctl.h"

#define PATH_KEYBOARD		"/dev/wskbd0"
#define PATH_MOUSE		"/dev/wsmouse0"
#define PATH_DISPLAY		"/dev/ttyC0"

extern const char *__progname;		/* from crt0.o */

extern struct field keyboard_field_tab[];
extern struct field mouse_field_tab[];
extern struct field display_field_tab[];

void usage __P((char *));

const struct vartypesw {
	const char *name, *file;
	struct field *field_tab;
	void (*getval) __P((const char *pre, int));
	void (*putval) __P((const char *pre, int));
} typesw[] = {
	{ "keyboard", PATH_KEYBOARD, keyboard_field_tab,
	  keyboard_get_values, keyboard_put_values },
	{ "mouse", PATH_MOUSE, mouse_field_tab,
	  mouse_get_values, mouse_put_values },
	{ "display", PATH_DISPLAY, display_field_tab,
	  display_get_values, display_put_values },
	{ NULL }
};

const struct vartypesw *tab_by_name __P((const char *));

void
usage(msg)
	char *msg;
{
	if (msg != NULL)
		fprintf(stderr, "%s: %s\n\n", __progname, msg);

	fprintf(stderr,
	    "usage: %s [-f file] [-n] name ...\n"
	    "       %s [-f file] [-n] -w name=value ...\n"
	    "       %s [-f file] [-n] -a\n", __progname,
		__progname, __progname);

	exit(1);
}

int
main(argc, argv)
	int argc;
	char **argv;
{
	int i, ch, fd, error;
	int aflag, wflag;
	char *sep, *p;
	const struct vartypesw *sw;
	struct field *f;
	int do_merge;

	error = aflag = wflag = 0;
	sw = NULL;
	sep = "=";

	while ((ch = getopt(argc, argv, "anw")) != -1) {
		switch(ch) {
		case 'a':
			aflag = 1;
			break;
		case 'n':
			sep = NULL;
			break;
		case 'w':
			wflag = 1;
			break;
		case '?':
		default:
			usage(NULL);
		}
	}

	argc -= optind;
	argv += optind;

	if (argc > 0 && aflag != 0)
		usage("excess arguments after -a");
	if (aflag != 0 && wflag != 0)
		usage("only one of -a or -w may be given");

	if (aflag != 0) {
		for (sw = typesw; sw->name; sw++) {
			if ((fd = open(sw->file, O_WRONLY)) < 0 &&
			    (fd = open(sw->file, O_RDONLY)) < 0) {
				warn("%s", sw->file);
				error = 1;
				continue;
			}
			for (i = 0; sw->field_tab[i].name; i++)
				if ((sw->field_tab[i].flags &
				    (FLG_NOAUTO|FLG_WRONLY)) == 0)
					sw->field_tab[i].flags |= FLG_GET;
			(*sw->getval)(sw->name, fd);
			for (i = 0; sw->field_tab[i].name; i++)
				if (sw->field_tab[i].flags & FLG_NOAUTO)
					warnx("Use explicit arg to view %s.%s.",
					      sw->name, sw->field_tab[i].name);
				else if (sw->field_tab[i].flags & FLG_GET)
					pr_field(sw->name, sw->field_tab + i, sep);
			close(fd);
		}
	} else if (argc > 0) {
		if (wflag != 0) {
			for (i = 0; i < argc; i++) {
				p = strchr(argv[i], '=');
				if (p == NULL) {
					warnx("'=' not found");
					continue;
				}
				if (p > argv[i] && *(p - 1) == '+') {
					*(p - 1) = '\0';
					do_merge = 1;
				} else
					do_merge = 0;
				*p++ = '\0';
				sw = tab_by_name(argv[i]);
				if (!sw)
					continue;
				f = field_by_name(sw->field_tab, argv[i]);
				if ((f->flags & FLG_RDONLY) != 0) {
					warnx("%s: read only", argv[i]);
					continue;
				}
				if (do_merge) {
					if ((f->flags & FLG_MODIFY) == 0)
						errx(1, "%s: can only be set",
						     argv[i]);
					f->flags |= FLG_GET;
					(*sw->getval)(sw->name, fd);
					f->flags &= ~FLG_GET;
				}
				rd_field(f, p, do_merge);
				f->flags |= FLG_SET;
				(*sw->putval)(sw->name, fd);
				f->flags &= ~FLG_SET;
			}
		} else {
			for (i = 0; i < argc; i++) {
				sw = tab_by_name(argv[i]);
				if (!sw)
					continue;
				f = field_by_name(sw->field_tab, argv[i]);
				if ((f->flags & FLG_WRONLY) != 0) {
					warnx("%s: write only", argv[i]);
					continue;
				}
				f->flags |= FLG_GET;
				(*sw->getval)(sw->name, fd);
				pr_field(sw->name, f, sep);
			}
		}
	} else
		usage(NULL);

	return (error);
}

const struct vartypesw *
tab_by_name(var)
	const char *var;
{
	const struct vartypesw *sw;
	const char *p = strchr(var, '.');

	if (!p) {
		warnx("%s: illegal variable name", var);
		return (NULL);
	}

	for (sw = typesw; sw->name; sw++)
		if (!strncmp(sw->name, var, p - var))
			break;

	if (!sw->name) {
		warnx("%s: no such variable", var);
		return (NULL);
	}

	return (sw);
}
