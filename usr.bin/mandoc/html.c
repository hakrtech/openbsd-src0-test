/*	$OpenBSD: html.c,v 1.66 2017/01/18 19:22:18 schwarze Exp $ */
/*
 * Copyright (c) 2008-2011, 2014 Kristaps Dzonsons <kristaps@bsd.lv>
 * Copyright (c) 2011-2015, 2017 Ingo Schwarze <schwarze@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHORS DISCLAIM ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include <sys/types.h>

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mandoc.h"
#include "mandoc_aux.h"
#include "out.h"
#include "html.h"
#include "manconf.h"
#include "main.h"

struct	htmldata {
	const char	 *name;
	int		  flags;
#define	HTML_NOSTACK	 (1 << 0)
#define	HTML_AUTOCLOSE	 (1 << 1)
#define	HTML_NLBEFORE	 (1 << 2)
#define	HTML_NLBEGIN	 (1 << 3)
#define	HTML_NLEND	 (1 << 4)
#define	HTML_NLAFTER	 (1 << 5)
#define	HTML_NLAROUND	 (HTML_NLBEFORE | HTML_NLAFTER)
#define	HTML_NLINSIDE	 (HTML_NLBEGIN | HTML_NLEND)
#define	HTML_NLALL	 (HTML_NLAROUND | HTML_NLINSIDE)
#define	HTML_INDENT	 (1 << 6)
#define	HTML_NOINDENT	 (1 << 7)
};

static	const struct htmldata htmltags[TAG_MAX] = {
	{"html",	HTML_NLALL},
	{"head",	HTML_NLALL | HTML_INDENT},
	{"body",	HTML_NLALL},
	{"meta",	HTML_NOSTACK | HTML_AUTOCLOSE | HTML_NLALL},
	{"title",	HTML_NLAROUND},
	{"div",		HTML_NLAROUND},
	{"h1",		HTML_NLAROUND},
	{"h2",		HTML_NLAROUND},
	{"span",	0},
	{"link",	HTML_NOSTACK | HTML_AUTOCLOSE | HTML_NLALL},
	{"br",		HTML_NOSTACK | HTML_AUTOCLOSE | HTML_NLALL},
	{"a",		0},
	{"table",	HTML_NLALL | HTML_INDENT},
	{"tbody",	HTML_NLALL | HTML_INDENT},
	{"col",		HTML_NOSTACK | HTML_AUTOCLOSE | HTML_NLALL},
	{"tr",		HTML_NLALL | HTML_INDENT},
	{"td",		HTML_NLAROUND},
	{"li",		HTML_NLAROUND | HTML_INDENT},
	{"ul",		HTML_NLALL | HTML_INDENT},
	{"ol",		HTML_NLALL | HTML_INDENT},
	{"dl",		HTML_NLALL | HTML_INDENT},
	{"dt",		HTML_NLAROUND},
	{"dd",		HTML_NLAROUND | HTML_INDENT},
	{"blockquote",	HTML_NLALL | HTML_INDENT},
	{"pre",		HTML_NLALL | HTML_NOINDENT},
	{"b",		0},
	{"i",		0},
	{"code",	0},
	{"small",	0},
	{"style",	HTML_NLALL | HTML_INDENT},
	{"math",	HTML_NLALL | HTML_INDENT},
	{"mrow",	0},
	{"mi",		0},
	{"mo",		0},
	{"msup",	0},
	{"msub",	0},
	{"msubsup",	0},
	{"mfrac",	0},
	{"msqrt",	0},
	{"mfenced",	0},
	{"mtable",	0},
	{"mtr",		0},
	{"mtd",		0},
	{"munderover",	0},
	{"munder",	0},
	{"mover",	0},
};

static	const char	*const roffscales[SCALE_MAX] = {
	"cm", /* SCALE_CM */
	"in", /* SCALE_IN */
	"pc", /* SCALE_PC */
	"pt", /* SCALE_PT */
	"em", /* SCALE_EM */
	"em", /* SCALE_MM */
	"ex", /* SCALE_EN */
	"ex", /* SCALE_BU */
	"em", /* SCALE_VS */
	"ex", /* SCALE_FS */
};

static	void	 a2width(const char *, struct roffsu *);
static	void	 html_endline(struct html *);
static	void	 html_indent(struct html *);
static	void	 print_ctag(struct html *, struct tag *);
static	int	 print_escape(char);
static	int	 print_encode(struct html *, const char *, const char *, int);
static	void	 print_href(struct html *, const char *, const char *, int);
static	void	 print_metaf(struct html *, enum mandoc_esc);


void *
html_alloc(const struct manoutput *outopts)
{
	struct html	*h;

	h = mandoc_calloc(1, sizeof(struct html));

	h->tags.head = NULL;
	h->style = outopts->style;
	h->base_man = outopts->man;
	h->base_includes = outopts->includes;
	if (outopts->fragment)
		h->oflags |= HTML_FRAGMENT;

	return h;
}

void
html_free(void *p)
{
	struct tag	*tag;
	struct html	*h;

	h = (struct html *)p;

	while ((tag = h->tags.head) != NULL) {
		h->tags.head = tag->next;
		free(tag);
	}

	free(h);
}

void
print_gen_head(struct html *h)
{
	struct tag	*t;

	print_otag(h, TAG_META, "?", "charset", "utf-8");

	/*
	 * Print a default style-sheet.
	 */

	t = print_otag(h, TAG_STYLE, "");
	print_text(h, "table.head, table.foot { width: 100%; }");
	html_endline(h);
	print_text(h, "td.head-rtitle, td.foot-os { text-align: right; }");
	html_endline(h);
	print_text(h, "td.head-vol { text-align: center; }");
	html_endline(h);
	print_text(h, "table.foot td { width: 50%; }");
	html_endline(h);
	print_text(h, "table.head td { width: 33%; }");
	html_endline(h);
	print_text(h, "div.spacer { margin: 1em 0; }");
	print_tagq(h, t);

	if (h->style)
		print_otag(h, TAG_LINK, "?h??", "rel", "stylesheet",
		    h->style, "type", "text/css", "media", "all");
}

static void
print_metaf(struct html *h, enum mandoc_esc deco)
{
	enum htmlfont	 font;

	switch (deco) {
	case ESCAPE_FONTPREV:
		font = h->metal;
		break;
	case ESCAPE_FONTITALIC:
		font = HTMLFONT_ITALIC;
		break;
	case ESCAPE_FONTBOLD:
		font = HTMLFONT_BOLD;
		break;
	case ESCAPE_FONTBI:
		font = HTMLFONT_BI;
		break;
	case ESCAPE_FONT:
	case ESCAPE_FONTROMAN:
		font = HTMLFONT_NONE;
		break;
	default:
		abort();
	}

	if (h->metaf) {
		print_tagq(h, h->metaf);
		h->metaf = NULL;
	}

	h->metal = h->metac;
	h->metac = font;

	switch (font) {
	case HTMLFONT_ITALIC:
		h->metaf = print_otag(h, TAG_I, "");
		break;
	case HTMLFONT_BOLD:
		h->metaf = print_otag(h, TAG_B, "");
		break;
	case HTMLFONT_BI:
		h->metaf = print_otag(h, TAG_B, "");
		print_otag(h, TAG_I, "");
		break;
	default:
		break;
	}
}

int
html_strlen(const char *cp)
{
	size_t		 rsz;
	int		 skip, sz;

	/*
	 * Account for escaped sequences within string length
	 * calculations.  This follows the logic in term_strlen() as we
	 * must calculate the width of produced strings.
	 * Assume that characters are always width of "1".  This is
	 * hacky, but it gets the job done for approximation of widths.
	 */

	sz = 0;
	skip = 0;
	while (1) {
		rsz = strcspn(cp, "\\");
		if (rsz) {
			cp += rsz;
			if (skip) {
				skip = 0;
				rsz--;
			}
			sz += rsz;
		}
		if ('\0' == *cp)
			break;
		cp++;
		switch (mandoc_escape(&cp, NULL, NULL)) {
		case ESCAPE_ERROR:
			return sz;
		case ESCAPE_UNICODE:
		case ESCAPE_NUMBERED:
		case ESCAPE_SPECIAL:
		case ESCAPE_OVERSTRIKE:
			if (skip)
				skip = 0;
			else
				sz++;
			break;
		case ESCAPE_SKIPCHAR:
			skip = 1;
			break;
		default:
			break;
		}
	}
	return sz;
}

static int
print_escape(char c)
{

	switch (c) {
	case '<':
		printf("&lt;");
		break;
	case '>':
		printf("&gt;");
		break;
	case '&':
		printf("&amp;");
		break;
	case '"':
		printf("&quot;");
		break;
	case ASCII_NBRSP:
		printf("&nbsp;");
		break;
	case ASCII_HYPH:
		putchar('-');
		break;
	case ASCII_BREAK:
		break;
	default:
		return 0;
	}
	return 1;
}

static int
print_encode(struct html *h, const char *p, const char *pend, int norecurse)
{
	size_t		 sz;
	int		 c, len, nospace;
	const char	*seq;
	enum mandoc_esc	 esc;
	static const char rejs[9] = { '\\', '<', '>', '&', '"',
		ASCII_NBRSP, ASCII_HYPH, ASCII_BREAK, '\0' };

	if (pend == NULL)
		pend = strchr(p, '\0');

	nospace = 0;

	while (p < pend) {
		if (HTML_SKIPCHAR & h->flags && '\\' != *p) {
			h->flags &= ~HTML_SKIPCHAR;
			p++;
			continue;
		}

		sz = strcspn(p, rejs);
		if (p + sz > pend)
			sz = pend - p;

		fwrite(p, 1, sz, stdout);
		p += (int)sz;

		if (p >= pend)
			break;

		if (print_escape(*p++))
			continue;

		esc = mandoc_escape(&p, &seq, &len);
		if (ESCAPE_ERROR == esc)
			break;

		switch (esc) {
		case ESCAPE_FONT:
		case ESCAPE_FONTPREV:
		case ESCAPE_FONTBOLD:
		case ESCAPE_FONTITALIC:
		case ESCAPE_FONTBI:
		case ESCAPE_FONTROMAN:
			if (0 == norecurse)
				print_metaf(h, esc);
			continue;
		case ESCAPE_SKIPCHAR:
			h->flags |= HTML_SKIPCHAR;
			continue;
		default:
			break;
		}

		if (h->flags & HTML_SKIPCHAR) {
			h->flags &= ~HTML_SKIPCHAR;
			continue;
		}

		switch (esc) {
		case ESCAPE_UNICODE:
			/* Skip past "u" header. */
			c = mchars_num2uc(seq + 1, len - 1);
			break;
		case ESCAPE_NUMBERED:
			c = mchars_num2char(seq, len);
			if (c < 0)
				continue;
			break;
		case ESCAPE_SPECIAL:
			c = mchars_spec2cp(seq, len);
			if (c <= 0)
				continue;
			break;
		case ESCAPE_NOSPACE:
			if ('\0' == *p)
				nospace = 1;
			continue;
		case ESCAPE_OVERSTRIKE:
			if (len == 0)
				continue;
			c = seq[len - 1];
			break;
		default:
			continue;
		}
		if ((c < 0x20 && c != 0x09) ||
		    (c > 0x7E && c < 0xA0))
			c = 0xFFFD;
		if (c > 0x7E)
			printf("&#%d;", c);
		else if ( ! print_escape(c))
			putchar(c);
	}

	return nospace;
}

static void
print_href(struct html *h, const char *name, const char *sec, int man)
{
	const char	*p, *pp;

	pp = man ? h->base_man : h->base_includes;
	while ((p = strchr(pp, '%')) != NULL) {
		print_encode(h, pp, p, 1);
		if (man && p[1] == 'S') {
			if (sec == NULL)
				putchar('1');
			else
				print_encode(h, sec, NULL, 1);
		} else if ((man && p[1] == 'N') ||
		    (man == 0 && p[1] == 'I'))
			print_encode(h, name, NULL, 1);
		else
			print_encode(h, p, p + 2, 1);
		pp = p + 2;
	}
	if (*pp != '\0')
		print_encode(h, pp, NULL, 1);
}

struct tag *
print_otag(struct html *h, enum htmltag tag, const char *fmt, ...)
{
	va_list		 ap;
	struct roffsu	 mysu, *su;
	struct tag	*t;
	const char	*attr;
	char		*s;
	double		 v;
	int		 i, have_style, tflags;

	tflags = htmltags[tag].flags;

	/* Push this tags onto the stack of open scopes. */

	if ((tflags & HTML_NOSTACK) == 0) {
		t = mandoc_malloc(sizeof(struct tag));
		t->tag = tag;
		t->next = h->tags.head;
		h->tags.head = t;
	} else
		t = NULL;

	if (tflags & HTML_NLBEFORE)
		html_endline(h);
	if (h->flags & HTML_NLDONE)
		html_indent(h);
	else if ((h->flags & HTML_NOSPACE) == 0) {
		if (h->flags & HTML_KEEP)
			printf("&#160;");
		else {
			if (h->flags & HTML_PREKEEP)
				h->flags |= HTML_KEEP;
			putchar(' ');
		}
	}

	if ( ! (h->flags & HTML_NONOSPACE))
		h->flags &= ~HTML_NOSPACE;
	else
		h->flags |= HTML_NOSPACE;

	/* Print out the tag name and attributes. */

	printf("<%s", htmltags[tag].name);

	va_start(ap, fmt);

	have_style = 0;
	while (*fmt != '\0') {
		if (*fmt == 's') {
			printf(" style=\"");
			have_style = 1;
			fmt++;
			break;
		}
		s = va_arg(ap, char *);
		switch (*fmt++) {
		case 'c':
			attr = "class";
			break;
		case 'h':
			attr = "href";
			break;
		case 'i':
			attr = "id";
			break;
		case '?':
			attr = s;
			s = va_arg(ap, char *);
			break;
		default:
			abort();
		}
		printf(" %s=\"", attr);
		switch (*fmt) {
		case 'M':
			print_href(h, s, va_arg(ap, char *), 1);
			fmt++;
			break;
		case 'I':
			print_href(h, s, NULL, 0);
			fmt++;
			break;
		case 'R':
			putchar('#');
			fmt++;
			/* FALLTHROUGH */
		default:
			print_encode(h, s, NULL, 1);
			break;
		}
		putchar('"');
	}

	/* Print out styles. */

	s = NULL;
	su = &mysu;
	while (*fmt != '\0') {

		/* First letter: input argument type. */

		switch (*fmt++) {
		case 'h':
			i = va_arg(ap, int);
			SCALE_HS_INIT(su, i);
			break;
		case 's':
			s = va_arg(ap, char *);
			break;
		case 'u':
			su = va_arg(ap, struct roffsu *);
			break;
		case 'v':
			i = va_arg(ap, int);
			SCALE_VS_INIT(su, i);
			break;
		case 'w':
			s = va_arg(ap, char *);
			a2width(s, su);
			break;
		default:
			abort();
		}

		/* Second letter: style name. */

		switch (*fmt++) {
		case 'b':
			attr = "margin-bottom";
			break;
		case 'h':
			attr = "height";
			break;
		case 'i':
			attr = "text-indent";
			break;
		case 'l':
			attr = "margin-left";
			break;
		case 't':
			attr = "margin-top";
			break;
		case 'w':
			attr = "width";
			break;
		case 'W':
			attr = "min-width";
			break;
		case '?':
			printf("%s: %s;", s, va_arg(ap, char *));
			continue;
		default:
			abort();
		}
		v = su->scale;
		if (su->unit == SCALE_MM && (v /= 100.0) == 0.0)
			v = 1.0;
		else if (su->unit == SCALE_BU)
			v /= 24.0;
		printf("%s: %.2f%s;", attr, v, roffscales[su->unit]);
	}
	if (have_style)
		putchar('"');

	va_end(ap);

	/* Accommodate for "well-formed" singleton escaping. */

	if (HTML_AUTOCLOSE & htmltags[tag].flags)
		putchar('/');

	putchar('>');

	if (tflags & HTML_NLBEGIN)
		html_endline(h);
	else
		h->flags |= HTML_NOSPACE;

	if (tflags & HTML_INDENT)
		h->indent++;
	if (tflags & HTML_NOINDENT)
		h->noindent++;

	return t;
}

static void
print_ctag(struct html *h, struct tag *tag)
{
	int	 tflags;

	/*
	 * Remember to close out and nullify the current
	 * meta-font and table, if applicable.
	 */
	if (tag == h->metaf)
		h->metaf = NULL;
	if (tag == h->tblt)
		h->tblt = NULL;

	tflags = htmltags[tag->tag].flags;

	if (tflags & HTML_INDENT)
		h->indent--;
	if (tflags & HTML_NOINDENT)
		h->noindent--;
	if (tflags & HTML_NLEND)
		html_endline(h);
	html_indent(h);
	printf("</%s>", htmltags[tag->tag].name);
	if (tflags & HTML_NLAFTER)
		html_endline(h);

	h->tags.head = tag->next;
	free(tag);
}

void
print_gen_decls(struct html *h)
{

	puts("<!DOCTYPE html>");
	h->flags |= HTML_NLDONE;
}

void
print_text(struct html *h, const char *word)
{
	if ((h->flags & (HTML_NLDONE | HTML_NOSPACE)) == 0) {
		if ( ! (HTML_KEEP & h->flags)) {
			if (HTML_PREKEEP & h->flags)
				h->flags |= HTML_KEEP;
			putchar(' ');
		} else
			printf("&#160;");
	}

	assert(NULL == h->metaf);
	switch (h->metac) {
	case HTMLFONT_ITALIC:
		h->metaf = print_otag(h, TAG_I, "");
		break;
	case HTMLFONT_BOLD:
		h->metaf = print_otag(h, TAG_B, "");
		break;
	case HTMLFONT_BI:
		h->metaf = print_otag(h, TAG_B, "");
		print_otag(h, TAG_I, "");
		break;
	default:
		html_indent(h);
		break;
	}

	assert(word);
	if ( ! print_encode(h, word, NULL, 0)) {
		if ( ! (h->flags & HTML_NONOSPACE))
			h->flags &= ~HTML_NOSPACE;
		h->flags &= ~HTML_NONEWLINE;
	} else
		h->flags |= HTML_NOSPACE | HTML_NONEWLINE;

	if (h->metaf) {
		print_tagq(h, h->metaf);
		h->metaf = NULL;
	}

	h->flags &= ~HTML_IGNDELIM;
}

void
print_tagq(struct html *h, const struct tag *until)
{
	struct tag	*tag;

	while ((tag = h->tags.head) != NULL) {
		print_ctag(h, tag);
		if (until && tag == until)
			return;
	}
}

void
print_stagq(struct html *h, const struct tag *suntil)
{
	struct tag	*tag;

	while ((tag = h->tags.head) != NULL) {
		if (suntil && tag == suntil)
			return;
		print_ctag(h, tag);
	}
}

void
print_paragraph(struct html *h)
{
	struct tag	*t;

	t = print_otag(h, TAG_DIV, "c", "spacer");
	print_tagq(h, t);
}

/*
 * If something was printed on the current output line, end it.
 * Not to be called right after html_indent().
 */
static void
html_endline(struct html *h)
{
	if (h->flags & HTML_NLDONE)
		return;

	putchar('\n');
	h->flags |= HTML_NLDONE | HTML_NOSPACE;
}

/*
 * If at the beginning of a new output line,
 * perform indentation and mark the line as containing output.
 * Make sure to really produce some output right afterwards,
 * but do not use print_otag() for producing it.
 */
static void
html_indent(struct html *h)
{
	int	 i;

	if ((h->flags & HTML_NLDONE) == 0)
		return;

	if (h->noindent == 0)
		for (i = 0; i < h->indent * 2; i++)
			putchar(' ');
	h->flags &= ~(HTML_NLDONE | HTML_NOSPACE);
}

/*
 * Calculate the scaling unit passed in a `-width' argument.  This uses
 * either a native scaling unit (e.g., 1i, 2m) or the string length of
 * the value.
 */
static void
a2width(const char *p, struct roffsu *su)
{
	if (a2roffsu(p, su, SCALE_MAX) < 2) {
		su->unit = SCALE_EN;
		su->scale = html_strlen(p);
	} else if (su->scale < 0.0)
		su->scale = 0.0;
}
