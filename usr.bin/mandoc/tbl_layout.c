/*	$OpenBSD: tbl_layout.c,v 1.19 2015/01/26 18:41:45 schwarze Exp $ */
/*
 * Copyright (c) 2009, 2010, 2011 Kristaps Dzonsons <kristaps@bsd.lv>
 * Copyright (c) 2012, 2014, 2015 Ingo Schwarze <schwarze@openbsd.org>
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

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "mandoc.h"
#include "mandoc_aux.h"
#include "libmandoc.h"
#include "libroff.h"

struct	tbl_phrase {
	char		 name;
	enum tbl_cellt	 key;
};

static	const struct tbl_phrase keys[] = {
	{ 'c',		 TBL_CELL_CENTRE },
	{ 'r',		 TBL_CELL_RIGHT },
	{ 'l',		 TBL_CELL_LEFT },
	{ 'n',		 TBL_CELL_NUMBER },
	{ 's',		 TBL_CELL_SPAN },
	{ 'a',		 TBL_CELL_LONG },
	{ '^',		 TBL_CELL_DOWN },
	{ '-',		 TBL_CELL_HORIZ },
	{ '_',		 TBL_CELL_HORIZ },
	{ '=',		 TBL_CELL_DHORIZ }
};

#define KEYS_MAX ((int)(sizeof(keys)/sizeof(keys[0])))

static	void		 mods(struct tbl_node *, struct tbl_cell *,
				int, const char *, int *);
static	void		 cell(struct tbl_node *, struct tbl_row *,
				int, const char *, int *);
static	struct tbl_cell *cell_alloc(struct tbl_node *, struct tbl_row *,
				enum tbl_cellt, int vert);


static void
mods(struct tbl_node *tbl, struct tbl_cell *cp,
		int ln, const char *p, int *pos)
{
	char		*endptr;

mod:
	while (p[*pos] == ' ' || p[*pos] == '\t')
		(*pos)++;

	/* Row delimiters and cell specifiers end modifier lists. */

	if (strchr(".,-=^_ACLNRSaclnrs|", p[*pos]) != NULL)
		return;

	/* Throw away parenthesised expression. */

	if ('(' == p[*pos]) {
		(*pos)++;
		while (p[*pos] && ')' != p[*pos])
			(*pos)++;
		if (')' == p[*pos]) {
			(*pos)++;
			goto mod;
		}
		mandoc_msg(MANDOCERR_TBLLAYOUT_PAR, tbl->parse,
		    ln, *pos, NULL);
		return;
	}

	/* Parse numerical spacing from modifier string. */

	if (isdigit((unsigned char)p[*pos])) {
		cp->spacing = strtoull(p + *pos, &endptr, 10);
		*pos = endptr - p;
		goto mod;
	}

	switch (tolower((unsigned char)p[(*pos)++])) {
	case 'b':
		/* FALLTHROUGH */
	case 'i':
		/* FALLTHROUGH */
	case 'r':
		(*pos)--;
		break;
	case 'd':
		cp->flags |= TBL_CELL_BALIGN;
		goto mod;
	case 'e':
		cp->flags |= TBL_CELL_EQUAL;
		goto mod;
	case 'f':
		break;
	case 'm':
		mandoc_msg(MANDOCERR_TBLLAYOUT_MOD, tbl->parse,
		    ln, *pos, "m");
		goto mod;
	case 'p':
		/* FALLTHROUGH */
	case 'v':
		if (p[*pos] == '-' || p[*pos] == '+')
			(*pos)++;
		while (isdigit((unsigned char)p[*pos]))
			(*pos)++;
		goto mod;
	case 't':
		cp->flags |= TBL_CELL_TALIGN;
		goto mod;
	case 'u':
		cp->flags |= TBL_CELL_UP;
		goto mod;
	case 'w':  /* XXX for now, ignore minimal column width */
		goto mod;
	case 'x':
		cp->flags |= TBL_CELL_WMAX;
		goto mod;
	case 'z':
		cp->flags |= TBL_CELL_WIGN;
		goto mod;
	default:
		mandoc_vmsg(MANDOCERR_TBLLAYOUT_CHAR, tbl->parse,
		    ln, *pos - 1, "%c", p[*pos - 1]);
		goto mod;
	}

	switch (tolower((unsigned char)p[(*pos)++])) {
	case '3':
		/* FALLTHROUGH */
	case 'b':
		cp->flags |= TBL_CELL_BOLD;
		goto mod;
	case '2':
		/* FALLTHROUGH */
	case 'i':
		cp->flags |= TBL_CELL_ITALIC;
		goto mod;
	case '1':
		/* FALLTHROUGH */
	case 'r':
		goto mod;
	default:
		mandoc_vmsg(MANDOCERR_FT_BAD, tbl->parse,
		    ln, *pos - 1, "TS f%c", p[*pos - 1]);
		goto mod;
	}
}

static void
cell(struct tbl_node *tbl, struct tbl_row *rp,
		int ln, const char *p, int *pos)
{
	int		 vert, i;
	enum tbl_cellt	 c;

	/* Handle vertical lines. */

	vert = 0;
again:
	while (p[*pos] == ' ' || p[*pos] == '\t' || p[*pos] == '|') {
		if (p[*pos] == '|') {
			if (vert < 2)
				vert++;
			else
				mandoc_msg(MANDOCERR_TBLLAYOUT_VERT,
				    tbl->parse, ln, *pos, NULL);
		}
		(*pos)++;
	}

	/* Handle trailing vertical lines */

	if ('.' == p[*pos] || '\0' == p[*pos]) {
		rp->vert = vert;
		return;
	}

	/* Parse the column position (`c', `l', `r', ...). */

	for (i = 0; i < KEYS_MAX; i++)
		if (tolower((unsigned char)p[*pos]) == keys[i].name)
			break;

	if (i == KEYS_MAX) {
		mandoc_vmsg(MANDOCERR_TBLLAYOUT_CHAR, tbl->parse,
		    ln, *pos, "%c", p[*pos]);
		(*pos)++;
		goto again;
	}
	c = keys[i].key;

	/* Special cases of spanners. */

	if (c == TBL_CELL_SPAN) {
		if (rp->last == NULL)
			mandoc_msg(MANDOCERR_TBLLAYOUT_SPAN,
			    tbl->parse, ln, *pos, NULL);
		else if (rp->last->pos == TBL_CELL_HORIZ ||
		    rp->last->pos == TBL_CELL_DHORIZ)
			c = rp->last->pos;
	} else if (c == TBL_CELL_DOWN && rp == tbl->first_row)
		mandoc_msg(MANDOCERR_TBLLAYOUT_DOWN,
		    tbl->parse, ln, *pos, NULL);

	(*pos)++;

	/* Allocate cell then parse its modifiers. */

	mods(tbl, cell_alloc(tbl, rp, c, vert), ln, p, pos);
}

void
tbl_layout(struct tbl_node *tbl, int ln, const char *p)
{
	struct tbl_row	*rp;
	int		 pos;

	pos = 0;
	rp = NULL;

	for (;;) {
		/* Skip whitespace before and after each cell. */

		while (p[pos] == ' ' || p[pos] == '\t')
			pos++;

		switch (p[pos]) {
		case ',':  /* Next row on this input line. */
			pos++;
			rp = NULL;
			continue;
		case '\0':  /* Next row on next input line. */
			return;
		case '.':  /* End of layout. */
			pos++;
			tbl->part = TBL_PART_DATA;
			if (tbl->first_row != NULL)
				return;
			mandoc_msg(MANDOCERR_TBLLAYOUT_NONE,
			    tbl->parse, ln, pos, NULL);
			rp = mandoc_calloc(1, sizeof(*rp));
			cell_alloc(tbl, rp, TBL_CELL_LEFT, 0);
			tbl->first_row = tbl->last_row = rp;
			return;
		default:  /* Cell. */
			break;
		}

		if (rp == NULL) {  /* First cell on this line. */
			rp = mandoc_calloc(1, sizeof(*rp));
			if (tbl->last_row)
				tbl->last_row->next = rp;
			else
				tbl->first_row = rp;
			tbl->last_row = rp;
		}
		cell(tbl, rp, ln, p, &pos);
	}
}

static struct tbl_cell *
cell_alloc(struct tbl_node *tbl, struct tbl_row *rp, enum tbl_cellt pos,
		int vert)
{
	struct tbl_cell	*p, *pp;
	struct tbl_head	*h, *hp;

	p = mandoc_calloc(1, sizeof(struct tbl_cell));

	if (NULL != (pp = rp->last)) {
		pp->next = p;
		h = pp->head->next;
	} else {
		rp->first = p;
		h = tbl->first_head;
	}
	rp->last = p;

	p->pos = pos;
	p->vert = vert;

	/* Re-use header. */

	if (h) {
		p->head = h;
		return(p);
	}

	hp = mandoc_calloc(1, sizeof(struct tbl_head));
	hp->ident = tbl->opts.cols++;
	hp->vert = vert;

	if (tbl->last_head) {
		hp->prev = tbl->last_head;
		tbl->last_head->next = hp;
	} else
		tbl->first_head = hp;
	tbl->last_head = hp;

	p->head = hp;
	return(p);
}
