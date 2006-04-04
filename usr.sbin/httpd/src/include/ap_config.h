/*	$OpenBSD: ap_config.h,v 1.21 2006/04/04 11:39:28 henning Exp $ */

/* ====================================================================
 * The Apache Software License, Version 1.1
 *
 * Copyright (c) 2000-2003 The Apache Software Foundation.  All rights
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:
 *       "This product includes software developed by the
 *        Apache Software Foundation (http://www.apache.org/)."
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names "Apache" and "Apache Software Foundation" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact apache@apache.org.
 *
 * 5. Products derived from this software may not be called "Apache",
 *    nor may "Apache" appear in their name, without prior written
 *    permission of the Apache Software Foundation.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE APACHE SOFTWARE FOUNDATION OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Software Foundation.  For more
 * information on the Apache Software Foundation, please see
 * <http://www.apache.org/>.
 *
 * Portions of this software are based upon public domain software
 * originally written at the National Center for Supercomputing Applications,
 * University of Illinois, Urbana-Champaign.
 */

#ifndef AP_CONFIG_H
#define AP_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * ap_config.h: system-dependant #defines and includes...
 * See PORTING for a listing of what they mean
 */

#include "ap_mmn.h"		/* MODULE_MAGIC_NUMBER_ */

/*
 * Support for platform dependent autogenerated defines
 */
#include "ap_config_auto.h"

#include <sys/types.h>
#include <sys/stat.h>


/* So that we can use inline on some critical functions, and use
 * GNUC attributes (such as to get -Wall warnings for printf-like
 * functions).  Only do this in gcc 2.7 or later ... it may work
 * on earlier stuff, but why chance it.
 */
#define ap_inline __inline__
#define USE_GNU_INLINE
#define ENUM_BITFIELD(e,n,w)  e n : w

#include "os.h"

/* Define these according to OpenBSD system. */
#define HAVE_GMTOFF 1
#undef NO_KILLPG
#undef NO_SETSID
#define HAVE_SYSLOG 1
#ifndef DEFAULT_USER
#define DEFAULT_USER "nobody"
#endif
#ifndef DEFAULT_GROUP
#define DEFAULT_GROUP "nogroup"
#endif
#define HAVE_SHMGET 1
#define HAVE_MMAP 1
#define USE_MMAP_SCOREBOARD
#define USE_MMAP_FILES
#define HAVE_FLOCK_SERIALIZED_ACCEPT
#define HAVE_SYSVSEM_SERIALIZED_ACCEPT
#define USE_SYSVSEM_SERIALIZED_ACCEPT
#include <sys/param.h>
#if (OpenBSD >= 199912)
#define NET_SIZE_T socklen_t
#endif
#define SINGLE_LISTEN_UNSERIALIZED_ACCEPT

#include <sys/param.h>

/* stuff marked API_EXPORT is part of the API, and intended for use
 * by modules
 */
#ifndef API_EXPORT
#define API_EXPORT(type)    type
#endif

/* Stuff marked API_EXPORT_NONSTD is part of the API, and intended for
 * use by modules.  The difference between API_EXPORT and
 * API_EXPORT_NONSTD is that the latter is required for any functions
 * which use varargs or are used via indirect function call.  This
 * is to accomodate the two calling conventions in windows dlls.
 */
#ifndef API_EXPORT_NONSTD
#define API_EXPORT_NONSTD(type)    type
#endif

#ifndef MODULE_VAR_EXPORT
#define MODULE_VAR_EXPORT
#endif
#ifndef API_VAR_EXPORT
#define API_VAR_EXPORT
#endif

/* modules should not used functions marked CORE_EXPORT
 * or CORE_EXPORT_NONSTD */
#ifndef CORE_EXPORT
#define CORE_EXPORT	API_EXPORT
#endif
#ifndef CORE_EXPORT_NONSTD
#define CORE_EXPORT_NONSTD	API_EXPORT_NONSTD
#endif

#define ap_private_extern

/*
 * The particular directory style your system supports. If you have dirent.h
 * in /usr/include (POSIX) or /usr/include/sys (SYSV), #include 
 * that file and define DIR_TYPE to be dirent. Otherwise, if you have 
 * /usr/include/sys/dir.h, define DIR_TYPE to be direct and include that
 * file. If you have neither, I'm confused.
 */

#include <sys/types.h>
#include <stdarg.h>

#include <dirent.h>
#define DIR_TYPE dirent

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ap_ctype.h"
#include <sys/file.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>		/* for inet_ntoa */
#include <sys/wait.h>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>
#define closesocket(s) close(s)
#ifndef O_BINARY
#define O_BINARY (0)
#endif

#include <limits.h>
#include <time.h>		/* for ctime */
#include <signal.h>
#include <errno.h>
#include <memory.h>

#include <regex.h>

#include <sys/resource.h>
#include <sys/mman.h>

/* A USE_FOO_SERIALIZED_ACCEPT implies a HAVE_FOO_SERIALIZED_ACCEPT */
#if defined(USE_SYSVSEM_SERIALIZED_ACCEPT) && !defined(HAVE_SYSVSEM_SERIALIZED_ACCEPT)
#define HAVE_SYSVSEM_SERIALIZED_ACCEPT
#endif
#if defined(USE_FLOCK_SERIALIZED_ACCEPT) && !defined(HAVE_FLOCK_SERIALIZED_ACCEPT)
#define HAVE_FLOCK_SERIALIZED_ACCEPT
#endif

#ifndef LOGNAME_MAX
#define LOGNAME_MAX 25
#endif

#include <unistd.h>

#ifndef S_ISLNK
#define S_ISLNK(m) (((m) & S_IFMT) == S_IFLNK)
#endif

#ifndef INADDR_NONE
#define INADDR_NONE ((unsigned long) -1)
#endif

/*
 * Replace signal function with sigaction equivalent
 */
typedef void Sigfunc(int);

#if defined(SIG_IGN) && !defined(SIG_ERR)
#define SIG_ERR ((Sigfunc *)-1)
#endif

/*
 * For some strange reason, QNX defines signal to signal. Eliminate it.
 */
#ifdef signal
#undef signal
#endif
#define signal(s,f)	ap_signal(s,f)
Sigfunc *signal(int signo, Sigfunc * func);

#include <setjmp.h>

#if defined(USE_LONGJMP)
#define ap_longjmp(x, y)        longjmp((x), (y))
#define ap_setjmp(x)            setjmp(x)
#ifndef JMP_BUF
#define JMP_BUF jmp_buf
#endif
#else
#define ap_longjmp(x, y)        siglongjmp((x), (y))
#define ap_setjmp(x)            sigsetjmp((x), 1)
#ifndef JMP_BUF
#define JMP_BUF sigjmp_buf
#endif
#endif

/* Majority of os's want to verify FD_SETSIZE */
#define CHECK_FD_SETSIZE

#if defined(SELECT_NEEDS_CAST)
#define ap_select(_a, _b, _c, _d, _e)   \
    select((_a), (int *)(_b), (int *)(_c), (int *)(_d), (_e))
#else
#define ap_select(_a, _b, _c, _d, _e)   \
	select(_a, _b, _c, _d, _e)
#endif

#define ap_accept(_fd, _sa, _ln)	accept(_fd, _sa, _ln)

#define ap_check_signals()

#define ap_fdopen(d,m) fdopen((d), (m))

#ifndef ap_inet_addr
#define ap_inet_addr inet_addr
#endif

/* Finding offsets of elements within structures.
 * Taken from the X code... they've sweated portability of this stuff
 * so we don't have to.  Sigh...
 */

#if defined(__arm)
#ifdef __STDC__
#define XtOffset(p_type,field) _Offsetof(p_type,field)
#else
#define XtOffset(p_type,field) ((unsigned int)&(((p_type)NULL)->field))
#endif /* __STDC__ */
#else /* ! (__arm) */

#define XtOffset(p_type,field) \
	((long) (((char *) (&(((p_type)NULL)->field))) - ((char *) NULL)))

#endif /* __arm */

#ifdef offsetof
#define XtOffsetOf(s_type,field) offsetof(s_type,field)
#else
#define XtOffsetOf(s_type,field) XtOffset(s_type*,field)
#endif

/*
 * NET_SIZE_T exists because of shortsightedness on the POSIX committee.  BSD
 * systems used "int *" as the parameter to accept(), getsockname(),
 * getpeername() et al.  Consequently many unixes took an int * for that
 * parameter.  The POSIX committee decided that "int" was just too generic and
 * had to be replaced with size_t almost everywhere.  There's no problem with
 * that when you're passing by value.  But when you're passing by reference
 * this creates a gross source incompatibility with existing programs.  On
 * 32-bit architectures it creates only a warning.  On 64-bit architectures it
 * creates broken code -- because "int *" is a pointer to a 64-bit quantity and
 * "size_t *" is frequently a pointer to a 32-bit quantity.
 *
 * Some Unixes adopted "size_t *" for the sake of POSIX compliance.  Others
 * ignored it because it was such a broken interface.  Chaos ensued.  POSIX
 * finally woke up and decided that it was wrong and created a new type
 * socklen_t.  The only useful value for socklen_t is int, and that's how
 * everyone who has a clue implements it.  It is almost always the case that
 * NET_SIZE_T should be defined to be an int, unless the system being compiled
 * for was created in the window of POSIX madness.
 */
#ifndef NET_SIZE_T
#define NET_SIZE_T int
#endif

/* The assumption is that when the functions are missing,
 * then there's no matching prototype available either.
 * Declare what is needed exactly as the replacement routines implement it.
 */

#ifndef ap_wait_t
#define ap_wait_t int
#endif

#ifdef __cplusplus
}
#endif

#endif /* !AP_CONFIG_H */
