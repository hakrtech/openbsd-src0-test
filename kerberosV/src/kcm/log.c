/*
 * Copyright (c) 1997, 1998, 2002 Kungliga Tekniska Högskolan
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
 * 3. Neither the name of the Institute nor the names of its contributors
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

#include "kcm_locl.h"

RCSID("$Id: log.c,v 1.2 2013/06/17 18:57:41 robert Exp $");

static krb5_log_facility *logf;

void
kcm_openlog(void)
{
    char **s = NULL, **p;
    krb5_initlog(kcm_context, "kcm", &logf);
    s = krb5_config_get_strings(kcm_context, NULL, "kcm", "logging", NULL);
    if(s == NULL)
	s = krb5_config_get_strings(kcm_context, NULL, "logging", "kcm", NULL);
    if(s){
	for(p = s; *p; p++)
	    krb5_addlog_dest(kcm_context, logf, *p);
	krb5_config_free_strings(s);
    }else
	krb5_addlog_dest(kcm_context, logf, DEFAULT_LOG_DEST);
    krb5_set_warn_dest(kcm_context, logf);
}

char*
kcm_log_msg_va(int level, const char *fmt, va_list ap)
{
    char *msg;
    krb5_vlog_msg(kcm_context, logf, &msg, level, fmt, ap);
    return msg;
}

char*
kcm_log_msg(int level, const char *fmt, ...)
{
    va_list ap;
    char *s;
    va_start(ap, fmt);
    s = kcm_log_msg_va(level, fmt, ap);
    va_end(ap);
    return s;
}

void
kcm_log(int level, const char *fmt, ...)
{
    va_list ap;
    char *s;
    va_start(ap, fmt);
    s = kcm_log_msg_va(level, fmt, ap);
    if(s) free(s);
    va_end(ap);
}
