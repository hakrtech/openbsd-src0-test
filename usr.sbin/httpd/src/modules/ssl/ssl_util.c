/*                      _             _
**  _ __ ___   ___   __| |    ___ ___| |  mod_ssl
** | '_ ` _ \ / _ \ / _` |   / __/ __| |  Apache Interface to OpenSSL
** | | | | | | (_) | (_| |   \__ \__ \ |  www.modssl.org
** |_| |_| |_|\___/ \__,_|___|___/___/_|  ftp.modssl.org
**                      |_____|
**  ssl_util.c
**  Utility Functions
*/

/* ====================================================================
 * Copyright (c) 1998-2003 Ralf S. Engelschall. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by
 *     Ralf S. Engelschall <rse@engelschall.com> for use in the
 *     mod_ssl project (http://www.modssl.org/)."
 *
 * 4. The names "mod_ssl" must not be used to endorse or promote
 *    products derived from this software without prior written
 *    permission. For written permission, please contact
 *    rse@engelschall.com.
 *
 * 5. Products derived from this software may not be called "mod_ssl"
 *    nor may "mod_ssl" appear in their names without prior
 *    written permission of Ralf S. Engelschall.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by
 *     Ralf S. Engelschall <rse@engelschall.com> for use in the
 *     mod_ssl project (http://www.modssl.org/)."
 *
 * THIS SOFTWARE IS PROVIDED BY RALF S. ENGELSCHALL ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL RALF S. ENGELSCHALL OR
 * HIS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 */

/* ====================================================================
 * Copyright (c) 1995-1999 Ben Laurie. All rights reserved.
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
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by Ben Laurie
 *    for use in the Apache-SSL HTTP server project."
 *
 * 4. The name "Apache-SSL Server" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission.
 *
 * 5. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by Ben Laurie
 *    for use in the Apache-SSL HTTP server project."
 *
 * THIS SOFTWARE IS PROVIDED BY BEN LAURIE ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL BEN LAURIE OR
 * HIS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 */
                             /* ``Every day of my life
                                  I am forced to add another
                                  name to the list of people
                                  who piss me off!''
                                            -- Calvin          */
#include "mod_ssl.h"


/*  _________________________________________________________________
**
**  Utility Functions
**  _________________________________________________________________
*/

char *ssl_util_server_root_relative(pool *p, char *what, char *arg)
{
    char *rv = NULL;

#ifdef SSL_VENDOR
    ap_hook_use("ap::mod_ssl::vendor::ssl_server_root_relative",
                AP_HOOK_SIG4(ptr,ptr,ptr,ptr), AP_HOOK_ALL, &rv, p, what, arg);
    if (rv != NULL)
        return rv;
#endif
    rv = ap_server_root_relative(p, arg);
    return rv;
}

char *ssl_util_vhostid(pool *p, server_rec *s)
{
    char *id;
    SSLSrvConfigRec *sc;
    char *host;
    unsigned int port;

    host = s->server_hostname;
    if (s->port != 0)
        port = s->port;
    else {
        sc = mySrvConfig(s);
        if (sc->bEnabled)
            port = DEFAULT_HTTPS_PORT;
        else
            port = DEFAULT_HTTP_PORT;
    }
    id = ap_psprintf(p, "%s:%u", host, port);
    return id;
}

FILE *ssl_util_ppopen(server_rec *s, pool *p, char *cmd)
{
    FILE *fpout;
    int rc;

    fpout = NULL;
    rc = ap_spawn_child(p, ssl_util_ppopen_child,
                        (void *)cmd, kill_after_timeout,
                        NULL, &fpout, NULL);
    if (rc == 0 || fpout == NULL) {
        ap_log_error(APLOG_MARK, APLOG_ERR, s,
                     "ssl_util_ppopen: could not run: %s", cmd);
        return NULL;
    }
    return (fpout);
}

int ssl_util_ppopen_child(void *cmd, child_info *pinfo)
{
    int child_pid = 1;

    /*
     * Prepare for exec
     */
    ap_cleanup_for_exec();
    signal(SIGHUP, SIG_IGN);

    /*
     * Exec() the child program
     */
    /* Standard Unix */
    execl(SHELL_PATH, SHELL_PATH, "-c", (char *)cmd, (char *)NULL);
    return (child_pid);
}

void ssl_util_ppclose(server_rec *s, pool *p, FILE *fp)
{
    ap_pfclose(p, fp);
    return;
}

/*
 * Run a filter program and read the first line of its stdout output
 */
char *ssl_util_readfilter(server_rec *s, pool *p, char *cmd)
{
    static char buf[MAX_STRING_LEN];
    FILE *fp;
    char c;
    int k;

    if ((fp = ssl_util_ppopen(s, p, cmd)) == NULL)
        return NULL;
    for (k = 0;    read(fileno(fp), &c, 1) == 1
                && (k < MAX_STRING_LEN-1)       ; ) {
        if (c == '\n' || c == '\r')
            break;
        buf[k++] = c;
    }
    buf[k] = NUL;
    ssl_util_ppclose(s, p, fp);

    return buf;
}

BOOL ssl_util_path_check(ssl_pathcheck_t pcm, char *path)
{
    struct stat sb;

    if (path == NULL)
        return FALSE;
    if (pcm & SSL_PCM_EXISTS && stat(path, &sb) != 0)
        return FALSE;
    if (pcm & SSL_PCM_ISREG && !S_ISREG(sb.st_mode))
        return FALSE;
    if (pcm & SSL_PCM_ISDIR && !S_ISDIR(sb.st_mode))
        return FALSE;
    if (pcm & SSL_PCM_ISNONZERO && sb.st_mode <= 0)
        return FALSE;
    return TRUE;
}

ssl_algo_t ssl_util_algotypeof(X509 *pCert, EVP_PKEY *pKey) 
{
    ssl_algo_t t;
            
    t = SSL_ALGO_UNKNOWN;
    if (pCert != NULL)
        pKey = X509_get_pubkey(pCert);
    if (pKey != NULL) {
        switch (EVP_PKEY_type(pKey->type)) {
            case EVP_PKEY_RSA: 
                t = SSL_ALGO_RSA;
                break;
            case EVP_PKEY_DSA: 
                t = SSL_ALGO_DSA;
                break;
            default:
                break;
        }
    }
    return t;
}

char *ssl_util_algotypestr(ssl_algo_t t) 
{
    char *cp;

    cp = "UNKNOWN";
    switch (t) {
        case SSL_ALGO_RSA: 
            cp = "RSA";
            break;
        case SSL_ALGO_DSA: 
            cp = "DSA";
            break;
        default:
            break;
    }
    return cp;
}

char *ssl_util_ptxtsub(
    pool *p, const char *cpLine, const char *cpMatch, char *cpSubst)
{
#define MAX_PTXTSUB 100
    char *cppMatch[MAX_PTXTSUB + 1];
    char *cpResult;
    int nResult;
    int nLine;
    int nSubst;
    int nMatch;
    char *cpI;
    char *cpO;
    char *cp;
    int i;

    /*
     * Pass 1: find substitution locations and calculate sizes
     */
    nLine  = strlen(cpLine);
    nMatch = strlen(cpMatch);
    nSubst = strlen(cpSubst);
    for (cpI = (char *)cpLine, i = 0, nResult = 0;
         cpI < cpLine+nLine && i < MAX_PTXTSUB;    ) {
        if ((cp = strstr(cpI, cpMatch)) != NULL) {
            cppMatch[i++] = cp;
            nResult += ((cp-cpI)+nSubst);
            cpI = (cp+nMatch);
        }
        else {
            nResult += strlen(cpI);
            break;
        }
    }
    cppMatch[i] = NULL;
    if (i == 0)
        return NULL;

    /*
     * Pass 2: allocate memory and assemble result
     */
    cpResult = ap_pcalloc(p, nResult+1);
    for (cpI = (char *)cpLine, cpO = cpResult, i = 0; cppMatch[i] != NULL; i++) {
        ap_cpystrn(cpO, cpI, cppMatch[i]-cpI+1);
        cpO += (cppMatch[i]-cpI);
        ap_cpystrn(cpO, cpSubst, nSubst+1);
        cpO += nSubst;
        cpI = (cppMatch[i]+nMatch);
    }
    ap_cpystrn(cpO, cpI, cpResult+nResult-cpO+1);

    return cpResult;
}

/*  _________________________________________________________________
**
**  Special Functions for Win32/OpenSSL
**  _________________________________________________________________
*/

void ssl_util_thread_setup(void)
{
    return;
}

void ssl_util_thread_cleanup(void)
{
    return;
}

