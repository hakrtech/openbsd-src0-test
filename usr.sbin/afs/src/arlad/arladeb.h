/*	$OpenBSD: arladeb.h,v 1.1.1.1 1998/09/14 21:52:55 art Exp $	*/
/*
 * Copyright (c) 1995, 1996, 1997, 1998 Kungliga Tekniska H�gskolan
 * (Royal Institute of Technology, Stockholm, Sweden).
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by the Kungliga Tekniska
 *      H�gskolan and its contributors.
 * 
 * 4. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * $KTH: arladeb.h,v 1.16 1998/05/23 05:25:47 assar Exp $
 */

#ifndef _arladeb_h
#define _arladeb_h

#include <stdio.h>
#include <stdarg.h>

#include <roken.h>

/* masks */
#define ADEBANY		0xffffffff
#define ADEBMISC        0x00000001	/* misc debugging */
#define ADEBCONN        0x00000002	/* conncache */
#define ADEBINIT        0x00000004	/* initialization debug */
#define ADEBFCACHE	0x00000008	/* file cache */
#define ADEBVOLCACHE	0x00000010	/* volume cache */
#define ADEBCM		0x00000020      /* cache manager */
#define ADEBCALLBACK	0x00000040      /* callbacks */
#define ADEBCLEANER	0x00000080      /* cleaner */
#define ADEBKERNEL	0x00000100      /* kernel interface */
#define ADEBMSG		0x00000200	/* messages */
#define ADEBFBUF	0x00000400	/* fbuf */
#define ADEBWARN	0x08000000      /* don't ignore warning */
#define ADEBERROR	0x10000000      /* don't ignore error */

void arla_log(unsigned level, char *fmt, ...);
void arla_loginit(char *log);
int arla_log_set_level (const char *s);
void arla_log_get_level (char *s, size_t len);
void arla_log_print_levels (FILE *f);

void
arla_err (int eval, unsigned level, int error, const char *fmt, ...)
__attribute__ ((noreturn))
__attribute__ ((format (printf, 4, 5)))
;

void
arla_verr (int eval, unsigned level, int error, const char *fmt, va_list args)
__attribute__ ((noreturn))
__attribute__ ((format (printf, 4, 0)))
;

void
arla_errx (int eval, unsigned level, const char *fmt, ...)
__attribute__ ((noreturn))
__attribute__ ((format (printf, 3, 4)))
;

void
arla_verrx (int eval, unsigned level, const char *fmt, va_list args)
__attribute__ ((noreturn))
__attribute__ ((format (printf, 3, 0)))
;

void
arla_warn (unsigned level, int error, const char *fmt, ...)
__attribute__ ((format (printf, 3, 4)))
;

void
arla_vwarn (unsigned level, int error, const char *fmt, va_list args)
__attribute__ ((format (printf, 3, 0)))
;

void
arla_warnx (unsigned level, const char *fmt, ...)
__attribute__ ((format (printf, 2, 3)))
;

void
arla_vwarnx (unsigned level, const char *fmt, va_list args)
__attribute__ ((format (printf, 2, 0)))
;

#endif				       /* _arladeb_h */
