/*	$OpenBSD: rmjob.c,v 1.13 2002/05/20 23:13:50 millert Exp $	*/
/*	$NetBSD: rmjob.c,v 1.16 2000/04/16 14:43:58 mrg Exp $	*/

/*
 * Copyright (c) 1983, 1993
 *	The Regents of the University of California.  All rights reserved.
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
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
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
 */

#ifndef lint
#if 0
static const char sccsid[] = "@(#)rmjob.c	8.2 (Berkeley) 4/28/95";
#else
static const char rcsid[] = "$OpenBSD: rmjob.c,v 1.13 2002/05/20 23:13:50 millert Exp $";
#endif
#endif /* not lint */

#include <sys/param.h>

#include <signal.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lp.h"
#include "lp.local.h"
#include "pathnames.h"

/*
 * rmjob - remove the specified jobs from the queue.
 */

/*
 * Stuff for handling lprm specifications
 */
extern char	*user[];		/* users to process */
extern int	users;			/* # of users in user array */
extern int	requ[];			/* job number of spool entries */
extern int	requests;		/* # of spool requests */
extern char	*person;		/* name of person doing lprm */

static char	root[] = "root";
static int	all = 0;		/* eliminate all files (root only) */
static int	cur_daemon;		/* daemon's pid */
static char	current[NAME_MAX];	/* active control file name */

static	void	do_unlink(char *);
static	void	alarmer(int);

void
rmjob(void)
{
	int i, nitems;
	int assasinated = 0;
	struct dirent **files;
	char *cp;

	if ((i = cgetent(&bp, printcapdb, printer)) == -2)
		fatal("can't open printer description file");
	else if (i == -1)
		fatal("unknown printer");
	else if (i == -3)
		fatal("potential reference loop detected in printcap file");
	if (cgetstr(bp, DEFLP, &LP) < 0)
		LP = _PATH_DEFDEVLP;
	if (cgetstr(bp, "rp", &RP) < 0)
		RP = DEFLP;
	if (cgetstr(bp, "sd", &SD) < 0)
		SD = _PATH_DEFSPOOL;
	if (cgetstr(bp,"lo", &LO) < 0)
		LO = DEFLOCK;
	cgetstr(bp, "rm", &RM);
	if ((cp = checkremote()) != NULL)
		printf("Warning: %s\n", cp);

	/*
	 * If the format was `lprm -' and the user isn't the super-user,
	 *  then fake things to look like he said `lprm user'.
	 */
	if (users < 0) {
		if (getuid() == 0)
			all = 1;	/* all files in local queue */
		else {
			user[0] = person;
			users = 1;
		}
	}
	if (!strcmp(person, "-all")) {
		if (from == host)
			fatal("The login name \"-all\" is reserved");
		all = 1;	/* all those from 'from' */
		person = root;
	}

	seteuid(euid);
	if (chdir(SD) < 0)
		fatal("cannot chdir to spool directory");
	if ((nitems = scandir(".", &files, iscf, NULL)) < 0)
		fatal("cannot access spool directory");
	seteuid(uid);

	if (nitems) {
		/*
		 * Check for an active printer daemon (in which case we
		 *  kill it if it is reading our file) then remove stuff
		 *  (after which we have to restart the daemon).
		 */
		if (lockchk(LO) && chk(current)) {
			seteuid(euid);
			assasinated = kill(cur_daemon, SIGINT) == 0;
			seteuid(uid);
			if (!assasinated)
				fatal("cannot kill printer daemon");
		}
		/*
		 * process the files
		 */
		for (i = 0; i < nitems; i++)
			process(files[i]->d_name);
	}
	rmremote();
	/*
	 * Restart the printer daemon if it was killed
	 */
	if (assasinated && !startdaemon(printer))
		fatal("cannot restart printer daemon\n");
	exit(0);
}

/*
 * Process a lock file: collect the pid of the active
 *  daemon and the file name of the active spool entry.
 * Return boolean indicating existence of a lock file.
 */
int
lockchk(char *s)
{
	FILE *fp;
	int i, n;

	seteuid(euid);
	if ((fp = fopen(s, "r")) == NULL) {
		if (errno == EACCES)
			fatal("can't access lock file");
		else
			return(0);
	}
	seteuid(uid);
	if (!getline(fp)) {
		(void)fclose(fp);
		return(0);		/* no daemon present */
	}
	cur_daemon = atoi(line);
	if (kill(cur_daemon, 0) < 0 && errno != EPERM) {
		(void)fclose(fp);
		return(0);		/* no daemon present */
	}
	for (i = 1; (n = fread(current, sizeof(char), sizeof(current), fp)) <= 0; i++) {
		if (i > 5) {
			n = 1;
			break;
		}
		sleep(i);
	}
	current[n-1] = '\0';
	(void)fclose(fp);
	return(1);
}

/*
 * Process a control file.
 */
void
process(char *file)
{
	FILE *cfp;

	if (!chk(file))
		return;
	seteuid(euid);
	if ((cfp = fopen(file, "r")) == NULL)
		fatal("cannot open %s", file);
	seteuid(uid);
	while (getline(cfp)) {
		switch (line[0]) {
		case 'U':  /* unlink associated files */
			if (strchr(line+1, '/') || strncmp(line+1, "df", 2))
				break;
			do_unlink(line+1);
		}
	}
	(void)fclose(cfp);
	do_unlink(file);
}

static void
do_unlink(char *file)
{
	int	ret;

	if (from != host)
		printf("%s: ", host);
	seteuid(euid);
	ret = unlink(file);
	seteuid(uid);
	printf(ret ? "cannot dequeue %s\n" : "%s dequeued\n", file);
}

/*
 * Do the dirty work in checking
 */
int
chk(char *file)
{
	int *r, n;
	char **u, *cp;
	FILE *cfp;

	/*
	 * Check for valid cf file name (mostly checking current).
	 */
	if (strlen(file) < 7 || file[0] != 'c' || file[1] != 'f')
		return(0);

	if (all && (from == host || !strcmp(from, file+6)))
		return(1);

	/*
	 * get the owner's name from the control file.
	 */
	seteuid(euid);
	if ((cfp = fopen(file, "r")) == NULL)
		return(0);
	seteuid(uid);
	while (getline(cfp)) {
		if (line[0] == 'P')
			break;
	}
	(void)fclose(cfp);
	if (line[0] != 'P')
		return(0);

	if (users == 0 && requests == 0)
		return(!strcmp(file, current) && isowner(line+1, file));
	/*
	 * Check the request list
	 */
	for (n = 0, cp = file+3; isdigit(*cp); )
		n = n * 10 + (*cp++ - '0');
	for (r = requ; r < &requ[requests]; r++)
		if (*r == n && isowner(line+1, file))
			return(1);
	/*
	 * Check to see if it's in the user list
	 */
	for (u = user; u < &user[users]; u++)
		if (!strcmp(*u, line+1) && isowner(line+1, file))
			return(1);
	return(0);
}

/*
 * If root is removing a file on the local machine, allow it.
 * If root is removing a file from a remote machine, only allow
 * files sent from the remote machine to be removed.
 * Normal users can only remove the file from where it was sent.
 */
int
isowner(char *owner, char *file)
{
	if (!strcmp(person, root) && (from == host || !strcmp(from, file+6)))
		return(1);
	if (!strcmp(person, owner) && !strcmp(from, file+6))
		return(1);
	if (from != host)
		printf("%s: ", host);
	printf("%s: Permission denied\n", file);
	return(0);
}

/*
 * Check to see if we are sending files to a remote machine. If we are,
 * then try removing files on the remote machine.
 */
void
rmremote(void)
{
	char *cp;
	int i, rem;
	char buf[BUFSIZ];

	if (!remote)
		return;	/* not sending to a remote machine */

	/*
	 * Flush stdout so the user can see what has been deleted
	 * while we wait (possibly) for the connection.
	 */
	fflush(stdout);

	(void)snprintf(buf, sizeof(buf)-2, "\5%s %s", RP, all ? "-all" : person);
	cp = buf + strlen(buf);
	for (i = 0; i < users && cp-buf+1+strlen(user[i]) < sizeof buf - 2; i++) {
		cp += strlen(cp);
		*cp++ = ' ';
		strcpy(cp, user[i]);
	}
	for (i = 0; i < requests && cp-buf+10 < sizeof(buf) - 2; i++) {
		cp += strlen(cp);
		(void)sprintf(cp, " %d", requ[i]);
	}
	strcat(cp, "\n");
	rem = getport(RM, 0);
	if (rem < 0) {
		if (from != host)
			printf("%s: ", host);
		printf("connection to %s is down\n", RM);
	} else {
		struct sigaction osa, nsa;

		memset(&nsa, 0, sizeof(nsa));
		nsa.sa_handler = alarmer;
		sigemptyset(&nsa.sa_mask);
		nsa.sa_flags = 0;
		(void)sigaction(SIGALRM, &nsa, &osa);
		alarm(wait_time);

		i = strlen(buf);
		if (write(rem, buf, i) != i)
			fatal("Lost connection");
		while ((i = read(rem, buf, sizeof(buf))) > 0)
			(void)fwrite(buf, 1, i, stdout);
		alarm(0);
		(void)sigaction(SIGALRM, &osa, NULL);
		(void)close(rem);
	}
}

static void
alarmer(int s)
{
	/* nothing */
}

/*
 * Return 1 if the filename begins with 'cf'
 */
int
iscf(struct dirent *d)
{
	return(d->d_name[0] == 'c' && d->d_name[1] == 'f');
}
