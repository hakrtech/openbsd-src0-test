/*	$OpenBSD: crt0.c,v 1.8 2016/09/26 15:43:26 kettenis Exp $	*/

/*
 * Copyright (c) 1995 Christopher G. Demetriou
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
 *      This product includes software developed by Christopher G. Demetriou
 *	for the NetBSD Project.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
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

#include <stdlib.h>
#include <limits.h>

#include "md_init.h"
#ifdef MD_RCRT0_START
#include "boot.h"
#endif

/* some defaults */
#ifndef	MD_START_ARGS
#define	MD_START_ARGS	\
	int argc, char **argv, char **envp, void (*cleanup)(void)
#endif
#ifndef MD_START
#define	MD_START	___start
static void		___start(MD_START_ARGS) __used;
#endif
#ifndef	MD_EPROL_LABEL
#define	MD_EPROL_LABEL	__asm("  .text\n_eprol:")
#endif

char	***_csu_finish(char **_argv, char **_envp, void (*_cleanup)(void));

#ifdef MCRT0
#include <sys/gmon.h>
extern unsigned char _etext, _eprol;
#endif /* MCRT0 */

#ifdef RCRT0
#ifdef MD_RCRT0_START
MD_RCRT0_START;
#endif
#else
#ifdef MD_CRT0_START
MD_CRT0_START;
#endif
#endif

void
MD_START(MD_START_ARGS)
{
	char ***environp;
#ifdef MD_START_SETUP
	MD_START_SETUP
#endif

	environp = _csu_finish(argv, envp, cleanup);

#ifdef MCRT0
	atexit(_mcleanup);
	_monstartup((u_long)&_eprol, (u_long)&_etext);
#endif

	__init();

	exit(main(argc, argv, *environp));
}

#ifdef MCRT0
MD_EPROL_LABEL;
#endif
