/* $OpenBSD: auth2-pubkey.c,v 1.66 2017/05/31 09:15:42 deraadt Exp $ */
/*
 * Copyright (c) 2000 Markus Friedl.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <errno.h>
#include <fcntl.h>
#include <paths.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>

#include "xmalloc.h"
#include "ssh.h"
#include "ssh2.h"
#include "packet.h"
#include "buffer.h"
#include "log.h"
#include "misc.h"
#include "servconf.h"
#include "compat.h"
#include "sshkey.h"
#include "hostfile.h"
#include "auth.h"
#include "pathnames.h"
#include "uidswap.h"
#include "auth-options.h"
#include "canohost.h"
#ifdef GSSAPI
#include "ssh-gss.h"
#endif
#include "monitor_wrap.h"
#include "authfile.h"
#include "match.h"
#include "ssherr.h"
#include "channels.h" /* XXX for session.h */
#include "session.h" /* XXX for child_set_env(); refactor? */

/* import */
extern ServerOptions options;
extern u_char *session_id2;
extern u_int session_id2_len;

static int
userauth_pubkey(struct ssh *ssh)
{
	Authctxt *authctxt = ssh->authctxt;
	struct sshbuf *b;
	struct sshkey *key = NULL;
	char *pkalg, *userstyle = NULL, *fp = NULL;
	u_char *pkblob, *sig, have_sig;
	size_t blen, slen;
	int r, pktype;
	int authenticated = 0;

	if (!authctxt->valid) {
		debug2("%s: disabled because of invalid user", __func__);
		return 0;
	}
	if ((r = sshpkt_get_u8(ssh, &have_sig)) != 0)
		fatal("%s: sshpkt_get_u8 failed: %s", __func__, ssh_err(r));
	if (ssh->compat & SSH_BUG_PKAUTH) {
		debug2("%s: SSH_BUG_PKAUTH", __func__);
		if ((b = sshbuf_new()) == NULL)
			fatal("%s: sshbuf_new failed", __func__);
		/* no explicit pkalg given */
		/* so we have to extract the pkalg from the pkblob */
		/* XXX use sshbuf_from() */
		if ((r = sshpkt_get_string(ssh, &pkblob, &blen)) != 0 ||
		    (r = sshbuf_put(b, pkblob, blen)) != 0 ||
		    (r = sshbuf_get_cstring(b, &pkalg, NULL)) != 0)
			fatal("%s: failed: %s", __func__, ssh_err(r));
		sshbuf_free(b);
	} else {
		if ((r = sshpkt_get_cstring(ssh, &pkalg, NULL)) != 0 ||
		    (r = sshpkt_get_string(ssh, &pkblob, &blen)) != 0)
			fatal("%s: sshpkt_get_cstring failed: %s",
			    __func__, ssh_err(r));
	}
	pktype = sshkey_type_from_name(pkalg);
	if (pktype == KEY_UNSPEC) {
		/* this is perfectly legal */
		logit("%s: unsupported public key algorithm: %s",
		    __func__, pkalg);
		goto done;
	}
	if ((r = sshkey_from_blob(pkblob, blen, &key)) != 0) {
		error("%s: could not parse key: %s", __func__, ssh_err(r));
		goto done;
	}
	if (key == NULL) {
		error("%s: cannot decode key: %s", __func__, pkalg);
		goto done;
	}
	if (key->type != pktype) {
		error("%s: type mismatch for decoded key "
		    "(received %d, expected %d)", __func__, key->type, pktype);
		goto done;
	}
	if (sshkey_type_plain(key->type) == KEY_RSA &&
	    (ssh->compat & SSH_BUG_RSASIGMD5) != 0) {
		logit("Refusing RSA key because client uses unsafe "
		    "signature scheme");
		goto done;
	}
	fp = sshkey_fingerprint(key, options.fingerprint_hash, SSH_FP_DEFAULT);
	if (auth2_userkey_already_used(authctxt, key)) {
		logit("refusing previously-used %s key", sshkey_type(key));
		goto done;
	}
	if (match_pattern_list(sshkey_ssh_name(key),
	    options.pubkey_key_types, 0) != 1) {
		logit("%s: key type %s not in PubkeyAcceptedKeyTypes",
		    __func__, sshkey_ssh_name(key));
		goto done;
	}

	if (have_sig) {
		debug3("%s: have signature for %s %s",
		    __func__, sshkey_type(key), fp);
		if ((r = sshpkt_get_string(ssh, &sig, &slen)) != 0 ||
		    (r = sshpkt_get_end(ssh)) != 0)
			fatal("%s: %s", __func__, ssh_err(r));
		if ((b = sshbuf_new()) == NULL)
			fatal("%s: sshbuf_new failed", __func__);
		if (ssh->compat & SSH_OLD_SESSIONID) {
			if ((r = sshbuf_put(b, session_id2,
			    session_id2_len)) != 0)
				fatal("%s: sshbuf_put session id: %s",
				    __func__, ssh_err(r));
		} else {
			if ((r = sshbuf_put_string(b, session_id2,
			    session_id2_len)) != 0)
				fatal("%s: sshbuf_put_string session id: %s",
				    __func__, ssh_err(r));
		}
		/* reconstruct packet */
		xasprintf(&userstyle, "%s%s%s", authctxt->user,
		    authctxt->style ? ":" : "",
		    authctxt->style ? authctxt->style : "");
		if ((r = sshbuf_put_u8(b, SSH2_MSG_USERAUTH_REQUEST)) != 0 ||
		    (r = sshbuf_put_cstring(b, userstyle)) != 0 ||
		    (r = sshbuf_put_cstring(b, ssh->compat & SSH_BUG_PKSERVICE ?
		    "ssh-userauth" : authctxt->service)) != 0)
			fatal("%s: build packet failed: %s",
			    __func__, ssh_err(r));
		if (ssh->compat & SSH_BUG_PKAUTH) {
			if ((r = sshbuf_put_u8(b, have_sig)) != 0)
				fatal("%s: build packet failed: %s",
				    __func__, ssh_err(r));
		} else {
			if ((r = sshbuf_put_cstring(b, "publickey")) != 0 ||
			    (r = sshbuf_put_u8(b, have_sig)) != 0 ||
			    (r = sshbuf_put_cstring(b, pkalg) != 0))
				fatal("%s: build packet failed: %s",
				    __func__, ssh_err(r));
		}
		if ((r = sshbuf_put_string(b, pkblob, blen)) != 0)
			fatal("%s: build packet failed: %s",
			    __func__, ssh_err(r));
#ifdef DEBUG_PK
		sshbuf_dump(b, stderr);
#endif
		pubkey_auth_info(authctxt, key, NULL);

		/* test for correct signature */
		authenticated = 0;
		if (PRIVSEP(user_key_allowed(authctxt->pw, key, 1)) &&
		    PRIVSEP(sshkey_verify(key, sig, slen, sshbuf_ptr(b),
		    sshbuf_len(b), ssh->compat)) == 0) {
			authenticated = 1;
			/* Record the successful key to prevent reuse */
			auth2_record_userkey(authctxt, key);
			key = NULL; /* Don't free below */
		}
		sshbuf_free(b);
		free(sig);
	} else {
		debug("%s: test whether pkalg/pkblob are acceptable for %s %s",
		    __func__, sshkey_type(key), fp);
		if ((r = sshpkt_get_end(ssh)) != 0)
			fatal("%s: %s", __func__, ssh_err(r));

		/* XXX fake reply and always send PK_OK ? */
		/*
		 * XXX this allows testing whether a user is allowed
		 * to login: if you happen to have a valid pubkey this
		 * message is sent. the message is NEVER sent at all
		 * if a user is not allowed to login. is this an
		 * issue? -markus
		 */
		if (PRIVSEP(user_key_allowed(authctxt->pw, key, 0))) {
			if ((r = sshpkt_start(ssh, SSH2_MSG_USERAUTH_PK_OK))
			    != 0 ||
			    (r = sshpkt_put_cstring(ssh, pkalg)) != 0 ||
			    (r = sshpkt_put_string(ssh, pkblob, blen)) != 0 ||
			    (r = sshpkt_send(ssh)) != 0)
				fatal("%s: %s", __func__, ssh_err(r));
			ssh_packet_write_wait(ssh);
			authctxt->postponed = 1;
		}
	}
	if (authenticated != 1)
		auth_clear_options();
done:
	debug2("%s: authenticated %d pkalg %s", __func__, authenticated, pkalg);
	if (key != NULL)
		sshkey_free(key);
	free(userstyle);
	free(pkalg);
	free(pkblob);
	free(fp);
	return authenticated;
}

void
pubkey_auth_info(Authctxt *authctxt, const struct sshkey *key,
    const char *fmt, ...)
{
	char *fp, *extra;
	va_list ap;
	int i;

	extra = NULL;
	if (fmt != NULL) {
		va_start(ap, fmt);
		i = vasprintf(&extra, fmt, ap);
		va_end(ap);
		if (i < 0 || extra == NULL)
			fatal("%s: vasprintf failed", __func__);
	}

	if (sshkey_is_cert(key)) {
		fp = sshkey_fingerprint(key->cert->signature_key,
		    options.fingerprint_hash, SSH_FP_DEFAULT);
		auth_info(authctxt, "%s ID %s (serial %llu) CA %s %s%s%s", 
		    sshkey_type(key), key->cert->key_id,
		    (unsigned long long)key->cert->serial,
		    sshkey_type(key->cert->signature_key),
		    fp == NULL ? "(null)" : fp,
		    extra == NULL ? "" : ", ", extra == NULL ? "" : extra);
		free(fp);
	} else {
		fp = sshkey_fingerprint(key, options.fingerprint_hash,
		    SSH_FP_DEFAULT);
		auth_info(authctxt, "%s %s%s%s", sshkey_type(key),
		    fp == NULL ? "(null)" : fp,
		    extra == NULL ? "" : ", ", extra == NULL ? "" : extra);
		free(fp);
	}
	free(extra);
}

/*
 * Splits 's' into an argument vector. Handles quoted string and basic
 * escape characters (\\, \", \'). Caller must free the argument vector
 * and its members.
 */
static int
split_argv(const char *s, int *argcp, char ***argvp)
{
	int r = SSH_ERR_INTERNAL_ERROR;
	int argc = 0, quote, i, j;
	char *arg, **argv = xcalloc(1, sizeof(*argv));

	*argvp = NULL;
	*argcp = 0;

	for (i = 0; s[i] != '\0'; i++) {
		/* Skip leading whitespace */
		if (s[i] == ' ' || s[i] == '\t')
			continue;

		/* Start of a token */
		quote = 0;
		if (s[i] == '\\' &&
		    (s[i + 1] == '\'' || s[i + 1] == '\"' || s[i + 1] == '\\'))
			i++;
		else if (s[i] == '\'' || s[i] == '"')
			quote = s[i++];

		argv = xreallocarray(argv, (argc + 2), sizeof(*argv));
		arg = argv[argc++] = xcalloc(1, strlen(s + i) + 1);
		argv[argc] = NULL;

		/* Copy the token in, removing escapes */
		for (j = 0; s[i] != '\0'; i++) {
			if (s[i] == '\\') {
				if (s[i + 1] == '\'' ||
				    s[i + 1] == '\"' ||
				    s[i + 1] == '\\') {
					i++; /* Skip '\' */
					arg[j++] = s[i];
				} else {
					/* Unrecognised escape */
					arg[j++] = s[i];
				}
			} else if (quote == 0 && (s[i] == ' ' || s[i] == '\t'))
				break; /* done */
			else if (quote != 0 && s[i] == quote)
				break; /* done */
			else
				arg[j++] = s[i];
		}
		if (s[i] == '\0') {
			if (quote != 0) {
				/* Ran out of string looking for close quote */
				r = SSH_ERR_INVALID_FORMAT;
				goto out;
			}
			break;
		}
	}
	/* Success */
	*argcp = argc;
	*argvp = argv;
	argc = 0;
	argv = NULL;
	r = 0;
 out:
	if (argc != 0 && argv != NULL) {
		for (i = 0; i < argc; i++)
			free(argv[i]);
		free(argv);
	}
	return r;
}

/*
 * Reassemble an argument vector into a string, quoting and escaping as
 * necessary. Caller must free returned string.
 */
static char *
assemble_argv(int argc, char **argv)
{
	int i, j, ws, r;
	char c, *ret;
	struct sshbuf *buf, *arg;

	if ((buf = sshbuf_new()) == NULL || (arg = sshbuf_new()) == NULL)
		fatal("%s: sshbuf_new failed", __func__);

	for (i = 0; i < argc; i++) {
		ws = 0;
		sshbuf_reset(arg);
		for (j = 0; argv[i][j] != '\0'; j++) {
			r = 0;
			c = argv[i][j];
			switch (c) {
			case ' ':
			case '\t':
				ws = 1;
				r = sshbuf_put_u8(arg, c);
				break;
			case '\\':
			case '\'':
			case '"':
				if ((r = sshbuf_put_u8(arg, '\\')) != 0)
					break;
				/* FALLTHROUGH */
			default:
				r = sshbuf_put_u8(arg, c);
				break;
			}
			if (r != 0)
				fatal("%s: sshbuf_put_u8: %s",
				    __func__, ssh_err(r));
		}
		if ((i != 0 && (r = sshbuf_put_u8(buf, ' ')) != 0) ||
		    (ws != 0 && (r = sshbuf_put_u8(buf, '"')) != 0) ||
		    (r = sshbuf_putb(buf, arg)) != 0 ||
		    (ws != 0 && (r = sshbuf_put_u8(buf, '"')) != 0))
			fatal("%s: buffer error: %s", __func__, ssh_err(r));
	}
	if ((ret = malloc(sshbuf_len(buf) + 1)) == NULL)
		fatal("%s: malloc failed", __func__);
	memcpy(ret, sshbuf_ptr(buf), sshbuf_len(buf));
	ret[sshbuf_len(buf)] = '\0';
	sshbuf_free(buf);
	sshbuf_free(arg);
	return ret;
}

/*
 * Runs command in a subprocess. Returns pid on success and a FILE* to the
 * subprocess' stdout or 0 on failure.
 * NB. "command" is only used for logging.
 */
static pid_t
subprocess(const char *tag, struct passwd *pw, const char *command,
    int ac, char **av, FILE **child)
{
	FILE *f;
	struct stat st;
	int devnull, p[2], i;
	pid_t pid;
	char *cp, errmsg[512];
	u_int envsize;
	char **child_env;

	*child = NULL;

	debug3("%s: %s command \"%s\" running as %s", __func__,
	    tag, command, pw->pw_name);

	/* Verify the path exists and is safe-ish to execute */
	if (*av[0] != '/') {
		error("%s path is not absolute", tag);
		return 0;
	}
	temporarily_use_uid(pw);
	if (stat(av[0], &st) < 0) {
		error("Could not stat %s \"%s\": %s", tag,
		    av[0], strerror(errno));
		restore_uid();
		return 0;
	}
	if (auth_secure_path(av[0], &st, NULL, 0,
	    errmsg, sizeof(errmsg)) != 0) {
		error("Unsafe %s \"%s\": %s", tag, av[0], errmsg);
		restore_uid();
		return 0;
	}

	/*
	 * Run the command; stderr is left in place, stdout is the
	 * authorized_keys output.
	 */
	if (pipe(p) != 0) {
		error("%s: pipe: %s", tag, strerror(errno));
		restore_uid();
		return 0;
	}

	/*
	 * Don't want to call this in the child, where it can fatal() and
	 * run cleanup_exit() code.
	 */
	restore_uid();

	switch ((pid = fork())) {
	case -1: /* error */
		error("%s: fork: %s", tag, strerror(errno));
		close(p[0]);
		close(p[1]);
		return 0;
	case 0: /* child */
		/* Prepare a minimal environment for the child. */
		envsize = 5;
		child_env = xcalloc(sizeof(*child_env), envsize);
		child_set_env(&child_env, &envsize, "PATH", _PATH_STDPATH);
		child_set_env(&child_env, &envsize, "USER", pw->pw_name);
		child_set_env(&child_env, &envsize, "LOGNAME", pw->pw_name);
		child_set_env(&child_env, &envsize, "HOME", pw->pw_dir);
		if ((cp = getenv("LANG")) != NULL)
			child_set_env(&child_env, &envsize, "LANG", cp);

		for (i = 0; i < NSIG; i++)
			signal(i, SIG_DFL);

		if ((devnull = open(_PATH_DEVNULL, O_RDWR)) == -1) {
			error("%s: open %s: %s", tag, _PATH_DEVNULL,
			    strerror(errno));
			_exit(1);
		}
		/* Keep stderr around a while longer to catch errors */
		if (dup2(devnull, STDIN_FILENO) == -1 ||
		    dup2(p[1], STDOUT_FILENO) == -1) {
			error("%s: dup2: %s", tag, strerror(errno));
			_exit(1);
		}
		closefrom(STDERR_FILENO + 1);

		/* Don't use permanently_set_uid() here to avoid fatal() */
		if (setresgid(pw->pw_gid, pw->pw_gid, pw->pw_gid) != 0) {
			error("%s: setresgid %u: %s", tag, (u_int)pw->pw_gid,
			    strerror(errno));
			_exit(1);
		}
		if (setresuid(pw->pw_uid, pw->pw_uid, pw->pw_uid) != 0) {
			error("%s: setresuid %u: %s", tag, (u_int)pw->pw_uid,
			    strerror(errno));
			_exit(1);
		}
		/* stdin is pointed to /dev/null at this point */
		if (dup2(STDIN_FILENO, STDERR_FILENO) == -1) {
			error("%s: dup2: %s", tag, strerror(errno));
			_exit(1);
		}

		execve(av[0], av, child_env);
		error("%s exec \"%s\": %s", tag, command, strerror(errno));
		_exit(127);
	default: /* parent */
		break;
	}

	close(p[1]);
	if ((f = fdopen(p[0], "r")) == NULL) {
		error("%s: fdopen: %s", tag, strerror(errno));
		close(p[0]);
		/* Don't leave zombie child */
		kill(pid, SIGTERM);
		while (waitpid(pid, NULL, 0) == -1 && errno == EINTR)
			;
		return 0;
	}
	/* Success */
	debug3("%s: %s pid %ld", __func__, tag, (long)pid);
	*child = f;
	return pid;
}

/* Returns 0 if pid exited cleanly, non-zero otherwise */
static int
exited_cleanly(pid_t pid, const char *tag, const char *cmd)
{
	int status;

	while (waitpid(pid, &status, 0) == -1) {
		if (errno != EINTR) {
			error("%s: waitpid: %s", tag, strerror(errno));
			return -1;
		}
	}
	if (WIFSIGNALED(status)) {
		error("%s %s exited on signal %d", tag, cmd, WTERMSIG(status));
		return -1;
	} else if (WEXITSTATUS(status) != 0) {
		error("%s %s failed, status %d", tag, cmd, WEXITSTATUS(status));
		return -1;
	}
	return 0;
}

static int
match_principals_option(const char *principal_list, struct sshkey_cert *cert)
{
	char *result;
	u_int i;

	/* XXX percent_expand() sequences for authorized_principals? */

	for (i = 0; i < cert->nprincipals; i++) {
		if ((result = match_list(cert->principals[i],
		    principal_list, NULL)) != NULL) {
			debug3("matched principal from key options \"%.100s\"",
			    result);
			free(result);
			return 1;
		}
	}
	return 0;
}

static int
process_principals(FILE *f, char *file, struct passwd *pw,
    const struct sshkey_cert *cert)
{
	char line[SSH_MAX_PUBKEY_BYTES], *cp, *ep, *line_opts;
	u_long linenum = 0;
	u_int i, found_principal = 0;

	while (read_keyfile_line(f, file, line, sizeof(line), &linenum) != -1) {
		/* Always consume entire input */
		if (found_principal)
			continue;
		/* Skip leading whitespace. */
		for (cp = line; *cp == ' ' || *cp == '\t'; cp++)
			;
		/* Skip blank and comment lines. */
		if ((ep = strchr(cp, '#')) != NULL)
			*ep = '\0';
		if (!*cp || *cp == '\n')
			continue;
		/* Trim trailing whitespace. */
		ep = cp + strlen(cp) - 1;
		while (ep > cp && (*ep == '\n' || *ep == ' ' || *ep == '\t'))
			*ep-- = '\0';
		/*
		 * If the line has internal whitespace then assume it has
		 * key options.
		 */
		line_opts = NULL;
		if ((ep = strrchr(cp, ' ')) != NULL ||
		    (ep = strrchr(cp, '\t')) != NULL) {
			for (; *ep == ' ' || *ep == '\t'; ep++)
				;
			line_opts = cp;
			cp = ep;
		}
		for (i = 0; i < cert->nprincipals; i++) {
			if (strcmp(cp, cert->principals[i]) == 0) {
				debug3("%s:%lu: matched principal \"%.100s\"",
				    file == NULL ? "(command)" : file,
				    linenum, cert->principals[i]);
				if (auth_parse_options(pw, line_opts,
				    file, linenum) != 1)
					continue;
				found_principal = 1;
				continue;
			}
		}
	}
	return found_principal;
}

static int
match_principals_file(char *file, struct passwd *pw, struct sshkey_cert *cert)
{
	FILE *f;
	int success;

	temporarily_use_uid(pw);
	debug("trying authorized principals file %s", file);
	if ((f = auth_openprincipals(file, pw, options.strict_modes)) == NULL) {
		restore_uid();
		return 0;
	}
	success = process_principals(f, file, pw, cert);
	fclose(f);
	restore_uid();
	return success;
}

/*
 * Checks whether principal is allowed in output of command.
 * returns 1 if the principal is allowed or 0 otherwise.
 */
static int
match_principals_command(struct passwd *user_pw, const struct sshkey *key)
{
	const struct sshkey_cert *cert = key->cert;
	FILE *f = NULL;
	int r, ok, found_principal = 0;
	struct passwd *pw;
	int i, ac = 0, uid_swapped = 0;
	pid_t pid;
	char *tmp, *username = NULL, *command = NULL, **av = NULL;
	char *ca_fp = NULL, *key_fp = NULL, *catext = NULL, *keytext = NULL;
	char serial_s[16];
	void (*osigchld)(int);

	if (options.authorized_principals_command == NULL)
		return 0;
	if (options.authorized_principals_command_user == NULL) {
		error("No user for AuthorizedPrincipalsCommand specified, "
		    "skipping");
		return 0;
	}

	/*
	 * NB. all returns later this function should go via "out" to
	 * ensure the original SIGCHLD handler is restored properly.
	 */
	osigchld = signal(SIGCHLD, SIG_DFL);

	/* Prepare and verify the user for the command */
	username = percent_expand(options.authorized_principals_command_user,
	    "u", user_pw->pw_name, (char *)NULL);
	pw = getpwnam(username);
	if (pw == NULL) {
		error("AuthorizedPrincipalsCommandUser \"%s\" not found: %s",
		    username, strerror(errno));
		goto out;
	}

	/* Turn the command into an argument vector */
	if (split_argv(options.authorized_principals_command, &ac, &av) != 0) {
		error("AuthorizedPrincipalsCommand \"%s\" contains "
		    "invalid quotes", command);
		goto out;
	}
	if (ac == 0) {
		error("AuthorizedPrincipalsCommand \"%s\" yielded no arguments",
		    command);
		goto out;
	}
	if ((ca_fp = sshkey_fingerprint(cert->signature_key,
	    options.fingerprint_hash, SSH_FP_DEFAULT)) == NULL) {
		error("%s: sshkey_fingerprint failed", __func__);
		goto out;
	}
	if ((key_fp = sshkey_fingerprint(key,
	    options.fingerprint_hash, SSH_FP_DEFAULT)) == NULL) {
		error("%s: sshkey_fingerprint failed", __func__);
		goto out;
	}
	if ((r = sshkey_to_base64(cert->signature_key, &catext)) != 0) {
		error("%s: sshkey_to_base64 failed: %s", __func__, ssh_err(r));
		goto out;
	}
	if ((r = sshkey_to_base64(key, &keytext)) != 0) {
		error("%s: sshkey_to_base64 failed: %s", __func__, ssh_err(r));
		goto out;
	}
	snprintf(serial_s, sizeof(serial_s), "%llu",
	    (unsigned long long)cert->serial);
	for (i = 1; i < ac; i++) {
		tmp = percent_expand(av[i],
		    "u", user_pw->pw_name,
		    "h", user_pw->pw_dir,
		    "t", sshkey_ssh_name(key),
		    "T", sshkey_ssh_name(cert->signature_key),
		    "f", key_fp,
		    "F", ca_fp,
		    "k", keytext,
		    "K", catext,
		    "i", cert->key_id,
		    "s", serial_s,
		    (char *)NULL);
		if (tmp == NULL)
			fatal("%s: percent_expand failed", __func__);
		free(av[i]);
		av[i] = tmp;
	}
	/* Prepare a printable command for logs, etc. */
	command = assemble_argv(ac, av);

	if ((pid = subprocess("AuthorizedPrincipalsCommand", pw, command,
	    ac, av, &f)) == 0)
		goto out;

	uid_swapped = 1;
	temporarily_use_uid(pw);

	ok = process_principals(f, NULL, pw, cert);

	fclose(f);
	f = NULL;

	if (exited_cleanly(pid, "AuthorizedPrincipalsCommand", command) != 0)
		goto out;

	/* Read completed successfully */
	found_principal = ok;
 out:
	if (f != NULL)
		fclose(f);
	signal(SIGCHLD, osigchld);
	for (i = 0; i < ac; i++)
		free(av[i]);
	free(av);
	if (uid_swapped)
		restore_uid();
	free(command);
	free(username);
	free(ca_fp);
	free(key_fp);
	free(catext);
	free(keytext);
	return found_principal;
}
/*
 * Checks whether key is allowed in authorized_keys-format file,
 * returns 1 if the key is allowed or 0 otherwise.
 */
static int
check_authkeys_file(FILE *f, char *file, struct sshkey *key, struct passwd *pw)
{
	char line[SSH_MAX_PUBKEY_BYTES];
	int found_key = 0;
	u_long linenum = 0;
	struct sshkey *found = NULL;

	while (read_keyfile_line(f, file, line, sizeof(line), &linenum) != -1) {
		char *cp, *key_options = NULL, *fp = NULL;
		const char *reason = NULL;

		/* Always consume entrire file */
		if (found_key)
			continue;
		if (found != NULL)
			sshkey_free(found);
		found = sshkey_new(sshkey_is_cert(key) ? KEY_UNSPEC : key->type);
		if (found == NULL)
			goto done;
		auth_clear_options();

		/* Skip leading whitespace, empty and comment lines. */
		for (cp = line; *cp == ' ' || *cp == '\t'; cp++)
			;
		if (!*cp || *cp == '\n' || *cp == '#')
			continue;

		if (sshkey_read(found, &cp) != 0) {
			/* no key?  check if there are options for this key */
			int quoted = 0;
			debug2("user_key_allowed: check options: '%s'", cp);
			key_options = cp;
			for (; *cp && (quoted || (*cp != ' ' && *cp != '\t')); cp++) {
				if (*cp == '\\' && cp[1] == '"')
					cp++;	/* Skip both */
				else if (*cp == '"')
					quoted = !quoted;
			}
			/* Skip remaining whitespace. */
			for (; *cp == ' ' || *cp == '\t'; cp++)
				;
			if (sshkey_read(found, &cp) != 0) {
				debug2("user_key_allowed: advance: '%s'", cp);
				/* still no key?  advance to next line*/
				continue;
			}
		}
		if (sshkey_is_cert(key)) {
			if (!sshkey_equal(found, key->cert->signature_key))
				continue;
			if (auth_parse_options(pw, key_options, file,
			    linenum) != 1)
				continue;
			if (!key_is_cert_authority)
				continue;
			if ((fp = sshkey_fingerprint(found,
			    options.fingerprint_hash, SSH_FP_DEFAULT)) == NULL)
				continue;
			debug("matching CA found: file %s, line %lu, %s %s",
			    file, linenum, sshkey_type(found), fp);
			/*
			 * If the user has specified a list of principals as
			 * a key option, then prefer that list to matching
			 * their username in the certificate principals list.
			 */
			if (authorized_principals != NULL &&
			    !match_principals_option(authorized_principals,
			    key->cert)) {
				reason = "Certificate does not contain an "
				    "authorized principal";
 fail_reason:
				free(fp);
				error("%s", reason);
				auth_debug_add("%s", reason);
				continue;
			}
			if (sshkey_cert_check_authority(key, 0, 0,
			    authorized_principals == NULL ? pw->pw_name : NULL,
			    &reason) != 0)
				goto fail_reason;
			if (auth_cert_options(key, pw, &reason) != 0)
				goto fail_reason;
			verbose("Accepted certificate ID \"%s\" (serial %llu) "
			    "signed by %s CA %s via %s", key->cert->key_id,
			    (unsigned long long)key->cert->serial,
			    sshkey_type(found), fp, file);
			free(fp);
			found_key = 1;
			break;
		} else if (sshkey_equal(found, key)) {
			if (auth_parse_options(pw, key_options, file,
			    linenum) != 1)
				continue;
			if (key_is_cert_authority)
				continue;
			if ((fp = sshkey_fingerprint(found,
			    options.fingerprint_hash, SSH_FP_DEFAULT)) == NULL)
				continue;
			debug("matching key found: file %s, line %lu %s %s",
			    file, linenum, sshkey_type(found), fp);
			free(fp);
			found_key = 1;
			continue;
		}
	}
 done:
	if (found != NULL)
		sshkey_free(found);
	if (!found_key)
		debug2("key not found");
	return found_key;
}

/* Authenticate a certificate key against TrustedUserCAKeys */
static int
user_cert_trusted_ca(struct passwd *pw, struct sshkey *key)
{
	char *ca_fp, *principals_file = NULL;
	const char *reason;
	int r, ret = 0, found_principal = 0, use_authorized_principals;

	if (!sshkey_is_cert(key) || options.trusted_user_ca_keys == NULL)
		return 0;

	if ((ca_fp = sshkey_fingerprint(key->cert->signature_key,
	    options.fingerprint_hash, SSH_FP_DEFAULT)) == NULL)
		return 0;

	if ((r = sshkey_in_file(key->cert->signature_key,
	    options.trusted_user_ca_keys, 1, 0)) != 0) {
		debug2("%s: CA %s %s is not listed in %s: %s", __func__,
		    sshkey_type(key->cert->signature_key), ca_fp,
		    options.trusted_user_ca_keys, ssh_err(r));
		goto out;
	}
	/*
	 * If AuthorizedPrincipals is in use, then compare the certificate
	 * principals against the names in that file rather than matching
	 * against the username.
	 */
	if ((principals_file = authorized_principals_file(pw)) != NULL) {
		if (match_principals_file(principals_file, pw, key->cert))
			found_principal = 1;
	}
	/* Try querying command if specified */
	if (!found_principal && match_principals_command(pw, key))
		found_principal = 1;
	/* If principals file or command is specified, then require a match */
	use_authorized_principals = principals_file != NULL ||
            options.authorized_principals_command != NULL;
	if (!found_principal && use_authorized_principals) {
		reason = "Certificate does not contain an authorized principal";
 fail_reason:
		error("%s", reason);
		auth_debug_add("%s", reason);
		goto out;
	}
	if (sshkey_cert_check_authority(key, 0, 1,
	    use_authorized_principals ? NULL : pw->pw_name, &reason) != 0)
		goto fail_reason;
	if (auth_cert_options(key, pw, &reason) != 0)
		goto fail_reason;

	verbose("Accepted certificate ID \"%s\" (serial %llu) signed by "
	    "%s CA %s via %s", key->cert->key_id,
	    (unsigned long long)key->cert->serial,
	    sshkey_type(key->cert->signature_key), ca_fp,
	    options.trusted_user_ca_keys);
	ret = 1;

 out:
	free(principals_file);
	free(ca_fp);
	return ret;
}

/*
 * Checks whether key is allowed in file.
 * returns 1 if the key is allowed or 0 otherwise.
 */
static int
user_key_allowed2(struct passwd *pw, struct sshkey *key, char *file)
{
	FILE *f;
	int found_key = 0;

	/* Temporarily use the user's uid. */
	temporarily_use_uid(pw);

	debug("trying public key file %s", file);
	if ((f = auth_openkeyfile(file, pw, options.strict_modes)) != NULL) {
		found_key = check_authkeys_file(f, file, key, pw);
		fclose(f);
	}

	restore_uid();
	return found_key;
}

/*
 * Checks whether key is allowed in output of command.
 * returns 1 if the key is allowed or 0 otherwise.
 */
static int
user_key_command_allowed2(struct passwd *user_pw, struct sshkey *key)
{
	FILE *f = NULL;
	int r, ok, found_key = 0;
	struct passwd *pw;
	int i, uid_swapped = 0, ac = 0;
	pid_t pid;
	char *username = NULL, *key_fp = NULL, *keytext = NULL;
	char *tmp, *command = NULL, **av = NULL;
	void (*osigchld)(int);

	if (options.authorized_keys_command == NULL)
		return 0;
	if (options.authorized_keys_command_user == NULL) {
		error("No user for AuthorizedKeysCommand specified, skipping");
		return 0;
	}

	/*
	 * NB. all returns later this function should go via "out" to
	 * ensure the original SIGCHLD handler is restored properly.
	 */
	osigchld = signal(SIGCHLD, SIG_DFL);

	/* Prepare and verify the user for the command */
	username = percent_expand(options.authorized_keys_command_user,
	    "u", user_pw->pw_name, (char *)NULL);
	pw = getpwnam(username);
	if (pw == NULL) {
		error("AuthorizedKeysCommandUser \"%s\" not found: %s",
		    username, strerror(errno));
		goto out;
	}

	/* Prepare AuthorizedKeysCommand */
	if ((key_fp = sshkey_fingerprint(key, options.fingerprint_hash,
	    SSH_FP_DEFAULT)) == NULL) {
		error("%s: sshkey_fingerprint failed", __func__);
		goto out;
	}
	if ((r = sshkey_to_base64(key, &keytext)) != 0) {
		error("%s: sshkey_to_base64 failed: %s", __func__, ssh_err(r));
		goto out;
	}

	/* Turn the command into an argument vector */
	if (split_argv(options.authorized_keys_command, &ac, &av) != 0) {
		error("AuthorizedKeysCommand \"%s\" contains invalid quotes",
		    command);
		goto out;
	}
	if (ac == 0) {
		error("AuthorizedKeysCommand \"%s\" yielded no arguments",
		    command);
		goto out;
	}
	for (i = 1; i < ac; i++) {
		tmp = percent_expand(av[i],
		    "u", user_pw->pw_name,
		    "h", user_pw->pw_dir,
		    "t", sshkey_ssh_name(key),
		    "f", key_fp,
		    "k", keytext,
		    (char *)NULL);
		if (tmp == NULL)
			fatal("%s: percent_expand failed", __func__);
		free(av[i]);
		av[i] = tmp;
	}
	/* Prepare a printable command for logs, etc. */
	command = assemble_argv(ac, av);

	/*
	 * If AuthorizedKeysCommand was run without arguments
	 * then fall back to the old behaviour of passing the
	 * target username as a single argument.
	 */
	if (ac == 1) {
		av = xreallocarray(av, ac + 2, sizeof(*av));
		av[1] = xstrdup(user_pw->pw_name);
		av[2] = NULL;
		/* Fix up command too, since it is used in log messages */
		free(command);
		xasprintf(&command, "%s %s", av[0], av[1]);
	}

	if ((pid = subprocess("AuthorizedKeysCommand", pw, command,
	    ac, av, &f)) == 0)
		goto out;

	uid_swapped = 1;
	temporarily_use_uid(pw);

	ok = check_authkeys_file(f, options.authorized_keys_command, key, pw);

	fclose(f);
	f = NULL;

	if (exited_cleanly(pid, "AuthorizedKeysCommand", command) != 0)
		goto out;

	/* Read completed successfully */
	found_key = ok;
 out:
	if (f != NULL)
		fclose(f);
	signal(SIGCHLD, osigchld);
	for (i = 0; i < ac; i++)
		free(av[i]);
	free(av);
	if (uid_swapped)
		restore_uid();
	free(command);
	free(username);
	free(key_fp);
	free(keytext);
	return found_key;
}

/*
 * Check whether key authenticates and authorises the user.
 */
int
user_key_allowed(struct passwd *pw, struct sshkey *key, int auth_attempt)
{
	u_int success, i;
	char *file;

	if (auth_key_is_revoked(key))
		return 0;
	if (sshkey_is_cert(key) &&
	    auth_key_is_revoked(key->cert->signature_key))
		return 0;

	success = user_cert_trusted_ca(pw, key);
	if (success)
		return success;

	success = user_key_command_allowed2(pw, key);
	if (success > 0)
		return success;

	for (i = 0; !success && i < options.num_authkeys_files; i++) {

		if (strcasecmp(options.authorized_keys_files[i], "none") == 0)
			continue;
		file = expand_authorized_keys(
		    options.authorized_keys_files[i], pw);

		success = user_key_allowed2(pw, key, file);
		free(file);
	}

	return success;
}

/* Records a public key in the list of previously-successful keys */
void
auth2_record_userkey(Authctxt *authctxt, struct sshkey *key)
{
	struct sshkey **tmp;

	if (authctxt->nprev_userkeys >= INT_MAX ||
	    (tmp = recallocarray(authctxt->prev_userkeys,
	    authctxt->nprev_userkeys, authctxt->nprev_userkeys + 1,
	    sizeof(*tmp))) == NULL)
		fatal("%s: recallocarray failed", __func__);
	authctxt->prev_userkeys = tmp;
	authctxt->prev_userkeys[authctxt->nprev_userkeys] = key;
	authctxt->nprev_userkeys++;
}

/* Checks whether a key has already been used successfully for authentication */
int
auth2_userkey_already_used(Authctxt *authctxt, struct sshkey *key)
{
	u_int i;

	for (i = 0; i < authctxt->nprev_userkeys; i++) {
		if (sshkey_equal_public(key, authctxt->prev_userkeys[i])) {
			return 1;
		}
	}
	return 0;
}

Authmethod method_pubkey = {
	"publickey",
	userauth_pubkey,
	&options.pubkey_authentication
};
