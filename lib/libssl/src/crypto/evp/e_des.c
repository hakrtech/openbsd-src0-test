/* $OpenBSD: e_des.c,v 1.14 2015/10/12 06:05:52 guenther Exp $ */
/* Copyright (C) 1995-1998 Eric Young (eay@cryptsoft.com)
 * All rights reserved.
 *
 * This package is an SSL implementation written
 * by Eric Young (eay@cryptsoft.com).
 * The implementation was written so as to conform with Netscapes SSL.
 *
 * This library is free for commercial and non-commercial use as long as
 * the following conditions are aheared to.  The following conditions
 * apply to all code found in this distribution, be it the RC4, RSA,
 * lhash, DES, etc., code; not just the SSL code.  The SSL documentation
 * included with this distribution is covered by the same copyright terms
 * except that the holder is Tim Hudson (tjh@cryptsoft.com).
 *
 * Copyright remains Eric Young's, and as such any Copyright notices in
 * the code are not to be removed.
 * If this package is used in a product, Eric Young should be given attribution
 * as the author of the parts of the library used.
 * This can be in the form of a textual message at program startup or
 * in documentation (online or textual) provided with the package.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    "This product includes cryptographic software written by
 *     Eric Young (eay@cryptsoft.com)"
 *    The word 'cryptographic' can be left out if the rouines from the library
 *    being used are not cryptographic related :-).
 * 4. If you include any Windows specific code (or a derivative thereof) from
 *    the apps directory (application code) you must include an acknowledgement:
 *    "This product includes software written by Tim Hudson (tjh@cryptsoft.com)"
 *
 * THIS SOFTWARE IS PROVIDED BY ERIC YOUNG ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * The licence and distribution terms for any publically available version or
 * derivative of this code cannot be changed.  i.e. this code cannot simply be
 * copied and put under another distribution licence
 * [including the GNU Public Licence.]
 */

#include <stdio.h>

#include <openssl/opensslconf.h>

#ifndef OPENSSL_NO_DES

#include <openssl/evp.h>
#include <openssl/des.h>
#include <openssl/objects.h>

#include "evp_locl.h"

static int des_init_key(EVP_CIPHER_CTX *ctx, const unsigned char *key,
    const unsigned char *iv, int enc);
static int des_ctrl(EVP_CIPHER_CTX *c, int type, int arg, void *ptr);

/* Because of various casts and different names can't use IMPLEMENT_BLOCK_CIPHER */

static int
des_ecb_cipher(EVP_CIPHER_CTX *ctx, unsigned char *out,
    const unsigned char *in, size_t inl)
{
	BLOCK_CIPHER_ecb_loop()
		DES_ecb_encrypt((DES_cblock *)(in + i), (DES_cblock *)(out + i),
		    ctx->cipher_data, ctx->encrypt);
	return 1;
}

static int
des_ofb_cipher(EVP_CIPHER_CTX *ctx, unsigned char *out,
    const unsigned char *in, size_t inl)
{
	while (inl >= EVP_MAXCHUNK) {
		DES_ofb64_encrypt(in, out, (long)EVP_MAXCHUNK, ctx->cipher_data,
		    (DES_cblock *)ctx->iv, &ctx->num);
		inl -= EVP_MAXCHUNK;
		in += EVP_MAXCHUNK;
		out += EVP_MAXCHUNK;
	}
	if (inl)
		DES_ofb64_encrypt(in, out, (long)inl, ctx->cipher_data,
		    (DES_cblock *)ctx->iv, &ctx->num);
	return 1;
}

static int
des_cbc_cipher(EVP_CIPHER_CTX *ctx, unsigned char *out,
    const unsigned char *in, size_t inl)
{
	while (inl >= EVP_MAXCHUNK) {
		DES_ncbc_encrypt(in, out, (long)EVP_MAXCHUNK, ctx->cipher_data,
		    (DES_cblock *)ctx->iv, ctx->encrypt);
		inl -= EVP_MAXCHUNK;
		in += EVP_MAXCHUNK;
		out += EVP_MAXCHUNK;
	}
	if (inl)
		DES_ncbc_encrypt(in, out, (long)inl, ctx->cipher_data,
		    (DES_cblock *)ctx->iv, ctx->encrypt);
	return 1;
}

static int
des_cfb64_cipher(EVP_CIPHER_CTX *ctx, unsigned char *out,
    const unsigned char *in, size_t inl)
{
	while (inl >= EVP_MAXCHUNK) {
		DES_cfb64_encrypt(in, out, (long)EVP_MAXCHUNK, ctx->cipher_data,
		    (DES_cblock *)ctx->iv, &ctx->num, ctx->encrypt);
		inl -= EVP_MAXCHUNK;
		in += EVP_MAXCHUNK;
		out += EVP_MAXCHUNK;
	}
	if (inl)
		DES_cfb64_encrypt(in, out, (long)inl, ctx->cipher_data,
		    (DES_cblock *)ctx->iv, &ctx->num, ctx->encrypt);
	return 1;
}

/* Although we have a CFB-r implementation for DES, it doesn't pack the right
   way, so wrap it here */
static int
des_cfb1_cipher(EVP_CIPHER_CTX *ctx, unsigned char *out,
    const unsigned char *in, size_t inl)
{
	size_t n, chunk = EVP_MAXCHUNK/8;
	unsigned char c[1], d[1];

	if (inl < chunk)
		chunk = inl;

	while (inl && inl >= chunk) {
		for (n = 0; n < chunk*8; ++n) {
			c[0] = (in[n / 8] & (1 << (7 - n % 8))) ? 0x80 : 0;
			DES_cfb_encrypt(c, d, 1, 1, ctx->cipher_data,
			    (DES_cblock *)ctx->iv, ctx->encrypt);
			out[n / 8] = (out[n / 8] &
			    ~(0x80 >> (unsigned int)(n % 8))) |
			    ((d[0] & 0x80) >> (unsigned int)(n % 8));
		}
		inl -= chunk;
		in += chunk;
		out += chunk;
		if (inl < chunk)
			chunk = inl;
	}

	return 1;
}

static int
des_cfb8_cipher(EVP_CIPHER_CTX *ctx, unsigned char *out,
    const unsigned char *in, size_t inl)
{
	while (inl >= EVP_MAXCHUNK) {
		DES_cfb_encrypt(in, out, 8, (long)EVP_MAXCHUNK,
		    ctx->cipher_data, (DES_cblock *)ctx->iv, ctx->encrypt);
		inl -= EVP_MAXCHUNK;
		in += EVP_MAXCHUNK;
		out += EVP_MAXCHUNK;
	}
	if (inl)
		DES_cfb_encrypt(in, out, 8, (long)inl, ctx->cipher_data,
		    (DES_cblock *)ctx->iv, ctx->encrypt);
	return 1;
}

BLOCK_CIPHER_defs(des, DES_key_schedule, NID_des, 8, 8, 8, 64,
    EVP_CIPH_RAND_KEY, des_init_key, NULL,
    EVP_CIPHER_set_asn1_iv,
    EVP_CIPHER_get_asn1_iv,
    des_ctrl)

BLOCK_CIPHER_def_cfb(des, DES_key_schedule, NID_des, 8, 8, 1,
    EVP_CIPH_RAND_KEY, des_init_key, NULL,
    EVP_CIPHER_set_asn1_iv,
    EVP_CIPHER_get_asn1_iv, des_ctrl)

BLOCK_CIPHER_def_cfb(des, DES_key_schedule, NID_des, 8, 8, 8,
    EVP_CIPH_RAND_KEY, des_init_key, NULL,
    EVP_CIPHER_set_asn1_iv,
    EVP_CIPHER_get_asn1_iv, des_ctrl)

static int
des_init_key(EVP_CIPHER_CTX *ctx, const unsigned char *key,
    const unsigned char *iv, int enc)
{
	DES_cblock *deskey = (DES_cblock *)key;

	DES_set_key_unchecked(deskey, ctx->cipher_data);
	return 1;
}

static int
des_ctrl(EVP_CIPHER_CTX *c, int type, int arg, void *ptr)
{
	switch (type) {
	case EVP_CTRL_RAND_KEY:
		if (DES_random_key((DES_cblock *)ptr) == 0)
			return 0;
		return 1;

	default:
		return -1;
	}
}

#endif
