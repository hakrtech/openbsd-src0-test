/* $OpenBSD: pk7_asn1.c,v 1.11 2015/02/10 06:37:38 jsing Exp $ */
/* Written by Dr Stephen N Henson (steve@openssl.org) for the OpenSSL
 * project 2000.
 */
/* ====================================================================
 * Copyright (c) 2000 The OpenSSL Project.  All rights reserved.
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

#include <openssl/asn1t.h>
#include <openssl/pkcs7.h>
#include <openssl/x509.h>

/* PKCS#7 ASN1 module */

/* This is the ANY DEFINED BY table for the top level PKCS#7 structure */

ASN1_ADB_TEMPLATE(p7default) = ASN1_EXP_OPT(PKCS7, d.other, ASN1_ANY, 0);

ASN1_ADB(PKCS7) = {
	ADB_ENTRY(NID_pkcs7_data,
	    ASN1_NDEF_EXP_OPT(PKCS7, d.data, ASN1_OCTET_STRING_NDEF, 0)),
	ADB_ENTRY(NID_pkcs7_signed,
	    ASN1_NDEF_EXP_OPT(PKCS7, d.sign, PKCS7_SIGNED, 0)),
	ADB_ENTRY(NID_pkcs7_enveloped,
	    ASN1_NDEF_EXP_OPT(PKCS7, d.enveloped, PKCS7_ENVELOPE, 0)),
	ADB_ENTRY(NID_pkcs7_signedAndEnveloped,
	    ASN1_NDEF_EXP_OPT(PKCS7, d.signed_and_enveloped,
		PKCS7_SIGN_ENVELOPE, 0)),
	ADB_ENTRY(NID_pkcs7_digest,
	    ASN1_NDEF_EXP_OPT(PKCS7, d.digest, PKCS7_DIGEST, 0)),
	ADB_ENTRY(NID_pkcs7_encrypted,
	    ASN1_NDEF_EXP_OPT(PKCS7, d.encrypted, PKCS7_ENCRYPT, 0))
} ASN1_ADB_END(PKCS7, 0, type, 0, &p7default_tt, NULL);

/* PKCS#7 streaming support */
static int
pk7_cb(int operation, ASN1_VALUE **pval, const ASN1_ITEM *it, void *exarg)
{
	ASN1_STREAM_ARG *sarg = exarg;
	PKCS7 **pp7 = (PKCS7 **)pval;

	switch (operation) {
	case ASN1_OP_STREAM_PRE:
		if (PKCS7_stream(&sarg->boundary, *pp7) <= 0)
			return 0;

	case ASN1_OP_DETACHED_PRE:
		sarg->ndef_bio = PKCS7_dataInit(*pp7, sarg->out);
		if (!sarg->ndef_bio)
			return 0;
		break;

	case ASN1_OP_STREAM_POST:
	case ASN1_OP_DETACHED_POST:
		if (PKCS7_dataFinal(*pp7, sarg->ndef_bio) <= 0)
			return 0;
		break;
	}
	return 1;
}

ASN1_NDEF_SEQUENCE_cb(PKCS7, pk7_cb) = {
	ASN1_SIMPLE(PKCS7, type, ASN1_OBJECT),
	ASN1_ADB_OBJECT(PKCS7)
}ASN1_NDEF_SEQUENCE_END_cb(PKCS7, PKCS7)


PKCS7 *
d2i_PKCS7(PKCS7 **a, const unsigned char **in, long len)
{
	return (PKCS7 *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &PKCS7_it);
}

int
i2d_PKCS7(PKCS7 *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &PKCS7_it);
}

PKCS7 *
PKCS7_new(void)
{
	return (PKCS7 *)ASN1_item_new(&PKCS7_it);
}

void
PKCS7_free(PKCS7 *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &PKCS7_it);
}

int
i2d_PKCS7_NDEF(PKCS7 *a, unsigned char **out)
{
	return ASN1_item_ndef_i2d((ASN1_VALUE *)a, out, &PKCS7_it);
}

PKCS7 *
PKCS7_dup(PKCS7 *x)
{
	return ASN1_item_dup(&PKCS7_it, x);
}

ASN1_NDEF_SEQUENCE(PKCS7_SIGNED) = {
	ASN1_SIMPLE(PKCS7_SIGNED, version, ASN1_INTEGER),
	ASN1_SET_OF(PKCS7_SIGNED, md_algs, X509_ALGOR),
	ASN1_SIMPLE(PKCS7_SIGNED, contents, PKCS7),
	ASN1_IMP_SEQUENCE_OF_OPT(PKCS7_SIGNED, cert, X509, 0),
	ASN1_IMP_SET_OF_OPT(PKCS7_SIGNED, crl, X509_CRL, 1),
	ASN1_SET_OF(PKCS7_SIGNED, signer_info, PKCS7_SIGNER_INFO)
} ASN1_NDEF_SEQUENCE_END(PKCS7_SIGNED)


PKCS7_SIGNED *
d2i_PKCS7_SIGNED(PKCS7_SIGNED **a, const unsigned char **in, long len)
{
	return (PKCS7_SIGNED *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &PKCS7_SIGNED_it);
}

int
i2d_PKCS7_SIGNED(PKCS7_SIGNED *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &PKCS7_SIGNED_it);
}

PKCS7_SIGNED *
PKCS7_SIGNED_new(void)
{
	return (PKCS7_SIGNED *)ASN1_item_new(&PKCS7_SIGNED_it);
}

void
PKCS7_SIGNED_free(PKCS7_SIGNED *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &PKCS7_SIGNED_it);
}

/* Minor tweak to operation: free up EVP_PKEY */
static int
si_cb(int operation, ASN1_VALUE **pval, const ASN1_ITEM *it, void *exarg)
{
	if (operation == ASN1_OP_FREE_POST) {
		PKCS7_SIGNER_INFO *si = (PKCS7_SIGNER_INFO *)*pval;
		EVP_PKEY_free(si->pkey);
	}
	return 1;
}

ASN1_SEQUENCE_cb(PKCS7_SIGNER_INFO, si_cb) = {
	ASN1_SIMPLE(PKCS7_SIGNER_INFO, version, ASN1_INTEGER),
	ASN1_SIMPLE(PKCS7_SIGNER_INFO, issuer_and_serial,
	    PKCS7_ISSUER_AND_SERIAL),
	ASN1_SIMPLE(PKCS7_SIGNER_INFO, digest_alg, X509_ALGOR),
	/* NB this should be a SET OF but we use a SEQUENCE OF so the
	 * original order * is retained when the structure is reencoded.
	 * Since the attributes are implicitly tagged this will not affect
	 * the encoding.
	 */
	ASN1_IMP_SEQUENCE_OF_OPT(PKCS7_SIGNER_INFO, auth_attr,
	    X509_ATTRIBUTE, 0),
	ASN1_SIMPLE(PKCS7_SIGNER_INFO, digest_enc_alg, X509_ALGOR),
	ASN1_SIMPLE(PKCS7_SIGNER_INFO, enc_digest, ASN1_OCTET_STRING),
	ASN1_IMP_SET_OF_OPT(PKCS7_SIGNER_INFO, unauth_attr, X509_ATTRIBUTE, 1)
} ASN1_SEQUENCE_END_cb(PKCS7_SIGNER_INFO, PKCS7_SIGNER_INFO)


PKCS7_SIGNER_INFO *
d2i_PKCS7_SIGNER_INFO(PKCS7_SIGNER_INFO **a, const unsigned char **in, long len)
{
	return (PKCS7_SIGNER_INFO *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &PKCS7_SIGNER_INFO_it);
}

int
i2d_PKCS7_SIGNER_INFO(PKCS7_SIGNER_INFO *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &PKCS7_SIGNER_INFO_it);
}

PKCS7_SIGNER_INFO *
PKCS7_SIGNER_INFO_new(void)
{
	return (PKCS7_SIGNER_INFO *)ASN1_item_new(&PKCS7_SIGNER_INFO_it);
}

void
PKCS7_SIGNER_INFO_free(PKCS7_SIGNER_INFO *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &PKCS7_SIGNER_INFO_it);
}

ASN1_SEQUENCE(PKCS7_ISSUER_AND_SERIAL) = {
	ASN1_SIMPLE(PKCS7_ISSUER_AND_SERIAL, issuer, X509_NAME),
	ASN1_SIMPLE(PKCS7_ISSUER_AND_SERIAL, serial, ASN1_INTEGER)
} ASN1_SEQUENCE_END(PKCS7_ISSUER_AND_SERIAL)


PKCS7_ISSUER_AND_SERIAL *
d2i_PKCS7_ISSUER_AND_SERIAL(PKCS7_ISSUER_AND_SERIAL **a, const unsigned char **in, long len)
{
	return (PKCS7_ISSUER_AND_SERIAL *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &PKCS7_ISSUER_AND_SERIAL_it);
}

int
i2d_PKCS7_ISSUER_AND_SERIAL(PKCS7_ISSUER_AND_SERIAL *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &PKCS7_ISSUER_AND_SERIAL_it);
}

PKCS7_ISSUER_AND_SERIAL *
PKCS7_ISSUER_AND_SERIAL_new(void)
{
	return (PKCS7_ISSUER_AND_SERIAL *)ASN1_item_new(&PKCS7_ISSUER_AND_SERIAL_it);
}

void
PKCS7_ISSUER_AND_SERIAL_free(PKCS7_ISSUER_AND_SERIAL *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &PKCS7_ISSUER_AND_SERIAL_it);
}

ASN1_NDEF_SEQUENCE(PKCS7_ENVELOPE) = {
	ASN1_SIMPLE(PKCS7_ENVELOPE, version, ASN1_INTEGER),
	ASN1_SET_OF(PKCS7_ENVELOPE, recipientinfo, PKCS7_RECIP_INFO),
	ASN1_SIMPLE(PKCS7_ENVELOPE, enc_data, PKCS7_ENC_CONTENT)
} ASN1_NDEF_SEQUENCE_END(PKCS7_ENVELOPE)


PKCS7_ENVELOPE *
d2i_PKCS7_ENVELOPE(PKCS7_ENVELOPE **a, const unsigned char **in, long len)
{
	return (PKCS7_ENVELOPE *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &PKCS7_ENVELOPE_it);
}

int
i2d_PKCS7_ENVELOPE(PKCS7_ENVELOPE *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &PKCS7_ENVELOPE_it);
}

PKCS7_ENVELOPE *
PKCS7_ENVELOPE_new(void)
{
	return (PKCS7_ENVELOPE *)ASN1_item_new(&PKCS7_ENVELOPE_it);
}

void
PKCS7_ENVELOPE_free(PKCS7_ENVELOPE *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &PKCS7_ENVELOPE_it);
}

/* Minor tweak to operation: free up X509 */
static int
ri_cb(int operation, ASN1_VALUE **pval, const ASN1_ITEM *it, void *exarg)
{
	if (operation == ASN1_OP_FREE_POST) {
		PKCS7_RECIP_INFO *ri = (PKCS7_RECIP_INFO *)*pval;
		X509_free(ri->cert);
	}
	return 1;
}

ASN1_SEQUENCE_cb(PKCS7_RECIP_INFO, ri_cb) = {
	ASN1_SIMPLE(PKCS7_RECIP_INFO, version, ASN1_INTEGER),
	ASN1_SIMPLE(PKCS7_RECIP_INFO, issuer_and_serial,
	    PKCS7_ISSUER_AND_SERIAL),
	ASN1_SIMPLE(PKCS7_RECIP_INFO, key_enc_algor, X509_ALGOR),
	ASN1_SIMPLE(PKCS7_RECIP_INFO, enc_key, ASN1_OCTET_STRING)
} ASN1_SEQUENCE_END_cb(PKCS7_RECIP_INFO, PKCS7_RECIP_INFO)


PKCS7_RECIP_INFO *
d2i_PKCS7_RECIP_INFO(PKCS7_RECIP_INFO **a, const unsigned char **in, long len)
{
	return (PKCS7_RECIP_INFO *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &PKCS7_RECIP_INFO_it);
}

int
i2d_PKCS7_RECIP_INFO(PKCS7_RECIP_INFO *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &PKCS7_RECIP_INFO_it);
}

PKCS7_RECIP_INFO *
PKCS7_RECIP_INFO_new(void)
{
	return (PKCS7_RECIP_INFO *)ASN1_item_new(&PKCS7_RECIP_INFO_it);
}

void
PKCS7_RECIP_INFO_free(PKCS7_RECIP_INFO *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &PKCS7_RECIP_INFO_it);
}

ASN1_NDEF_SEQUENCE(PKCS7_ENC_CONTENT) = {
	ASN1_SIMPLE(PKCS7_ENC_CONTENT, content_type, ASN1_OBJECT),
	ASN1_SIMPLE(PKCS7_ENC_CONTENT, algorithm, X509_ALGOR),
	ASN1_IMP_OPT(PKCS7_ENC_CONTENT, enc_data, ASN1_OCTET_STRING_NDEF, 0)
} ASN1_NDEF_SEQUENCE_END(PKCS7_ENC_CONTENT)


PKCS7_ENC_CONTENT *
d2i_PKCS7_ENC_CONTENT(PKCS7_ENC_CONTENT **a, const unsigned char **in, long len)
{
	return (PKCS7_ENC_CONTENT *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &PKCS7_ENC_CONTENT_it);
}

int
i2d_PKCS7_ENC_CONTENT(PKCS7_ENC_CONTENT *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &PKCS7_ENC_CONTENT_it);
}

PKCS7_ENC_CONTENT *
PKCS7_ENC_CONTENT_new(void)
{
	return (PKCS7_ENC_CONTENT *)ASN1_item_new(&PKCS7_ENC_CONTENT_it);
}

void
PKCS7_ENC_CONTENT_free(PKCS7_ENC_CONTENT *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &PKCS7_ENC_CONTENT_it);
}

ASN1_NDEF_SEQUENCE(PKCS7_SIGN_ENVELOPE) = {
	ASN1_SIMPLE(PKCS7_SIGN_ENVELOPE, version, ASN1_INTEGER),
	ASN1_SET_OF(PKCS7_SIGN_ENVELOPE, recipientinfo, PKCS7_RECIP_INFO),
	ASN1_SET_OF(PKCS7_SIGN_ENVELOPE, md_algs, X509_ALGOR),
	ASN1_SIMPLE(PKCS7_SIGN_ENVELOPE, enc_data, PKCS7_ENC_CONTENT),
	ASN1_IMP_SET_OF_OPT(PKCS7_SIGN_ENVELOPE, cert, X509, 0),
	ASN1_IMP_SET_OF_OPT(PKCS7_SIGN_ENVELOPE, crl, X509_CRL, 1),
	ASN1_SET_OF(PKCS7_SIGN_ENVELOPE, signer_info, PKCS7_SIGNER_INFO)
} ASN1_NDEF_SEQUENCE_END(PKCS7_SIGN_ENVELOPE)


PKCS7_SIGN_ENVELOPE *
d2i_PKCS7_SIGN_ENVELOPE(PKCS7_SIGN_ENVELOPE **a, const unsigned char **in, long len)
{
	return (PKCS7_SIGN_ENVELOPE *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &PKCS7_SIGN_ENVELOPE_it);
}

int
i2d_PKCS7_SIGN_ENVELOPE(PKCS7_SIGN_ENVELOPE *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &PKCS7_SIGN_ENVELOPE_it);
}

PKCS7_SIGN_ENVELOPE *
PKCS7_SIGN_ENVELOPE_new(void)
{
	return (PKCS7_SIGN_ENVELOPE *)ASN1_item_new(&PKCS7_SIGN_ENVELOPE_it);
}

void
PKCS7_SIGN_ENVELOPE_free(PKCS7_SIGN_ENVELOPE *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &PKCS7_SIGN_ENVELOPE_it);
}

ASN1_NDEF_SEQUENCE(PKCS7_ENCRYPT) = {
	ASN1_SIMPLE(PKCS7_ENCRYPT, version, ASN1_INTEGER),
	ASN1_SIMPLE(PKCS7_ENCRYPT, enc_data, PKCS7_ENC_CONTENT)
} ASN1_NDEF_SEQUENCE_END(PKCS7_ENCRYPT)


PKCS7_ENCRYPT *
d2i_PKCS7_ENCRYPT(PKCS7_ENCRYPT **a, const unsigned char **in, long len)
{
	return (PKCS7_ENCRYPT *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &PKCS7_ENCRYPT_it);
}

int
i2d_PKCS7_ENCRYPT(PKCS7_ENCRYPT *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &PKCS7_ENCRYPT_it);
}

PKCS7_ENCRYPT *
PKCS7_ENCRYPT_new(void)
{
	return (PKCS7_ENCRYPT *)ASN1_item_new(&PKCS7_ENCRYPT_it);
}

void
PKCS7_ENCRYPT_free(PKCS7_ENCRYPT *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &PKCS7_ENCRYPT_it);
}

ASN1_NDEF_SEQUENCE(PKCS7_DIGEST) = {
	ASN1_SIMPLE(PKCS7_DIGEST, version, ASN1_INTEGER),
	ASN1_SIMPLE(PKCS7_DIGEST, md, X509_ALGOR),
	ASN1_SIMPLE(PKCS7_DIGEST, contents, PKCS7),
	ASN1_SIMPLE(PKCS7_DIGEST, digest, ASN1_OCTET_STRING)
} ASN1_NDEF_SEQUENCE_END(PKCS7_DIGEST)


PKCS7_DIGEST *
d2i_PKCS7_DIGEST(PKCS7_DIGEST **a, const unsigned char **in, long len)
{
	return (PKCS7_DIGEST *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &PKCS7_DIGEST_it);
}

int
i2d_PKCS7_DIGEST(PKCS7_DIGEST *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &PKCS7_DIGEST_it);
}

PKCS7_DIGEST *
PKCS7_DIGEST_new(void)
{
	return (PKCS7_DIGEST *)ASN1_item_new(&PKCS7_DIGEST_it);
}

void
PKCS7_DIGEST_free(PKCS7_DIGEST *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &PKCS7_DIGEST_it);
}

/* Specials for authenticated attributes */

/* When signing attributes we want to reorder them to match the sorted
 * encoding.
 */

ASN1_ITEM_TEMPLATE(PKCS7_ATTR_SIGN) =
    ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_SET_ORDER, 0, PKCS7_ATTRIBUTES,
	X509_ATTRIBUTE)
ASN1_ITEM_TEMPLATE_END(PKCS7_ATTR_SIGN)

/* When verifying attributes we need to use the received order. So
 * we use SEQUENCE OF and tag it to SET OF
 */

ASN1_ITEM_TEMPLATE(PKCS7_ATTR_VERIFY) =
    ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_SEQUENCE_OF | ASN1_TFLG_IMPTAG |
	ASN1_TFLG_UNIVERSAL, V_ASN1_SET, PKCS7_ATTRIBUTES, X509_ATTRIBUTE)
ASN1_ITEM_TEMPLATE_END(PKCS7_ATTR_VERIFY)


int
PKCS7_print_ctx(BIO *out, PKCS7 *x, int indent, const ASN1_PCTX *pctx)
{
	return ASN1_item_print(out, (ASN1_VALUE *)x, indent,
	    &PKCS7_it, pctx);
}
