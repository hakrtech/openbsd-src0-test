/* $OpenBSD: v3_akeya.c,v 1.6 2015/02/09 16:03:11 jsing Exp $ */
/* Written by Dr Stephen N Henson (steve@openssl.org) for the OpenSSL
 * project 1999.
 */
/* ====================================================================
 * Copyright (c) 1999 The OpenSSL Project.  All rights reserved.
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
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit. (http://www.OpenSSL.org/)"
 *
 * 4. The names "OpenSSL Toolkit" and "OpenSSL Project" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    licensing@OpenSSL.org.
 *
 * 5. Products derived from this software may not be called "OpenSSL"
 *    nor may "OpenSSL" appear in their names without prior written
 *    permission of the OpenSSL Project.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit (http://www.OpenSSL.org/)"
 *
 * THIS SOFTWARE IS PROVIDED BY THE OpenSSL PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OpenSSL PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com).  This product includes software written by Tim
 * Hudson (tjh@cryptsoft.com).
 *
 */

#include <stdio.h>

#include <openssl/asn1.h>
#include <openssl/asn1t.h>
#include <openssl/conf.h>
#include <openssl/x509v3.h>

ASN1_SEQUENCE(AUTHORITY_KEYID) = {
	ASN1_IMP_OPT(AUTHORITY_KEYID, keyid, ASN1_OCTET_STRING, 0),
	ASN1_IMP_SEQUENCE_OF_OPT(AUTHORITY_KEYID, issuer, GENERAL_NAME, 1),
	ASN1_IMP_OPT(AUTHORITY_KEYID, serial, ASN1_INTEGER, 2)
} ASN1_SEQUENCE_END(AUTHORITY_KEYID)


AUTHORITY_KEYID *
d2i_AUTHORITY_KEYID(AUTHORITY_KEYID **a, const unsigned char **in, long len)
{
	return (AUTHORITY_KEYID *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &AUTHORITY_KEYID_it);
}

int
i2d_AUTHORITY_KEYID(AUTHORITY_KEYID *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &AUTHORITY_KEYID_it);
}

AUTHORITY_KEYID *
AUTHORITY_KEYID_new(void)
{
	return (AUTHORITY_KEYID *)ASN1_item_new(&AUTHORITY_KEYID_it);
}

void
AUTHORITY_KEYID_free(AUTHORITY_KEYID *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &AUTHORITY_KEYID_it);
}
