/* $OpenBSD: cmd-respawn-pane.c,v 1.22 2016/10/16 19:04:05 nicm Exp $ */

/*
 * Copyright (c) 2008 Nicholas Marriott <nicholas.marriott@gmail.com>
 * Copyright (c) 2011 Marcel P. Partap <mpartap@gmx.net>
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

#include <stdlib.h>
#include <unistd.h>

#include "tmux.h"

/*
 * Respawn a pane (restart the command). Kill existing if -k given.
 */

static enum cmd_retval	cmd_respawn_pane_exec(struct cmd *, struct cmdq_item *);

const struct cmd_entry cmd_respawn_pane_entry = {
	.name = "respawn-pane",
	.alias = "respawnp",

	.args = { "kt:", 0, -1 },
	.usage = "[-k] " CMD_TARGET_PANE_USAGE " [command]",

	.tflag = CMD_PANE,

	.flags = 0,
	.exec = cmd_respawn_pane_exec
};

static enum cmd_retval
cmd_respawn_pane_exec(struct cmd *self, struct cmdq_item *item)
{
	struct args		*args = self->args;
	struct winlink		*wl = item->state.tflag.wl;
	struct window		*w = wl->window;
	struct window_pane	*wp = item->state.tflag.wp;
	struct session		*s = item->state.tflag.s;
	struct environ		*env;
	const char		*path;
	char			*cause;
	u_int			 idx;
	struct environ_entry	*envent;

	if (!args_has(self->args, 'k') && wp->fd != -1) {
		if (window_pane_index(wp, &idx) != 0)
			fatalx("index not found");
		cmdq_error(item, "pane still active: %s:%d.%u",
		    s->name, wl->idx, idx);
		return (CMD_RETURN_ERROR);
	}

	env = environ_create();
	environ_copy(global_environ, env);
	environ_copy(s->environ, env);
	server_fill_environ(s, env);

	window_pane_reset_mode(wp);
	screen_reinit(&wp->base);
	input_init(wp);

	path = NULL;
	if (item->client != NULL && item->client->session == NULL)
		envent = environ_find(item->client->environ, "PATH");
	else
		envent = environ_find(s->environ, "PATH");
	if (envent != NULL)
		path = envent->value;

	if (window_pane_spawn(wp, args->argc, args->argv, path, NULL, NULL, env,
	    s->tio, &cause) != 0) {
		cmdq_error(item, "respawn pane failed: %s", cause);
		free(cause);
		environ_free(env);
		return (CMD_RETURN_ERROR);
	}
	wp->flags |= PANE_REDRAW;
	server_status_window(w);

	environ_free(env);
	return (CMD_RETURN_NORMAL);
}
