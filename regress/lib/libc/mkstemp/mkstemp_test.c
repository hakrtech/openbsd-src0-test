/*
 * Copyright (c) 2010  Philip Guenther <guenther@openbsd.org>
 *
 * Public domain.
 *
 * Verify that mkstemp() and mkstemps() doesn't overrun or underrun
 * the template buffer and that it can generate names that don't
 * contain any X's
 */

#include <sys/param.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_TEMPLATE_LEN	10
#define MAX_TRIES		100
#define MIN_Xs			6

#define SUFFIX	".suff"
#define SLEN	(sizeof SUFFIX - 1)

long pg;

/*
 * verify that a path generated by mkstemp() or mkstemp() looks like a
 * reasonable expansion of the template and matches the fd.  Returns true
 * if all the X's were replaced with non-X's
 */
int
check(int fd, char const *path, char const *prefix, size_t plen,
    char const *suffix, size_t slen, int tlen)
{
	struct stat sb, fsb;
	char const *p;

	if (tlen < MIN_Xs) {
		if (fd >= 0)
			errx(1, "mkstemp(%s) succeed with too few Xs", path);
		if (errno != EINVAL)
			err(1, "mkstemp(%s) failed with wrong errno", path);
		return 1;
	}
	if (fd < 0)
		err(1, "mkstemp(%s)", path);
	if (stat(path, &sb))
		err(1, "stat(%s)", path);
	if (fstat(fd, &fsb))
		err(1, "fstat(%d==%s)", fd, path);
	if (sb.st_dev != fsb.st_dev || sb.st_ino != fsb.st_ino)
		errx(1, "stat mismatch");
	close(fd);
	if (memcmp(path, prefix, plen) != 0)
		errx(1, "prefix changed!  %s vs %s", prefix, path);
	if (memcmp(path + plen + tlen, suffix, slen + 1) != 0)
		errx(1, "suffix changed!  %s vs %s", suffix, path);
	for (p = path + plen; p < path + plen + tlen; p++)
		if (*p == '\0')
			errx(1, "unexpected truncation");
		else if (*p == 'X')
			return 0;
	return 1;
}


void
try_mkstemp(char *p, char const *prefix, int len)
{
	char *q;
	size_t plen = strlen(prefix);
	int tries, fd;

	for (tries = 0; tries < MAX_TRIES; tries++) {
		memcpy(p, prefix, plen);
		memset(p + plen, 'X', len);
		p[plen + len] = '\0';
		fd = mkstemp(p);
		if (check(fd, p, prefix, plen, "", 0, len))
			return;
	}
	errx(1, "exceeded MAX_TRIES");
}

void
try_mkstemps(char *p, char const *prefix, int len, char const *suffix)
{
	char *q;
	size_t plen = strlen(prefix);
	size_t slen = strlen(suffix);
	int tries, fd;

	for (tries = 0; tries < MAX_TRIES; tries++) {
		memcpy(p, prefix, plen);
		memset(p + plen, 'X', len);
		memcpy(p + plen + len, suffix, slen + 1);
		fd = mkstemps(p, slen);
		if (check(fd, p, prefix, plen, suffix, slen, len))
			return;
	}
	errx(1, "exceeded MAX_TRIES");
}

int
main(void)
{
	struct stat sb, fsb;
	char cwd[MAXPATHLEN + 1];
	char *p;
	size_t clen;
	int i;

	pg = sysconf(_SC_PAGESIZE);
	if (getcwd(cwd, sizeof cwd - 1) == NULL)
		err(1, "getcwd");
	clen = strlen(cwd);
	cwd[clen++] = '/';
	cwd[clen] = '\0';
	p = mmap(NULL, pg * 3, PROT_READ | PROT_WRITE, MAP_ANON, -1, 0);
	if (p == MAP_FAILED)
		err(1, "mmap");
	if (mprotect(p, pg, PROT_NONE) || mprotect(p + pg * 2, pg, PROT_NONE))
		err(1, "mprotect");
	p += pg;

	i = MAX_TEMPLATE_LEN + 1;
	while (i-- > 0) {
		/* try first at the start of a page, no prefix */
		try_mkstemp(p, "", i);
		/* now at the end of the page, no prefix */
		try_mkstemp(p + pg - i - 1, "", i);
		/* start of the page, prefixed with the cwd */
		try_mkstemp(p, cwd, i);
		/* how about at the end of the page, prefixed with cwd? */
		try_mkstemp(p + pg - clen - i - 1, cwd, i);

		/* again, with mkstemps() and an empty suffix */
		/* try first at the start of a page, no prefix */
		try_mkstemps(p, "", i, "");
		/* now at the end of the page, no prefix */
		try_mkstemps(p + pg - i - 1, "", i, "");
		/* start of the page, prefixed with the cwd */
		try_mkstemps(p, cwd, i, "");
		/* how about at the end of the page, prefixed with cwd? */
		try_mkstemps(p + pg - clen - i - 1, cwd, i, "");

		/* mkstemps() and a non-empty suffix */
		/* try first at the start of a page, no prefix */
		try_mkstemps(p, "", i, SUFFIX);
		/* now at the end of the page, no prefix */
		try_mkstemps(p + pg - i - SLEN - 1, "", i, SUFFIX);
		/* start of the page, prefixed with the cwd */
		try_mkstemps(p, cwd, i, SUFFIX);
		/* how about at the end of the page, prefixed with cwd? */
		try_mkstemps(p + pg - clen - i - SLEN - 1, cwd, i, SUFFIX);

	}

	return 0;
}
