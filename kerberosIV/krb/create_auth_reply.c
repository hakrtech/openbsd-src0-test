/* $KTH: create_auth_reply.c,v 1.11 1997/04/01 08:18:20 joda Exp $ */

/*
 * Copyright (c) 1995, 1996, 1997 Kungliga Tekniska H�gskolan
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

#include "krb_locl.h"

/*
 * This routine is called by the Kerberos authentication server
 * to create a reply to an authentication request.  The routine
 * takes the user's name, instance, and realm, the client's
 * timestamp, the number of tickets, the user's key version
 * number and the ciphertext containing the tickets themselves.
 * It constructs a packet and returns a pointer to it.
 *
 * Notes: The packet returned by this routine is static.  Thus, if you
 * intend to keep the result beyond the next call to this routine, you
 * must copy it elsewhere.
 *
 * The packet is built in the following format:
 * 
 * 			variable
 * type			or constant	   data
 * ----			-----------	   ----
 * 
 * unsigned char	KRB_PROT_VERSION   protocol version number
 * 
 * unsigned char	AUTH_MSG_KDC_REPLY protocol message type
 * 
 * [least significant	HOST_BYTE_ORDER	   sender's (server's) byte
 *  bit of above field]			   order
 * 
 * string		pname		   principal's name
 * 
 * string		pinst		   principal's instance
 * 
 * string		prealm		   principal's realm
 * 
 * unsigned long	time_ws		   client's timestamp
 * 
 * unsigned char	n		   number of tickets
 * 
 * unsigned long	x_date		   expiration date
 * 
 * unsigned char	kvno		   master key version
 * 
 * short		w_1		   cipher length
 * 
 * ---			cipher->dat	   cipher data
 */

KTEXT
create_auth_reply(char *pname,	/* Principal's name */
		  char *pinst,	/* Principal's instance */
		  char *prealm,	/* Principal's authentication domain */
		  int32_t time_ws, /* Workstation time */
		  int n,	/* Number of tickets */
		  u_int32_t x_date, /* Principal's expiration date */
		  int kvno,	/* Principal's key version number */
		  KTEXT cipher)	/* Cipher text with tickets and session keys */
{
    static  KTEXT_ST pkt_st;
    KTEXT pkt = &pkt_st;
    
    unsigned char *p = pkt->dat;

    p += krb_put_int(KRB_PROT_VERSION, p, 1);
    p += krb_put_int(AUTH_MSG_KDC_REPLY, p, 1);

    if(n != 0){
	/* barf on old code */
	krb_warning("create_auth_reply: don't give me no krb3 crap!"
		    " (n == %d)\n", n);
	return NULL;
    }


    p += krb_put_nir(pname, pinst, prealm, p);

    p += krb_put_int(time_ws, p, 4);
    
    p += krb_put_int(n, p, 1);
    
    p += krb_put_int(x_date, p, 4);
    
    p += krb_put_int(kvno, p, 1);
    
    p += krb_put_int(cipher->length, p, 2);
    
    memcpy(p, cipher->dat, cipher->length);
    p += cipher->length;

    pkt->length = p - pkt->dat;

    return pkt;
}
