/*	$OpenBSD: bill.c,v 1.4 1998/10/01 05:31:38 pjanzen Exp $	*/
/*	$NetBSD: bill.c,v 1.5 1997/10/18 20:03:06 christos Exp $	 */

/*-
 * Copyright (c) 1991 The Regents of the University of California.
 * All rights reserved.
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
static char sccsid[] = "@(#)bill.c	5.2 (Berkeley) 5/28/91";
#else
static char rcsid[] = "$OpenBSD: bill.c,v 1.4 1998/10/01 05:31:38 pjanzen Exp $";
#endif
#endif /* not lint */

#include <sys/file.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "header.h"
#include "extern.h"
#include "pathnames.h"

/* bill.c		 Larn is copyrighted 1986 by Noah Morgan. */

#ifndef NOSPAM
char *mail[] = {
	"Undeclared Income",
	"From: The LRS (Larn Revenue Service)\n",
	"\n   We have heard you survived the caverns of Larn.  Let us be the",
	"\nfirst to congratulate you on your success.  It was quite a feat.",
	"\nIt was also very profitable for you...",
	"\n\n   The Dungeon Master has informed us that you brought",
	"1",
	"\ncounty of Larn is in dire need of funds, we have wasted no time",
	"2",
	"\nof this notice, due within 5 days.  Failure to pay will",
	"\nmean penalties.  Once again, congratulations.  We look forward",
	"\nto your future successful expeditions.\n",
	NULL,
	"A Noble Deed",
	"From: His Majesty King Wilfred of Larndom\n",
	"\n   I have heard of your magnificent feat, and I, King Wilfred,",
	"\nforthwith declare today to be a national holiday.  Furthermore,",
	"\nthree days hence be ye invited to the castle to receive the",
	"\nhonour of Knight of the Realm.  Upon thy name shall it be written...",
	"\n\nBravery and courage be yours.",
	"\n\nMay you live in happiness forevermore...\n",
	NULL,
	"You Bastard!",
	"From: Count Endelford\n",
	"\n   I have heard (from sources) of your journey.  Congratulations!",
	"\nYou Bastard!  With several attempts I have yet to endure the",
	" caves,\nand you, a nobody, make the journey!  From this time",
	" onward, bewarned -- \nupon our meeting you shall pay the price!\n",
	NULL,
	"High Praise",
	"From: Mainair, Duke of Larnty\n",
	"\n   With certainty, a hero I declare to be amongst us!  A nod of",
	"\nfavour I send to thee.  Me thinks Count Endelford this day of",
	"\nright breath'eth fire as of dragon of whom ye are slayer.  I",
	"\nyearn to behold his anger and jealousy.  Should ye choose to",
	"\nunleash some of thy wealth upon those who be unfortunate, I,",
	"\nDuke Mainair, shall equal thy gift also.\n",
	NULL,
	"these poor children",
	"From: St. Mary's Children's Home\n",
	"\n   News of your great conquests has spread to all of Larndom.",
	"\nMight I have a moment of a great adventurer's time?  We here at",
	"\nSt. Mary's Children's Home are very poor, and many children are",
	"\nstarving.  Disease is widespread and very often fatal without",
	"\ngood food.  Could you possibly find it in your heart to help us",
	"\nin our plight?  Whatever you could give will help much.",
	"\n(your gift is tax deductible)\n",
	NULL,
	"hope",
	"From: The National Cancer Society of Larn\n",
	"\nCongratulations on your successful expedition.  We are sure much",
	"\ncourage and determination were needed on your quest.  There are",
	"\nmany though, that could never hope to undertake such a journey",
	"\ndue to an enfeebling disease -- cancer.  We at the National",
	"\nCancer Society of Larn wish to appeal to your philanthropy in",
	"\norder to save many good people -- possibly even yourself a few",
	"\nyears from now.  Much work needs to be done in researching this",
	"\ndreaded disease, and you can help today.  Could you please see it",
	"\nin your heart to give generously?  Your continued good health",
	"\ncan be your everlasting reward.\n",
	NULL,
	NULL
};
#endif

/*
 * function to mail the letters to the player if a winner
 */

void
mailbill()
{
#ifndef NOSPAM
	int	i;
	char	buf[128];
	char	**cp;
	int	fd[2];
	pid_t	pid;

	/* This is the last thing that gets run.  We don't care if it
	 * fails, so exit on any failure.  Drop privs. */
	setegid(gid);
	setgid(gid);
	wait(0);
	resetscroll();
	cp = mail;
	for (i = 0; i < 6; i++) {
		if (pipe(fd) < 0)
			exit(0);
		if ((pid = fork()) < 0)
			exit(0);
		else if (pid > 0) { /* parent */
			close(fd[0]);
			while (*++cp != NULL) {
				if (*cp[0] == '1')
					sprintf(buf, "\n%ld gold pieces back with you from your journey.  As the",
						(long) c[GOLD]);
				else if (*cp[0] == '2')
					sprintf(buf, "\nin preparing your tax bill.  You owe %ld gold pieces as", (long) c[GOLD] * TAXRATE);
				else
					sprintf(buf, *cp);
				if (write(fd[1], buf, strlen(buf)) != strlen(buf))
					exit(0);
			}
			cp++;
			close(fd[1]);
			if (waitpid(pid, NULL, 0) < 0)
				exit(0);
		} else {	/* child */
			close(fd[1]);
			if (fd[0] != STDIN_FILENO) {
				if (dup2(fd[0], STDIN_FILENO) != STDIN_FILENO)
					exit(0);
				close(fd[0]);
			}
			if (execl(_PATH_MAIL, "mail", "-s", *cp, loginname,
			    (char *) NULL) < 0)
				exit(0);
		}
	}
#endif
	exit(0);
}
