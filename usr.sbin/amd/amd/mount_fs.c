/*
 * Copyright (c) 1990 Jan-Simon Pendry
 * Copyright (c) 1990 Imperial College of Science, Technology & Medicine
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Jan-Simon Pendry at Imperial College, London.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	from: @(#)mount_fs.c	8.1 (Berkeley) 6/6/93
 *	$Id: mount_fs.c,v 1.13 2014/10/20 02:33:42 guenther Exp $
 */

#include "am.h"
#ifdef NFS_3
typedef nfs_fh fhandle_t;
#endif /* NFS_3 */

#include <unistd.h>
#include <sys/stat.h>

/*
 * Standard mount flags
 */

struct opt_tab mnt_flags[] = {
	{ "ro", M_RDONLY },
#ifdef M_CACHE
	{ "nocache", M_NOCACHE },
#endif /* M_CACHE */
#ifdef M_GRPID
	{ "grpid", M_GRPID },
#endif /* M_GRPID */
#ifdef M_MULTI
	{ "multi", M_MULTI },
#endif /* M_MULTI */
#ifdef M_NODEV
	{ "nodev", M_NODEV },
#endif /* M_NODEV */
#ifdef M_NOEXEC
	{ "noexec", M_NOEXEC },
#endif /* M_NOEXEC */
#ifdef M_NOSUB
	{ "nosub", M_NOSUB },
#endif /* M_NOSUB */
#ifdef M_NOSUID
	{ "nosuid", M_NOSUID },
#endif /* M_NOSUID */
#ifdef M_SYNC
	{ "sync", M_SYNC },
#endif /* M_SYNC */
	{ 0, 0 }
};

int
compute_mount_flags(struct mntent *mnt)
{
	struct opt_tab *opt;
	int flags;
#ifdef NFS_4
	flags = M_NEWTYPE;
#else
	flags = 0;
#endif /* NFS_4 */

	/*
	 * Crack basic mount options
	 */
	for (opt = mnt_flags; opt->opt; opt++)
		flags |= hasmntopt(mnt, opt->opt) ? opt->flag : 0;

	return flags;
}

int
mount_fs(struct mntent *mnt, int flags, caddr_t mnt_data, int retry,
    MTYPE_TYPE type)
{
	int error = 0;

#ifdef DEBUG
#ifdef NFS_4
	dlog("%s fstype %s (%s) flags %#x (%s)",
		mnt->mnt_dir, type, mnt->mnt_type, flags, mnt->mnt_opts);
#else
	dlog("%s fstype %d (%s) flags %#x (%s)",
		mnt->mnt_dir, type, mnt->mnt_type, flags, mnt->mnt_opts);
#endif /* NFS_4 */
#endif /* DEBUG */

	/*
	 * Fake some mount table entries for the automounter
	 */
#ifdef FASCIST_DF_COMMAND
	/*
	 * Some systems have a df command which blows up when
	 * presented with an unknown mount type.
	 */
	if (STREQ(mnt->mnt_type, MNTTYPE_AUTO)) {
		/*
		 * Try it with the normal name
		 */
		mnt->mnt_type = FASCIST_DF_COMMAND;
	}
#endif /* FASCIST_DF_COMMAND */

again:
	clock_valid = 0;
	error = MOUNT_TRAP(type, mnt, flags, mnt_data);
	if (error < 0)
		plog(XLOG_ERROR, "%s: mount: %m", mnt->mnt_dir);
	if (error < 0 && --retry > 0) {
		sleep(1);
		goto again;
	}
	if (error < 0) {
#ifdef notdef
		if (automount)
			going_down(errno);
#endif
		return errno;
	}


	return 0;
}

/*
 * Some systems don't provide these to the user,
 * but amd needs them, so...
 *
 * From: Piete Brooks <pb@cl.cam.ac.uk>
 */

#include <ctype.h>

static char *
nextmntopt(char **p)
{
	char *cp = *p;
	char *rp;
	/*
	 * Skip past white space
	 */
	while (isspace((unsigned char)*cp))
		cp++;
	/*
	 * Word starts here
	 */
	rp = cp;
	/*
	 * Scan to send of string or separator
	 */
	while (*cp && *cp != ',')
		cp++;
	/*
	 * If separator found the overwrite with nul char.
	 */
	if (*cp) {
		*cp = '\0';
		cp++;
	}
	/*
	 * Return value for next call
	 */
	*p = cp;
	return rp;
}

char *
hasmntopt(struct mntent *mnt, char *opt)
{
	char t[MNTMAXSTR];
	char *f;
	char *o = t;
	int l = strlen(opt);

	strlcpy(t, mnt->mnt_opts, sizeof(t));

	while (*(f = nextmntopt(&o)))
		if (strncmp(opt, f, l) == 0)
			return f - t + mnt->mnt_opts;

	return 0;
}

#ifdef MOUNT_HELPER_SOURCE
#include MOUNT_HELPER_SOURCE
#endif /* MOUNT_HELPER_SOURCE */
