/*	$OpenBSD: aesni.c,v 1.29 2014/11/18 15:18:01 mikeb Exp $	*/
/*-
 * Copyright (c) 2003 Jason Wright
 * Copyright (c) 2003, 2004 Theo de Raadt
 * Copyright (c) 2010, Thordur I. Bjornsson
 * Copyright (c) 2010, Mike Belopuhov
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/queue.h>
#include <sys/types.h>
#include <sys/malloc.h>
#include <sys/pool.h>
#include <sys/mbuf.h>
#include <sys/pool.h>

#include <crypto/cryptodev.h>
#include <crypto/rijndael.h>
#include <crypto/gmac.h>
#include <crypto/xform.h>
#include <crypto/cryptosoft.h>

#include <dev/rndvar.h>

#include <machine/fpu.h>

/* defines from crypto/xform.c */
#define AESCTR_NONCESIZE	4
#define AESCTR_IVSIZE		8
#define AESCTR_BLOCKSIZE	16

#define AES_XTS_BLOCKSIZE	16
#define AES_XTS_IVSIZE		8
#define AES_XTS_ALPHA		0x87	/* GF(2^128) generator polynomial */

struct aesni_aes_ctx {
	uint32_t		 aes_ekey[4 * (AES_MAXROUNDS + 1)];
	uint32_t		 aes_dkey[4 * (AES_MAXROUNDS + 1)];
	uint32_t		 aes_klen;
	uint32_t		 aes_pad[3];
};

struct aesni_xts_ctx {
	struct aesni_aes_ctx	 xts_keys[2];
};

struct aesni_session {
	uint32_t		 ses_ekey[4 * (AES_MAXROUNDS + 1)];
	uint32_t		 ses_dkey[4 * (AES_MAXROUNDS + 1)];
	uint32_t		 ses_klen;
	uint8_t			 ses_nonce[AESCTR_NONCESIZE];
	int			 ses_sid;
	GHASH_CTX		*ses_ghash;
	struct aesni_xts_ctx	*ses_xts;
	struct swcr_data	*ses_swd;
	LIST_ENTRY(aesni_session)
				 ses_entries;
};

struct aesni_softc {
	uint8_t			*sc_buf;
	size_t			 sc_buflen;
	int32_t			 sc_cid;
	uint32_t		 sc_sid;
	LIST_HEAD(, aesni_session)
				 sc_sessions;
} *aesni_sc;

struct pool aesnipl;

uint32_t aesni_ops;

/* assembler-assisted key setup */
extern void aesni_set_key(struct aesni_session *ses, uint8_t *key, size_t len);

/* aes encryption/decryption */
extern void aesni_enc(struct aesni_session *ses, uint8_t *dst, uint8_t *src);
extern void aesni_dec(struct aesni_session *ses, uint8_t *dst, uint8_t *src);

/* assembler-assisted CBC mode */
extern void aesni_cbc_enc(struct aesni_session *ses, uint8_t *dst,
	    uint8_t *src, size_t len, uint8_t *iv);
extern void aesni_cbc_dec(struct aesni_session *ses, uint8_t *dst,
	    uint8_t *src, size_t len, uint8_t *iv);

/* assembler-assisted CTR mode */
extern void aesni_ctr_enc(struct aesni_session *ses, uint8_t *dst,
	    uint8_t *src, size_t len, uint8_t *icb);

/* assembler-assisted XTS mode */
extern void aesni_xts_enc(struct aesni_xts_ctx *xts, uint8_t *dst,
	    uint8_t *src, size_t len, uint8_t *tweak);
extern void aesni_xts_dec(struct aesni_xts_ctx *xts, uint8_t *dst,
	    uint8_t *src, size_t len, uint8_t *tweak);

/* assembler-assisted GMAC */
extern void aesni_gmac_update(GHASH_CTX *ghash, uint8_t *src, size_t len);
extern void aesni_gmac_final(struct aesni_session *ses, uint8_t *tag,
    uint8_t *icb, uint8_t *hashstate);

void	aesni_setup(void);
int	aesni_newsession(u_int32_t *, struct cryptoini *);
int	aesni_freesession(u_int64_t);
int	aesni_process(struct cryptop *);

int	aesni_swauth(struct cryptop *, struct cryptodesc *, struct swcr_data *,
	    caddr_t);

int	aesni_encdec(struct cryptop *, struct cryptodesc *,
	    struct cryptodesc *, struct aesni_session *);

void
aesni_setup(void)
{
	int algs[CRYPTO_ALGORITHM_MAX + 1];

	aesni_sc = malloc(sizeof(*aesni_sc), M_DEVBUF, M_NOWAIT|M_ZERO);
	if (aesni_sc == NULL)
		return;

	aesni_sc->sc_buf = malloc(PAGE_SIZE, M_DEVBUF, M_NOWAIT|M_ZERO);
	if (aesni_sc->sc_buf != NULL)
		aesni_sc->sc_buflen = PAGE_SIZE;

	bzero(algs, sizeof(algs));

	/* Encryption algorithms. */
	algs[CRYPTO_AES_CBC] = CRYPTO_ALG_FLAG_SUPPORTED;
	algs[CRYPTO_AES_CTR] = CRYPTO_ALG_FLAG_SUPPORTED;
	algs[CRYPTO_AES_GCM_16] = CRYPTO_ALG_FLAG_SUPPORTED;
	algs[CRYPTO_AES_XTS] = CRYPTO_ALG_FLAG_SUPPORTED;

	/* Authenticated encryption algorithms. */
	algs[CRYPTO_AES_GMAC] = CRYPTO_ALG_FLAG_SUPPORTED;
	algs[CRYPTO_AES_128_GMAC] = CRYPTO_ALG_FLAG_SUPPORTED;
	algs[CRYPTO_AES_192_GMAC] = CRYPTO_ALG_FLAG_SUPPORTED;
	algs[CRYPTO_AES_256_GMAC] = CRYPTO_ALG_FLAG_SUPPORTED;

	/* HMACs needed for IPsec, uses software crypto. */
	algs[CRYPTO_MD5_HMAC] = CRYPTO_ALG_FLAG_SUPPORTED;
	algs[CRYPTO_SHA1_HMAC] = CRYPTO_ALG_FLAG_SUPPORTED;
	algs[CRYPTO_RIPEMD160_HMAC] = CRYPTO_ALG_FLAG_SUPPORTED;
	algs[CRYPTO_SHA2_256_HMAC] = CRYPTO_ALG_FLAG_SUPPORTED;
	algs[CRYPTO_SHA2_384_HMAC] = CRYPTO_ALG_FLAG_SUPPORTED;
	algs[CRYPTO_SHA2_512_HMAC] = CRYPTO_ALG_FLAG_SUPPORTED;

	/* IPsec Extended Sequence Numbers. */
	algs[CRYPTO_ESN] = CRYPTO_ALG_FLAG_SUPPORTED;

	aesni_sc->sc_cid = crypto_get_driverid(0);
	if (aesni_sc->sc_cid < 0) {
		free(aesni_sc, M_DEVBUF, 0);
		return;
	}

	pool_init(&aesnipl, sizeof(struct aesni_session), 16, 0, 0,
	    "aesni", NULL);
	pool_setlowat(&aesnipl, 2);

	crypto_register(aesni_sc->sc_cid, algs, aesni_newsession,
	    aesni_freesession, aesni_process);
}

int
aesni_newsession(u_int32_t *sidp, struct cryptoini *cri)
{
	struct aesni_session *ses = NULL;
	struct aesni_aes_ctx *aes1, *aes2;
	struct cryptoini *c;
	struct auth_hash *axf;
	struct swcr_data *swd;
	int i;

	if (sidp == NULL || cri == NULL)
		return (EINVAL);

	ses = pool_get(&aesnipl, PR_NOWAIT | PR_ZERO);
	if (!ses)
		return (ENOMEM);
	LIST_INSERT_HEAD(&aesni_sc->sc_sessions, ses, ses_entries);
	ses->ses_sid = ++aesni_sc->sc_sid;

	for (c = cri; c != NULL; c = c->cri_next) {
		switch (c->cri_alg) {
		case CRYPTO_AES_CBC:
			ses->ses_klen = c->cri_klen / 8;
			fpu_kernel_enter();
			aesni_set_key(ses, c->cri_key, ses->ses_klen);
			fpu_kernel_exit();
			break;

		case CRYPTO_AES_CTR:
		case CRYPTO_AES_GCM_16:
		case CRYPTO_AES_GMAC:
			ses->ses_klen = c->cri_klen / 8 - AESCTR_NONCESIZE;
			bcopy(c->cri_key + ses->ses_klen, ses->ses_nonce,
			    AESCTR_NONCESIZE);
			fpu_kernel_enter();
			aesni_set_key(ses, c->cri_key, ses->ses_klen);
			fpu_kernel_exit();
			break;

		case CRYPTO_AES_XTS:
			ses->ses_xts = malloc(sizeof(struct aesni_xts_ctx),
			    M_CRYPTO_DATA, M_NOWAIT | M_ZERO);
			if (ses->ses_xts == NULL) {
				aesni_freesession(ses->ses_sid);
				return (ENOMEM);
			}

			ses->ses_klen = c->cri_klen / 16;
			aes1 = &ses->ses_xts->xts_keys[0];
			aes1->aes_klen = ses->ses_klen;
			aes2 = &ses->ses_xts->xts_keys[1];
			aes2->aes_klen = ses->ses_klen;

			fpu_kernel_enter();
			aesni_set_key((struct aesni_session *)aes1,
			    c->cri_key, aes1->aes_klen);
			aesni_set_key((struct aesni_session *)aes2,
			    c->cri_key + ses->ses_klen, aes2->aes_klen);
			fpu_kernel_exit();
			break;

		case CRYPTO_AES_128_GMAC:
		case CRYPTO_AES_192_GMAC:
		case CRYPTO_AES_256_GMAC:
			ses->ses_ghash = malloc(sizeof(GHASH_CTX),
			    M_CRYPTO_DATA, M_NOWAIT | M_ZERO);
			if (ses->ses_ghash == NULL) {
				aesni_freesession(ses->ses_sid);
				return (ENOMEM);
			}

			bzero(ses->ses_ghash->H, GMAC_BLOCK_LEN);
			bzero(ses->ses_ghash->S, GMAC_BLOCK_LEN);
			bzero(ses->ses_ghash->Z, GMAC_BLOCK_LEN);

			/* prepare a hash subkey */
			aesni_enc(ses, ses->ses_ghash->H, ses->ses_ghash->H);
			break;

		case CRYPTO_MD5_HMAC:
			axf = &auth_hash_hmac_md5_96;
			goto authcommon;
		case CRYPTO_SHA1_HMAC:
			axf = &auth_hash_hmac_sha1_96;
			goto authcommon;
		case CRYPTO_RIPEMD160_HMAC:
			axf = &auth_hash_hmac_ripemd_160_96;
			goto authcommon;
		case CRYPTO_SHA2_256_HMAC:
			axf = &auth_hash_hmac_sha2_256_128;
			goto authcommon;
		case CRYPTO_SHA2_384_HMAC:
			axf = &auth_hash_hmac_sha2_384_192;
			goto authcommon;
		case CRYPTO_SHA2_512_HMAC:
			axf = &auth_hash_hmac_sha2_512_256;
		authcommon:
			swd = malloc(sizeof(struct swcr_data), M_CRYPTO_DATA,
			    M_NOWAIT|M_ZERO);
			if (swd == NULL) {
				aesni_freesession(ses->ses_sid);
				return (ENOMEM);
			}
			ses->ses_swd = swd;

			swd->sw_ictx = malloc(axf->ctxsize, M_CRYPTO_DATA,
			    M_NOWAIT);
			if (swd->sw_ictx == NULL) {
				aesni_freesession(ses->ses_sid);
				return (ENOMEM);
			}

			swd->sw_octx = malloc(axf->ctxsize, M_CRYPTO_DATA,
			    M_NOWAIT);
			if (swd->sw_octx == NULL) {
				aesni_freesession(ses->ses_sid);
				return (ENOMEM);
			}

			for (i = 0; i < c->cri_klen / 8; i++)
				c->cri_key[i] ^= HMAC_IPAD_VAL;

			axf->Init(swd->sw_ictx);
			axf->Update(swd->sw_ictx, c->cri_key, c->cri_klen / 8);
			axf->Update(swd->sw_ictx, hmac_ipad_buffer,
			    axf->blocksize - (c->cri_klen / 8));

			for (i = 0; i < c->cri_klen / 8; i++)
				c->cri_key[i] ^= (HMAC_IPAD_VAL ^
				    HMAC_OPAD_VAL);

			axf->Init(swd->sw_octx);
			axf->Update(swd->sw_octx, c->cri_key, c->cri_klen / 8);
			axf->Update(swd->sw_octx, hmac_opad_buffer,
			    axf->blocksize - (c->cri_klen / 8));

			for (i = 0; i < c->cri_klen / 8; i++)
				c->cri_key[i] ^= HMAC_OPAD_VAL;

			swd->sw_axf = axf;
			swd->sw_alg = c->cri_alg;

			break;

		case CRYPTO_ESN:
			/* nothing to do */
			break;

		default:
			aesni_freesession(ses->ses_sid);
			return (EINVAL);
		}
	}

	*sidp = ses->ses_sid;
	return (0);
}

int
aesni_freesession(u_int64_t tid)
{
	struct aesni_session *ses;
	struct swcr_data *swd;
	struct auth_hash *axf;
	u_int32_t sid = (u_int32_t)tid;

	LIST_FOREACH(ses, &aesni_sc->sc_sessions, ses_entries) {
		if (ses->ses_sid == sid)
			break;
	}

	if (ses == NULL)
		return (EINVAL);

	LIST_REMOVE(ses, ses_entries);

	if (ses->ses_ghash) {
		explicit_bzero(ses->ses_ghash, sizeof(GHASH_CTX));
		free(ses->ses_ghash, M_CRYPTO_DATA, 0);
	}

	if (ses->ses_xts) {
		explicit_bzero(ses->ses_xts, sizeof(struct aesni_xts_ctx));
		free(ses->ses_xts, M_CRYPTO_DATA, 0);
	}

	if (ses->ses_swd) {
		swd = ses->ses_swd;
		axf = swd->sw_axf;

		if (swd->sw_ictx) {
			explicit_bzero(swd->sw_ictx, axf->ctxsize);
			free(swd->sw_ictx, M_CRYPTO_DATA, 0);
		}
		if (swd->sw_octx) {
			explicit_bzero(swd->sw_octx, axf->ctxsize);
			free(swd->sw_octx, M_CRYPTO_DATA, 0);
		}
		free(swd, M_CRYPTO_DATA, 0);
	}

	explicit_bzero(ses, sizeof (*ses));
	pool_put(&aesnipl, ses);

	return (0);
}

int
aesni_swauth(struct cryptop *crp, struct cryptodesc *crd,
    struct swcr_data *sw, caddr_t buf)
{
	int type;

	if (crp->crp_flags & CRYPTO_F_IMBUF)
		type = CRYPTO_BUF_MBUF;
	else
		type = CRYPTO_BUF_IOV;

	return (swcr_authcompute(crp, crd, sw, buf, type));
}

int
aesni_encdec(struct cryptop *crp, struct cryptodesc *crd,
    struct cryptodesc *crda, struct aesni_session *ses)
{
	int aadlen, err, ivlen, iskip, oskip, rlen;
	uint8_t iv[EALG_MAX_BLOCK_LEN];
	uint8_t icb[AESCTR_BLOCKSIZE];
	uint8_t tag[GMAC_DIGEST_LEN];
	uint8_t *buf = aesni_sc->sc_buf;
	uint32_t *dw;

	aadlen = rlen = err = iskip = oskip = 0;

	if (crd->crd_len > aesni_sc->sc_buflen) {
		if (buf != NULL) {
			explicit_bzero(buf, aesni_sc->sc_buflen);
			free(buf, M_DEVBUF, 0);
		}

		aesni_sc->sc_buflen = 0;
		rlen = roundup(crd->crd_len, EALG_MAX_BLOCK_LEN);
		aesni_sc->sc_buf = buf = malloc(rlen, M_DEVBUF, M_NOWAIT |
		    M_ZERO);
		if (buf == NULL)
			return (ENOMEM);
		aesni_sc->sc_buflen = rlen;
	}

	/* CBC uses 16, CTR/XTS only 8. */
	ivlen = (crd->crd_alg == CRYPTO_AES_CBC) ? 16 : 8;

	/* Initialize the IV */
	if (crd->crd_flags & CRD_F_ENCRYPT) {
		if (crd->crd_flags & CRD_F_IV_EXPLICIT)
			bcopy(crd->crd_iv, iv, ivlen);
		else
			arc4random_buf(iv, ivlen);

		/* Do we need to write the IV */
		if ((crd->crd_flags & CRD_F_IV_PRESENT) == 0) {
			if (crp->crp_flags & CRYPTO_F_IMBUF) {
				if (m_copyback((struct mbuf *)crp->crp_buf,
				    crd->crd_inject, ivlen, iv, M_NOWAIT)) {
					err = ENOMEM;
					goto out;
				}
			} else
				cuio_copyback((struct uio *)crp->crp_buf,
				    crd->crd_inject, ivlen, iv);
		}
	} else {
		if (crd->crd_flags & CRD_F_IV_EXPLICIT)
			bcopy(crd->crd_iv, iv, ivlen);
		else {
			if (crp->crp_flags & CRYPTO_F_IMBUF)
				m_copydata((struct mbuf *)crp->crp_buf,
				    crd->crd_inject, ivlen, iv);
			else
				cuio_copydata((struct uio *)crp->crp_buf,
				    crd->crd_inject, ivlen, iv);
		}
	}

	if (crda) {
		/* Supply GMAC with AAD */
		aadlen = crda->crd_len;
		if (crda->crd_flags & CRD_F_ESN) {
			aadlen += 4;
			/* SPI */
			if (crp->crp_flags & CRYPTO_F_IMBUF)
				m_copydata((struct mbuf *)crp->crp_buf,
				    crda->crd_skip, 4, buf);
			else
				cuio_copydata((struct uio *)crp->crp_buf,
				    crda->crd_skip, 4, buf);
			iskip = 4; /* additional input offset */
			/* ESN */
			bcopy(crda->crd_esn, buf + 4, 4);
			oskip = iskip + 4; /* offset output buffer by 8 */
		}
		rlen = roundup(aadlen, GMAC_BLOCK_LEN);
		if (crp->crp_flags & CRYPTO_F_IMBUF)
			m_copydata((struct mbuf *)crp->crp_buf,
			    crda->crd_skip + iskip, crda->crd_len - iskip,
			    buf + oskip);
		else
			cuio_copydata((struct uio *)crp->crp_buf,
			    crda->crd_skip + iskip, crda->crd_len - iskip,
			    buf + oskip);
		fpu_kernel_enter();
		aesni_gmac_update(ses->ses_ghash, buf, rlen);
		fpu_kernel_exit();
		bzero(buf, aadlen);
	}

	/* Copy data to be processed to the buffer */
	if (crp->crp_flags & CRYPTO_F_IMBUF)
		m_copydata((struct mbuf *)crp->crp_buf, crd->crd_skip,
		    crd->crd_len, buf);
	else
		cuio_copydata((struct uio *)crp->crp_buf, crd->crd_skip,
		    crd->crd_len, buf);

	if (crd->crd_alg == CRYPTO_AES_CTR ||
	    crd->crd_alg == CRYPTO_AES_GCM_16 ||
	    crd->crd_alg == CRYPTO_AES_GMAC) {
		bzero(icb, AESCTR_BLOCKSIZE);
		bcopy(ses->ses_nonce, icb, AESCTR_NONCESIZE);
		bcopy(iv, icb + AESCTR_NONCESIZE, AESCTR_IVSIZE);
		/* rlen is for gcm and gmac only */
		rlen = roundup(crd->crd_len, AESCTR_BLOCKSIZE);
	}

	/* Apply cipher */
	fpu_kernel_enter();
	switch (crd->crd_alg) {
	case CRYPTO_AES_CBC:
		if (crd->crd_flags & CRD_F_ENCRYPT)
			aesni_cbc_enc(ses, buf, buf, crd->crd_len, iv);
		else
			aesni_cbc_dec(ses, buf, buf, crd->crd_len, iv);
		break;
	case CRYPTO_AES_CTR:
		aesni_ctr_enc(ses, buf, buf, crd->crd_len, icb);
		break;
	case CRYPTO_AES_GCM_16:
		icb[AESCTR_BLOCKSIZE - 1] = 1;
		if (crd->crd_flags & CRD_F_ENCRYPT) {
			/* encrypt padded data */
			aesni_ctr_enc(ses, buf, buf, rlen, icb);
			/* zero out padding bytes */
			bzero(buf + crd->crd_len, rlen - crd->crd_len);
			/* hash encrypted data padded with zeroes */
			aesni_gmac_update(ses->ses_ghash, buf, rlen);
		} else {
			aesni_gmac_update(ses->ses_ghash, buf, rlen);
			aesni_ctr_enc(ses, buf, buf, rlen, icb);
		}
		goto gcmcommon;
	case CRYPTO_AES_GMAC:
		icb[AESCTR_BLOCKSIZE - 1] = 1;
		aesni_gmac_update(ses->ses_ghash, buf, rlen);
	gcmcommon:
		/* lengths block */
		bzero(tag, GMAC_BLOCK_LEN);
		dw = (uint32_t *)tag + 1;
		*dw = htobe32(aadlen * 8);
		dw = (uint32_t *)tag + 3;
		*dw = htobe32(crd->crd_len * 8);
		aesni_gmac_update(ses->ses_ghash, tag, GMAC_BLOCK_LEN);
		/* finalization */
		aesni_gmac_final(ses, tag, icb, ses->ses_ghash->S);
		break;
	case CRYPTO_AES_XTS:
		if (crd->crd_flags & CRD_F_ENCRYPT)
			aesni_xts_enc(ses->ses_xts, buf, buf, crd->crd_len, iv);
		else
			aesni_xts_dec(ses->ses_xts, buf, buf, crd->crd_len, iv);
		break;
	}
	fpu_kernel_exit();

	aesni_ops++;

	/* Copy back the result */
	if (crp->crp_flags & CRYPTO_F_IMBUF) {
		if (m_copyback((struct mbuf *)crp->crp_buf, crd->crd_skip,
		    crd->crd_len, buf, M_NOWAIT)) {
			err = ENOMEM;
			goto out;
		}
	} else
		cuio_copyback((struct uio *)crp->crp_buf, crd->crd_skip,
		    crd->crd_len, buf);

	/* Copy back the authentication tag */
	if (crda) {
		if (crp->crp_flags & CRYPTO_F_IMBUF) {
			if (m_copyback((struct mbuf *)crp->crp_buf,
			    crda->crd_inject, GMAC_DIGEST_LEN, tag,
			    M_NOWAIT)) {
				err = ENOMEM;
				goto out;
			}
		} else
			bcopy(tag, crp->crp_mac, GMAC_BLOCK_LEN);

		/* clean up GHASH state */
		bzero(ses->ses_ghash->S, GMAC_BLOCK_LEN);
		bzero(ses->ses_ghash->Z, GMAC_BLOCK_LEN);
	}

out:
	explicit_bzero(buf, roundup(crd->crd_len, EALG_MAX_BLOCK_LEN));
	return (err);
}

int
aesni_process(struct cryptop *crp)
{
	struct aesni_session *ses;
	struct cryptodesc *crd, *crda, *crde;
	int err = 0;

	if (crp == NULL || crp->crp_callback == NULL)
		return (EINVAL);

	LIST_FOREACH(ses, &aesni_sc->sc_sessions, ses_entries) {
		if (ses->ses_sid == (crp->crp_sid & 0xffffffff))
			break;
	}

	if (!ses) {
		err = EINVAL;
		goto out;
	}

	crda = crde = NULL;
	for (crd = crp->crp_desc; crd; crd = crd->crd_next) {
		switch (crd->crd_alg) {
		case CRYPTO_AES_CBC:
		case CRYPTO_AES_CTR:
		case CRYPTO_AES_XTS:
			err = aesni_encdec(crp, crd, NULL, ses);
			if (err != 0)
				goto out;
			break;

		case CRYPTO_AES_GCM_16:
		case CRYPTO_AES_GMAC:
			crde = crd;
			if (!crda)
				continue;
			goto gcmcommon;
		case CRYPTO_AES_128_GMAC:
		case CRYPTO_AES_192_GMAC:
		case CRYPTO_AES_256_GMAC:
			crda = crd;
			if (!crde)
				continue;
		gcmcommon:
			err = aesni_encdec(crp, crde, crda, ses);
			if (err != 0)
				goto out;
			break;

		case CRYPTO_MD5_HMAC:
		case CRYPTO_SHA1_HMAC:
		case CRYPTO_RIPEMD160_HMAC:
		case CRYPTO_SHA2_256_HMAC:
		case CRYPTO_SHA2_384_HMAC:
		case CRYPTO_SHA2_512_HMAC:
			err = aesni_swauth(crp, crd, ses->ses_swd,
			    crp->crp_buf);
			if (err != 0)
				goto out;
			break;

		default:
			err = EINVAL;
			goto out;
		}
	}

out:
	crp->crp_etype = err;
	crypto_done(crp);
	return (err);
}
