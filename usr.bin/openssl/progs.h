/* $OpenBSD: progs.h,v 1.4 2015/06/20 13:51:52 jsing Exp $ */
/* Public domain */

extern int asn1parse_main(int argc, char *argv[]);
extern int ca_main(int argc, char *argv[]);
extern int certhash_main(int argc, char *argv[]);
extern int ciphers_main(int argc, char *argv[]);
extern int cms_main(int argc, char *argv[]);
extern int crl2pkcs7_main(int argc, char *argv[]);
extern int crl_main(int argc, char *argv[]);
extern int dgst_main(int argc, char *argv[]);
extern int dh_main(int argc, char *argv[]);
extern int dhparam_main(int argc, char *argv[]);
extern int dsa_main(int argc, char *argv[]);
extern int dsaparam_main(int argc, char *argv[]);
extern int ec_main(int argc, char *argv[]);
extern int ecparam_main(int argc, char *argv[]);
extern int enc_main(int argc, char *argv[]);
extern int engine_main(int argc, char *argv[]);
extern int errstr_main(int argc, char *argv[]);
extern int gendh_main(int argc, char *argv[]);
extern int gendsa_main(int argc, char *argv[]);
extern int genpkey_main(int argc, char *argv[]);
extern int genrsa_main(int argc, char *argv[]);
extern int nseq_main(int argc, char *argv[]);
extern int ocsp_main(int argc, char *argv[]);
extern int passwd_main(int argc, char *argv[]);
extern int pkcs7_main(int argc, char *argv[]);
extern int pkcs8_main(int argc, char *argv[]);
extern int pkcs12_main(int argc, char *argv[]);
extern int pkey_main(int argc, char *argv[]);
extern int pkeyparam_main(int argc, char *argv[]);
extern int pkeyutl_main(int argc, char *argv[]);
extern int prime_main(int argc, char *argv[]);
extern int rand_main(int argc, char *argv[]);
extern int req_main(int argc, char *argv[]);
extern int rsa_main(int argc, char *argv[]);
extern int rsautl_main(int argc, char *argv[]);
extern int s_client_main(int argc, char *argv[]);
extern int s_server_main(int argc, char *argv[]);
extern int s_time_main(int argc, char *argv[]);
extern int sess_id_main(int argc, char *argv[]);
extern int smime_main(int argc, char *argv[]);
extern int speed_main(int argc, char *argv[]);
extern int spkac_main(int argc, char *argv[]);
extern int ts_main(int argc, char *argv[]);
extern int verify_main(int argc, char *argv[]);
extern int version_main(int argc, char *argv[]);
extern int x509_main(int argc, char *argv[]);

#define FUNC_TYPE_GENERAL	1
#define FUNC_TYPE_MD		2
#define FUNC_TYPE_CIPHER	3
#define FUNC_TYPE_PKEY		4
#define FUNC_TYPE_MD_ALG	5
#define FUNC_TYPE_CIPHER_ALG	6

typedef struct {
	int type;
	const char *name;
	int (*func)(int argc, char *argv[]);
} FUNCTION;
DECLARE_LHASH_OF(FUNCTION);

FUNCTION functions[] = {

	/* General functions. */
	{ FUNC_TYPE_GENERAL, "asn1parse", asn1parse_main },
	{ FUNC_TYPE_GENERAL, "ca", ca_main },
	{ FUNC_TYPE_GENERAL, "certhash", certhash_main },
	{ FUNC_TYPE_GENERAL, "ciphers", ciphers_main },
#ifndef OPENSSL_NO_CMS
	{ FUNC_TYPE_GENERAL, "cms", cms_main },
#endif
	{ FUNC_TYPE_GENERAL, "crl2pkcs7", crl2pkcs7_main },
	{ FUNC_TYPE_GENERAL, "crl", crl_main },
	{ FUNC_TYPE_GENERAL, "dgst", dgst_main },
	{ FUNC_TYPE_GENERAL, "enc", enc_main },
#ifndef OPENSSL_NO_ENGINE
	{ FUNC_TYPE_GENERAL, "engine", engine_main },
#endif
	{ FUNC_TYPE_GENERAL, "errstr", errstr_main },
	{ FUNC_TYPE_GENERAL, "genpkey", genpkey_main },
	{ FUNC_TYPE_GENERAL, "nseq", nseq_main },
#ifndef OPENSSL_NO_OCSP
	{ FUNC_TYPE_GENERAL, "ocsp", ocsp_main },
#endif
	{ FUNC_TYPE_GENERAL, "passwd", passwd_main },
	{ FUNC_TYPE_GENERAL, "pkcs7", pkcs7_main },
	{ FUNC_TYPE_GENERAL, "pkcs8", pkcs8_main },
#if !defined(OPENSSL_NO_DES) && !defined(OPENSSL_NO_SHA1)
	{ FUNC_TYPE_GENERAL, "pkcs12", pkcs12_main },
#endif
	{ FUNC_TYPE_GENERAL, "pkey", pkey_main },
	{ FUNC_TYPE_GENERAL, "pkeyparam", pkeyparam_main },
	{ FUNC_TYPE_GENERAL, "pkeyutl", pkeyutl_main },
	{ FUNC_TYPE_GENERAL, "prime", prime_main },
	{ FUNC_TYPE_GENERAL, "rand", rand_main },
	{ FUNC_TYPE_GENERAL, "req", req_main },
	{ FUNC_TYPE_GENERAL, "s_client", s_client_main },
	{ FUNC_TYPE_GENERAL, "s_server", s_server_main },
	{ FUNC_TYPE_GENERAL, "s_time", s_time_main },
	{ FUNC_TYPE_GENERAL, "sess_id", sess_id_main },
	{ FUNC_TYPE_GENERAL, "smime", smime_main },
#ifndef OPENSSL_NO_SPEED
	{ FUNC_TYPE_GENERAL, "speed", speed_main },
#endif
	{ FUNC_TYPE_GENERAL, "spkac", spkac_main },
	{ FUNC_TYPE_GENERAL, "ts", ts_main },
	{ FUNC_TYPE_GENERAL, "verify", verify_main },
	{ FUNC_TYPE_GENERAL, "version", version_main },
	{ FUNC_TYPE_GENERAL, "x509", x509_main },

#ifndef OPENSSL_NO_DH
	{ FUNC_TYPE_GENERAL, "dh", dh_main },
	{ FUNC_TYPE_GENERAL, "dhparam", dhparam_main },
	{ FUNC_TYPE_GENERAL, "gendh", gendh_main },
#endif
#ifndef OPENSSL_NO_DSA
	{ FUNC_TYPE_GENERAL, "dsa", dsa_main },
	{ FUNC_TYPE_GENERAL, "dsaparam", dsaparam_main },
	{ FUNC_TYPE_GENERAL, "gendsa", gendsa_main },
#endif
#ifndef OPENSSL_NO_EC
	{ FUNC_TYPE_GENERAL, "ec", ec_main },
	{ FUNC_TYPE_GENERAL, "ecparam", ecparam_main },
#endif
#ifndef OPENSSL_NO_RSA
	{ FUNC_TYPE_GENERAL, "genrsa", genrsa_main },
	{ FUNC_TYPE_GENERAL, "rsa", rsa_main },
	{ FUNC_TYPE_GENERAL, "rsautl", rsautl_main },
#endif

	/* Message Digests. */
#ifndef OPENSSL_NO_GOST
	{ FUNC_TYPE_MD, "gost-mac", dgst_main },
	{ FUNC_TYPE_MD, "md_gost94", dgst_main },
	{ FUNC_TYPE_MD, "streebog256", dgst_main },
	{ FUNC_TYPE_MD, "streebog512", dgst_main },
#endif
#ifndef OPENSSL_NO_MD4
	{ FUNC_TYPE_MD, "md4", dgst_main },
#endif
#ifndef OPENSSL_NO_MD5
	{ FUNC_TYPE_MD, "md5", dgst_main },
#endif
#ifndef OPENSSL_NO_RIPEMD160
	{ FUNC_TYPE_MD, "ripemd160", dgst_main },
#endif
#ifndef OPENSSL_NO_SHA
	{ FUNC_TYPE_MD, "sha", dgst_main },
#endif
#ifndef OPENSSL_NO_SHA1
	{ FUNC_TYPE_MD, "sha1", dgst_main },
#endif
#ifndef OPENSSL_NO_SHA224
	{ FUNC_TYPE_MD, "sha224", dgst_main },
#endif
#ifndef OPENSSL_NO_SHA256
	{ FUNC_TYPE_MD, "sha256", dgst_main },
#endif
#ifndef OPENSSL_NO_SHA384
	{ FUNC_TYPE_MD, "sha384", dgst_main },
#endif
#ifndef OPENSSL_NO_SHA512
	{ FUNC_TYPE_MD, "sha512", dgst_main },
#endif
#ifndef OPENSSL_NO_WHIRLPOOL
	{ FUNC_TYPE_MD, "whirlpool", dgst_main },
#endif

	/* Ciphers. */
	{ FUNC_TYPE_CIPHER, "base64", enc_main },
#ifndef OPENSSL_NO_AES
	{ FUNC_TYPE_CIPHER, "aes-128-cbc", enc_main },
	{ FUNC_TYPE_CIPHER, "aes-128-ecb", enc_main },
	{ FUNC_TYPE_CIPHER, "aes-192-cbc", enc_main },
	{ FUNC_TYPE_CIPHER, "aes-192-ecb", enc_main },
	{ FUNC_TYPE_CIPHER, "aes-256-cbc", enc_main },
	{ FUNC_TYPE_CIPHER, "aes-256-ecb", enc_main },
#endif
#ifndef OPENSSL_NO_BF
	{ FUNC_TYPE_CIPHER, "bf", enc_main },
	{ FUNC_TYPE_CIPHER, "bf-cbc", enc_main },
	{ FUNC_TYPE_CIPHER, "bf-ecb", enc_main },
	{ FUNC_TYPE_CIPHER, "bf-cfb", enc_main },
	{ FUNC_TYPE_CIPHER, "bf-ofb", enc_main },
#endif
#ifndef OPENSSL_NO_CAMELLIA
	{ FUNC_TYPE_CIPHER, "camellia-128-cbc", enc_main },
	{ FUNC_TYPE_CIPHER, "camellia-128-ecb", enc_main },
	{ FUNC_TYPE_CIPHER, "camellia-192-cbc", enc_main },
	{ FUNC_TYPE_CIPHER, "camellia-192-ecb", enc_main },
	{ FUNC_TYPE_CIPHER, "camellia-256-cbc", enc_main },
	{ FUNC_TYPE_CIPHER, "camellia-256-ecb", enc_main },
#endif
#ifndef OPENSSL_NO_CAST
	{ FUNC_TYPE_CIPHER, "cast", enc_main },
	{ FUNC_TYPE_CIPHER, "cast5-cbc", enc_main },
	{ FUNC_TYPE_CIPHER, "cast5-ecb", enc_main },
	{ FUNC_TYPE_CIPHER, "cast5-cfb", enc_main },
	{ FUNC_TYPE_CIPHER, "cast5-ofb", enc_main },
	{ FUNC_TYPE_CIPHER, "cast-cbc", enc_main },
#endif
#ifndef OPENSSL_NO_CHACHA
	{ FUNC_TYPE_CIPHER, "chacha", enc_main },
#endif
#ifndef OPENSSL_NO_DES
	{ FUNC_TYPE_CIPHER, "des", enc_main },
	{ FUNC_TYPE_CIPHER, "des3", enc_main },
	{ FUNC_TYPE_CIPHER, "desx", enc_main },
	{ FUNC_TYPE_CIPHER, "des-ecb", enc_main },
	{ FUNC_TYPE_CIPHER, "des-ede", enc_main },
	{ FUNC_TYPE_CIPHER, "des-ede3", enc_main },
	{ FUNC_TYPE_CIPHER, "des-cbc", enc_main },
	{ FUNC_TYPE_CIPHER, "des-ede-cbc", enc_main },
	{ FUNC_TYPE_CIPHER, "des-ede3-cbc", enc_main },
	{ FUNC_TYPE_CIPHER, "des-cfb", enc_main },
	{ FUNC_TYPE_CIPHER, "des-ede-cfb", enc_main },
	{ FUNC_TYPE_CIPHER, "des-ede3-cfb", enc_main },
	{ FUNC_TYPE_CIPHER, "des-ofb", enc_main },
	{ FUNC_TYPE_CIPHER, "des-ede-ofb", enc_main },
	{ FUNC_TYPE_CIPHER, "des-ede3-ofb", enc_main },
#endif
#ifndef OPENSSL_NO_IDEA
	{ FUNC_TYPE_CIPHER, "idea", enc_main },
	{ FUNC_TYPE_CIPHER, "idea-cbc", enc_main },
	{ FUNC_TYPE_CIPHER, "idea-ecb", enc_main },
	{ FUNC_TYPE_CIPHER, "idea-cfb", enc_main },
	{ FUNC_TYPE_CIPHER, "idea-ofb", enc_main },
#endif
#ifndef OPENSSL_NO_RC2
	{ FUNC_TYPE_CIPHER, "rc2", enc_main },
	{ FUNC_TYPE_CIPHER, "rc2-cbc", enc_main },
	{ FUNC_TYPE_CIPHER, "rc2-ecb", enc_main },
	{ FUNC_TYPE_CIPHER, "rc2-cfb", enc_main },
	{ FUNC_TYPE_CIPHER, "rc2-ofb", enc_main },
	{ FUNC_TYPE_CIPHER, "rc2-64-cbc", enc_main },
	{ FUNC_TYPE_CIPHER, "rc2-40-cbc", enc_main },
#endif
#ifndef OPENSSL_NO_RC4
	{ FUNC_TYPE_CIPHER, "rc4", enc_main },
	{ FUNC_TYPE_CIPHER, "rc4-40", enc_main },
#endif
#ifndef OPENSSL_NO_RC5
	{ FUNC_TYPE_CIPHER, "rc5", enc_main },
	{ FUNC_TYPE_CIPHER, "rc5-cbc", enc_main },
	{ FUNC_TYPE_CIPHER, "rc5-ecb", enc_main },
	{ FUNC_TYPE_CIPHER, "rc5-cfb", enc_main },
	{ FUNC_TYPE_CIPHER, "rc5-ofb", enc_main },
#endif
#ifdef ZLIB
	{ FUNC_TYPE_CIPHER, "zlib", enc_main },
#endif

	{ 0, NULL, NULL }
};
