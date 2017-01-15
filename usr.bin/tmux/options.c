/* $OpenBSD: options.c,v 1.26 2017/01/15 20:48:41 nicm Exp $ */

/*
 * Copyright (c) 2008 Nicholas Marriott <nicholas.marriott@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF MIND, USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/types.h>

#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "tmux.h"

/*
 * Option handling; each option has a name, type and value and is stored in
 * a red-black tree.
 */

struct option {
	struct options				 *owner;

	const char				 *name;
	const struct options_table_entry	 *tableentry;

	union {
		char				 *string;
		long long			  number;
		struct grid_cell		  style;
		struct {
			const char		**array;
			u_int			  arraysize;
		};
	};

	RB_ENTRY(option)			  entry;
};

struct options {
	RB_HEAD(options_tree, option)		 tree;
	struct options				*parent;
};

static struct option	*options_add(struct options *, const char *);

#define OPTIONS_ARRAY_LIMIT 1000

#define OPTIONS_IS_STRING(o)						\
	((o)->tableentry == NULL ||					\
	    (o)->tableentry->type == OPTIONS_TABLE_STRING)
#define OPTIONS_IS_NUMBER(o) \
	((o)->tableentry != NULL &&					\
	    ((o)->tableentry->type == OPTIONS_TABLE_NUMBER ||		\
	    (o)->tableentry->type == OPTIONS_TABLE_KEY ||		\
	    (o)->tableentry->type == OPTIONS_TABLE_COLOUR ||		\
	    (o)->tableentry->type == OPTIONS_TABLE_ATTRIBUTES ||	\
	    (o)->tableentry->type == OPTIONS_TABLE_FLAG ||		\
	    (o)->tableentry->type == OPTIONS_TABLE_CHOICE))
#define OPTIONS_IS_STYLE(o) \
	((o)->tableentry != NULL &&					\
	    (o)->tableentry->type == OPTIONS_TABLE_STYLE)
#define OPTIONS_IS_ARRAY(o) \
	((o)->tableentry != NULL &&					\
	    (o)->tableentry->type == OPTIONS_TABLE_ARRAY)

static int	options_cmp(struct option *, struct option *);
RB_GENERATE_STATIC(options_tree, option, entry, options_cmp);

static int
options_cmp(struct option *lhs, struct option *rhs)
{
	return (strcmp(lhs->name, rhs->name));
}

static const struct options_table_entry *
options_parent_table_entry(struct options *oo, const char *s)
{
	struct option	*o;

	if (oo->parent == NULL)
		fatalx("no parent options for %s", s);
	o = options_get_only(oo->parent, s);
	if (o == NULL)
		fatalx("%s not in parent options", s);
	return (o->tableentry);
}

struct options *
options_create(struct options *parent)
{
	struct options	*oo;

	oo = xcalloc(1, sizeof *oo);
	RB_INIT(&oo->tree);
	oo->parent = parent;
	return (oo);
}

void
options_free(struct options *oo)
{
	struct option	*o, *tmp;

	RB_FOREACH_SAFE (o, options_tree, &oo->tree, tmp)
		options_remove(o);
	free(oo);
}

struct option *
options_first(struct options *oo)
{
	return (RB_MIN(options_tree, &oo->tree));
}

struct option *
options_next(struct option *o)
{
	return (RB_NEXT(options_tree, &oo->tree, o));
}

struct option *
options_get_only(struct options *oo, const char *name)
{
	struct option	o;

	o.name = name;
	return (RB_FIND(options_tree, &oo->tree, &o));
}

struct option *
options_get(struct options *oo, const char *name)
{
	struct option	*o;

	o = options_get_only(oo, name);
	while (o == NULL) {
		oo = oo->parent;
		if (oo == NULL)
			break;
		o = options_get_only(oo, name);
	}
	return (o);
}

struct option *
options_empty(struct options *oo, const struct options_table_entry *oe)
{
	struct option	*o;

	o = options_add(oo, oe->name);
	o->tableentry = oe;

	return (o);
}

struct option *
options_default(struct options *oo, const struct options_table_entry *oe)
{
	struct option	*o;
	char		*cp, *copy, *next;
	u_int		 idx = 0;

	o = options_empty(oo, oe);

	if (oe->type == OPTIONS_TABLE_ARRAY) {
		copy = cp = xstrdup(oe->default_str);
		while ((next = strsep(&cp, ",")) != NULL) {
			options_array_set(o, idx, next);
			idx++;
		}
		free(copy);
		return (o);
	}

	if (oe->type == OPTIONS_TABLE_STRING)
		o->string = xstrdup(oe->default_str);
	else if (oe->type == OPTIONS_TABLE_STYLE) {
		memcpy(&o->style, &grid_default_cell, sizeof o->style);
		style_parse(&grid_default_cell, &o->style, oe->default_str);
	} else
		o->number = oe->default_num;
	return (o);
}

static struct option *
options_add(struct options *oo, const char *name)
{
	struct option	*o;

	o = options_get_only(oo, name);
	if (o != NULL)
		options_remove(o);

	o = xcalloc(1, sizeof *o);
	o->owner = oo;
	o->name = xstrdup(name);

	RB_INSERT(options_tree, &oo->tree, o);
	return (o);
}

void
options_remove(struct option *o)
{
	struct options	*oo = o->owner;
	u_int		 i;

	if (OPTIONS_IS_STRING(o))
		free((void *)o->string);
	else if (OPTIONS_IS_ARRAY(o)) {
		for (i = 0; i < o->arraysize; i++)
			free((void *)o->array[i]);
		free(o->array);
	}

	RB_REMOVE(options_tree, &oo->tree, o);
	free(o);
}

const char *
options_name(struct option *o)
{
	return (o->name);
}

const struct options_table_entry *
options_table_entry(struct option *o)
{
	return (o->tableentry);
}

const char *
options_array_get(struct option *o, u_int idx)
{
	if (!OPTIONS_IS_ARRAY(o))
		return (NULL);
	if (idx >= o->arraysize)
		return (NULL);
	return (o->array[idx]);
}

int
options_array_set(struct option *o, u_int idx, const char *value)
{
	u_int	i;

	if (!OPTIONS_IS_ARRAY(o))
		return (-1);

	if (idx >= OPTIONS_ARRAY_LIMIT)
		return (-1);
	if (idx >= o->arraysize) {
		o->array = xreallocarray(o->array, idx + 1, sizeof *o->array);
		for (i = o->arraysize; i < idx + 1; i++)
			o->array[i] = NULL;
		o->arraysize = idx + 1;
	}
	if (o->array[idx] != NULL)
		free((void *)o->array[idx]);
	if (value != NULL)
		o->array[idx] = xstrdup(value);
	else
		o->array[idx] = NULL;
	return (0);
}

int
options_array_size(struct option *o, u_int *size)
{
	if (!OPTIONS_IS_ARRAY(o))
		return (-1);
	if (size != NULL)
		*size = o->arraysize;
	return (0);
}

int
options_isstring(struct option *o)
{
	if (o->tableentry == NULL)
		return (1);
	return (OPTIONS_IS_STRING(o) || OPTIONS_IS_ARRAY(o));
}

const char *
options_tostring(struct option *o, int idx)
{
	static char	 s[1024];
	const char	*tmp;

	if (OPTIONS_IS_ARRAY(o)) {
		if (idx == -1)
			return (NULL);
		if ((u_int)idx >= o->arraysize || o->array[idx] == NULL)
			return ("");
		return (o->array[idx]);
	}
	if (OPTIONS_IS_STYLE(o))
		return (style_tostring(&o->style));
	if (OPTIONS_IS_NUMBER(o)) {
		tmp = NULL;
		switch (o->tableentry->type) {
		case OPTIONS_TABLE_NUMBER:
			xsnprintf(s, sizeof s, "%lld", o->number);
			break;
		case OPTIONS_TABLE_KEY:
			tmp = key_string_lookup_key(o->number);
			break;
		case OPTIONS_TABLE_COLOUR:
			tmp = colour_tostring(o->number);
			break;
		case OPTIONS_TABLE_ATTRIBUTES:
			tmp = attributes_tostring(o->number);
			break;
		case OPTIONS_TABLE_FLAG:
			tmp = (o->number ? "on" : "off");
			break;
		case OPTIONS_TABLE_CHOICE:
			tmp = o->tableentry->choices[o->number];
			break;
		case OPTIONS_TABLE_STRING:
		case OPTIONS_TABLE_STYLE:
		case OPTIONS_TABLE_ARRAY:
			break;
		}
		if (tmp != NULL)
			xsnprintf(s, sizeof s, "%s", tmp);
		return (s);
	}
	if (OPTIONS_IS_STRING(o))
		return (o->string);
	return (NULL);
}

char *
options_parse(const char *name, int *idx)
{
	char	*copy, *cp, *end;

	if (*name == '\0')
		return (NULL);
	copy = xstrdup(name);
	if ((cp = strchr(copy, '[')) == NULL) {
		*idx = -1;
		return (copy);
	}
	end = strchr(cp + 1, ']');
	if (end == NULL || end[1] != '\0' || !isdigit((u_char)end[-1])) {
		free(copy);
		return (NULL);
	}
	if (sscanf(cp, "[%d]", idx) != 1 || *idx < 0) {
		free(copy);
		return (NULL);
	}
	*cp = '\0';
	return (copy);
}

struct option *
options_parse_get(struct options *oo, const char *s, int *idx, int only)
{
	struct option	*o;
	char		*name;

	name = options_parse(s, idx);
	if (name == NULL)
		return (NULL);
	if (only)
		o = options_get_only(oo, name);
	else
		o = options_get(oo, name);
	free(name);
	if (o != NULL) {
		if (OPTIONS_IS_ARRAY(o) && *idx == -1)
			return (NULL);
		if (!OPTIONS_IS_ARRAY(o) && *idx != -1)
			return (NULL);
	}
	return (o);
}

char *
options_match(const char *s, int *idx, int* ambiguous)
{
	const struct options_table_entry	*oe, *found;
	char					*name;
	size_t					 namelen;

	name = options_parse(s, idx);
	namelen = strlen(name);

	found = NULL;
	for (oe = options_table; oe->name != NULL; oe++) {
		if (strcmp(oe->name, name) == 0) {
			found = oe;
			break;
		}
		if (strncmp(oe->name, name, namelen) == 0) {
			if (found != NULL) {
				*ambiguous = 1;
				free(name);
				return (NULL);
			}
			found = oe;
		}
	}
	free(name);
	if (found == NULL) {
		*ambiguous = 0;
		return (NULL);
	}
	return (xstrdup(found->name));
}

struct option *
options_match_get(struct options *oo, const char *s, int *idx, int only,
    int* ambiguous)
{
	char		*name;
	struct option	*o;

	name = options_match(s, idx, ambiguous);
	if (name == NULL)
		return (NULL);
	*ambiguous = 0;
	if (only)
		o = options_get_only(oo, name);
	else
		o = options_get(oo, name);
	free(name);
	if (o != NULL) {
		if (OPTIONS_IS_ARRAY(o) && *idx == -1)
			return (NULL);
		if (!OPTIONS_IS_ARRAY(o) && *idx != -1)
			return (NULL);
	}
	return (o);
}


const char *
options_get_string(struct options *oo, const char *name)
{
	struct option	*o;

	o = options_get(oo, name);
	if (o == NULL)
		fatalx("missing option %s", name);
	if (!OPTIONS_IS_STRING(o))
		fatalx("option %s is not a string", name);
	return (o->string);
}

long long
options_get_number(struct options *oo, const char *name)
{
	struct option	*o;

	o = options_get(oo, name);
	if (o == NULL)
		fatalx("missing option %s", name);
	if (!OPTIONS_IS_NUMBER(o))
	    fatalx("option %s is not a number", name);
	return (o->number);
}

const struct grid_cell *
options_get_style(struct options *oo, const char *name)
{
	struct option	*o;

	o = options_get(oo, name);
	if (o == NULL)
		fatalx("missing option %s", name);
	if (!OPTIONS_IS_STYLE(o))
		fatalx("option %s is not a style", name);
	return (&o->style);
}

struct option *
options_set_string(struct options *oo, const char *name, int append,
    const char *fmt, ...)
{
	struct option	*o;
	va_list		 ap;
	char		*s, *value;

	va_start(ap, fmt);
	xvasprintf(&s, fmt, ap);
	va_end(ap);

	o = options_get_only(oo, name);
	if (o != NULL && append && OPTIONS_IS_STRING(o)) {
		xasprintf(&value, "%s%s", o->string, s);
		free(s);
	} else
		value = s;
	if (o == NULL && *name == '@')
		o = options_add(oo, name);
	else if (o == NULL) {
		o = options_default(oo, options_parent_table_entry(oo, name));
		if (o == NULL)
			return (NULL);
	}

	if (!OPTIONS_IS_STRING(o))
		fatalx("option %s is not a string", name);
	free(o->string);
	o->string = value;
	return (o);
}

struct option *
options_set_number(struct options *oo, const char *name, long long value)
{
	struct option	*o;

	if (*name == '@')
		fatalx("user option %s must be a string", name);

	o = options_get_only(oo, name);
	if (o == NULL) {
		o = options_default(oo, options_parent_table_entry(oo, name));
		if (o == NULL)
			return (NULL);
	}

	if (!OPTIONS_IS_NUMBER(o))
		fatalx("option %s is not a number", name);
	o->number = value;
	return (o);
}

struct option *
options_set_style(struct options *oo, const char *name, int append,
    const char *value)
{
	struct option		*o;
	struct grid_cell	 gc;

	if (*name == '@')
		fatalx("user option %s must be a string", name);

	o = options_get_only(oo, name);
	if (o != NULL && append && OPTIONS_IS_STYLE(o))
		memcpy(&gc, &o->style, sizeof gc);
	else
		memcpy(&gc, &grid_default_cell, sizeof gc);
	if (style_parse(&grid_default_cell, &gc, value) == -1)
		return (NULL);
	if (o == NULL) {
		o = options_default(oo, options_parent_table_entry(oo, name));
		if (o == NULL)
			return (NULL);
	}

	if (!OPTIONS_IS_STYLE(o))
		fatalx("option %s is not a style", name);
	memcpy(&o->style, &gc, sizeof o->style);
	return (o);
}

enum options_table_scope
options_scope_from_flags(struct args *args, int window,
    struct cmd_find_state *fs, struct options **oo, char **cause)
{
	struct session	*s = fs->s;
	struct winlink	*wl = fs->wl;
	const char	*target= args_get(args, 't');

	if (args_has(args, 's')) {
		*oo = global_options;
		return (OPTIONS_TABLE_SERVER);
	}

	if (window || args_has(args, 'w')) {
		if (args_has(args, 'g')) {
			*oo = global_w_options;
			return (OPTIONS_TABLE_WINDOW);
		}
		if (wl == NULL) {
			if (target != NULL)
				xasprintf(cause, "no such window: %s", target);
			else
				xasprintf(cause, "no current window");
			return (OPTIONS_TABLE_NONE);
		}
		*oo = wl->window->options;
		return (OPTIONS_TABLE_WINDOW);
	} else {
		if (args_has(args, 'g')) {
			*oo = global_s_options;
			return (OPTIONS_TABLE_SESSION);
		}
		if (s == NULL) {
			if (target != NULL)
				xasprintf(cause, "no such session: %s", target);
			else
				xasprintf(cause, "no current session");
			return (OPTIONS_TABLE_NONE);
		}
		*oo = s->options;
		return (OPTIONS_TABLE_SESSION);
	}
}

void
options_style_update_new(struct options *oo, struct option *o)
{
	const char	*newname = o->tableentry->style;
	struct option	*new;

	if (newname == NULL)
		return;
	new = options_get_only(oo, newname);
	if (new == NULL)
		new = options_set_style(oo, newname, 0, "default");

	if (strstr(o->name, "-bg") != NULL)
		new->style.bg = o->number;
	else if (strstr(o->name, "-fg") != NULL)
		new->style.fg = o->number;
	else if (strstr(o->name, "-attr") != NULL)
		new->style.attr = o->number;
}

void
options_style_update_old(struct options *oo, struct option *o)
{
	char	newname[128];
	int	size;

	size = strrchr(o->name, '-') - o->name;

	xsnprintf(newname, sizeof newname, "%.*s-bg", size, o->name);
	options_set_number(oo, newname, o->style.bg);

	xsnprintf(newname, sizeof newname, "%.*s-fg", size, o->name);
	options_set_number(oo, newname, o->style.fg);

	xsnprintf(newname, sizeof newname, "%.*s-attr", size, o->name);
	options_set_number(oo, newname, o->style.attr);
}
