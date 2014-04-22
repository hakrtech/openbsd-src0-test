/* NOCW */
/* used by apps/speed.c */

DSA *get_dsa512(void);
DSA *get_dsa1024(void);
DSA *get_dsa2048(void);

static unsigned char dsa512_priv[] = {
	0x65, 0xe5, 0xc7, 0x38, 0x60, 0x24, 0xb5, 0x89, 0xd4, 0x9c, 0xeb, 0x4c,
	0x9c, 0x1d, 0x7a, 0x22, 0xbd, 0xd1, 0xc2, 0xd2,
};
static unsigned char dsa512_pub[] = {
	0x00, 0x95, 0xa7, 0x0d, 0xec, 0x93, 0x68, 0xba, 0x5f, 0xf7, 0x5f, 0x07,
	0xf2, 0x3b, 0xad, 0x6b, 0x01, 0xdc, 0xbe, 0xec, 0xde, 0x04, 0x7a, 0x3a,
	0x27, 0xb3, 0xec, 0x49, 0xfd, 0x08, 0x43, 0x3d, 0x7e, 0xa8, 0x2c, 0x5e,
	0x7b, 0xbb, 0xfc, 0xf4, 0x6e, 0xeb, 0x6c, 0xb0, 0x6e, 0xf8, 0x02, 0x12,
	0x8c, 0x38, 0x5d, 0x83, 0x56, 0x7d, 0xee, 0x53, 0x05, 0x3e, 0x24, 0x84,
	0xbe, 0xba, 0x0a, 0x6b, 0xc8,
};
static unsigned char dsa512_p[] = {
	0x9D, 0x1B, 0x69, 0x8E, 0x26, 0xDB, 0xF2, 0x2B, 0x11, 0x70, 0x19, 0x86,
	0xF6, 0x19, 0xC8, 0xF8, 0x19, 0xF2, 0x18, 0x53, 0x94, 0x46, 0x06, 0xD0,
	0x62, 0x50, 0x33, 0x4B, 0x02, 0x3C, 0x52, 0x30, 0x03, 0x8B, 0x3B, 0xF9,
	0x5F, 0xD1, 0x24, 0x06, 0x4F, 0x7B, 0x4C, 0xBA, 0xAA, 0x40, 0x9B, 0xFD,
	0x96, 0xE4, 0x37, 0x33, 0xBB, 0x2D, 0x5A, 0xD7, 0x5A, 0x11, 0x40, 0x66,
	0xA2, 0x76, 0x7D, 0x31,
};
static unsigned char dsa512_q[] = {
	0xFB, 0x53, 0xEF, 0x50, 0xB4, 0x40, 0x92, 0x31, 0x56, 0x86, 0x53, 0x7A,
	0xE8, 0x8B, 0x22, 0x9A, 0x49, 0xFB, 0x71, 0x8F,
};
static unsigned char dsa512_g[] = {
	0x83, 0x3E, 0x88, 0xE5, 0xC5, 0x89, 0x73, 0xCE, 0x3B, 0x6C, 0x01, 0x49,
	0xBF, 0xB3, 0xC7, 0x9F, 0x0A, 0xEA, 0x44, 0x91, 0xE5, 0x30, 0xAA, 0xD9,
	0xBE, 0x5B, 0x5F, 0xB7, 0x10, 0xD7, 0x89, 0xB7, 0x8E, 0x74, 0xFB, 0xCF,
	0x29, 0x1E, 0xEB, 0xA8, 0x2C, 0x54, 0x51, 0xB8, 0x10, 0xDE, 0xA0, 0xCE,
	0x2F, 0xCC, 0x24, 0x6B, 0x90, 0x77, 0xDE, 0xA2, 0x68, 0xA6, 0x52, 0x12,
	0xA2, 0x03, 0x9D, 0x20,
};

DSA *
get_dsa512()
{
	DSA *dsa;

	if ((dsa = DSA_new()) == NULL)
		return (NULL);
	dsa->priv_key = BN_bin2bn(dsa512_priv, sizeof(dsa512_priv), NULL);
	dsa->pub_key = BN_bin2bn(dsa512_pub, sizeof(dsa512_pub), NULL);
	dsa->p = BN_bin2bn(dsa512_p, sizeof(dsa512_p), NULL);
	dsa->q = BN_bin2bn(dsa512_q, sizeof(dsa512_q), NULL);
	dsa->g = BN_bin2bn(dsa512_g, sizeof(dsa512_g), NULL);
	if ((dsa->priv_key == NULL) || (dsa->pub_key == NULL) ||
	    (dsa->p == NULL) || (dsa->q == NULL) || (dsa->g == NULL))
		return (NULL);
	return (dsa);
}

static unsigned char dsa1024_priv[] = {
	0x7d, 0x21, 0xda, 0xbb, 0x62, 0x15, 0x47, 0x36, 0x07, 0x67, 0x12, 0xe8,
	0x8c, 0xaa, 0x1c, 0xcd, 0x38, 0x12, 0x61, 0x18,
};
static unsigned char dsa1024_pub[] = {
	0x3c, 0x4e, 0x9c, 0x2a, 0x7f, 0x16, 0xc1, 0x25, 0xeb, 0xac, 0x78, 0x63,
	0x90, 0x14, 0x8c, 0x8b, 0xf4, 0x68, 0x43, 0x3c, 0x2d, 0xee, 0x65, 0x50,
	0x7d, 0x9c, 0x8f, 0x8c, 0x8a, 0x51, 0xd6, 0x11, 0x2b, 0x99, 0xaf, 0x1e,
	0x90, 0x97, 0xb5, 0xd3, 0xa6, 0x20, 0x25, 0xd6, 0xfe, 0x43, 0x02, 0xd5,
	0x91, 0x7d, 0xa7, 0x8c, 0xdb, 0xc9, 0x85, 0xa3, 0x36, 0x48, 0xf7, 0x68,
	0xaa, 0x60, 0xb1, 0xf7, 0x05, 0x68, 0x3a, 0xa3, 0x3f, 0xd3, 0x19, 0x82,
	0xd8, 0x82, 0x7a, 0x77, 0xfb, 0xef, 0xf4, 0x15, 0x0a, 0xeb, 0x06, 0x04,
	0x7f, 0x53, 0x07, 0x0c, 0xbc, 0xcb, 0x2d, 0x83, 0xdb, 0x3e, 0xd1, 0x28,
	0xa5, 0xa1, 0x31, 0xe0, 0x67, 0xfa, 0x50, 0xde, 0x9b, 0x07, 0x83, 0x7e,
	0x2c, 0x0b, 0xc3, 0x13, 0x50, 0x61, 0xe5, 0xad, 0xbd, 0x36, 0xb8, 0x97,
	0x4e, 0x40, 0x7d, 0xe8, 0x83, 0x0d, 0xbc, 0x4b
};
static unsigned char dsa1024_p[] = {
	0xA7, 0x3F, 0x6E, 0x85, 0xBF, 0x41, 0x6A, 0x29, 0x7D, 0xF0, 0x9F, 0x47,
	0x19, 0x30, 0x90, 0x9A, 0x09, 0x1D, 0xDA, 0x6A, 0x33, 0x1E, 0xC5, 0x3D,
	0x86, 0x96, 0xB3, 0x15, 0xE0, 0x53, 0x2E, 0x8F, 0xE0, 0x59, 0x82, 0x73,
	0x90, 0x3E, 0x75, 0x31, 0x99, 0x47, 0x7A, 0x52, 0xFB, 0x85, 0xE4, 0xD9,
	0xA6, 0x7B, 0x38, 0x9B, 0x68, 0x8A, 0x84, 0x9B, 0x87, 0xC6, 0x1E, 0xB5,
	0x7E, 0x86, 0x4B, 0x53, 0x5B, 0x59, 0xCF, 0x71, 0x65, 0x19, 0x88, 0x6E,
	0xCE, 0x66, 0xAE, 0x6B, 0x88, 0x36, 0xFB, 0xEC, 0x28, 0xDC, 0xC2, 0xD7,
	0xA5, 0xBB, 0xE5, 0x2C, 0x39, 0x26, 0x4B, 0xDA, 0x9A, 0x70, 0x18, 0x95,
	0x37, 0x95, 0x10, 0x56, 0x23, 0xF6, 0x15, 0xED, 0xBA, 0x04, 0x5E, 0xDE,
	0x39, 0x4F, 0xFD, 0xB7, 0x43, 0x1F, 0xB5, 0xA4, 0x65, 0x6F, 0xCD, 0x80,
	0x11, 0xE4, 0x70, 0x95, 0x5B, 0x50, 0xCD, 0x49,
};
static unsigned char dsa1024_q[] = {
	0xF7, 0x07, 0x31, 0xED, 0xFA, 0x6C, 0x06, 0x03, 0xD5, 0x85, 0x8A, 0x1C,
	0xAC, 0x9C, 0x65, 0xE7, 0x50, 0x66, 0x65, 0x6F,
};
static unsigned char dsa1024_g[] = {
	0x4D, 0xDF, 0x4C, 0x03, 0xA6, 0x91, 0x8A, 0xF5, 0x19, 0x6F, 0x50, 0x46,
	0x25, 0x99, 0xE5, 0x68, 0x6F, 0x30, 0xE3, 0x69, 0xE1, 0xE5, 0xB3, 0x5D,
	0x98, 0xBB, 0x28, 0x86, 0x48, 0xFC, 0xDE, 0x99, 0x04, 0x3F, 0x5F, 0x88,
	0x0C, 0x9C, 0x73, 0x24, 0x0D, 0x20, 0x5D, 0xB9, 0x2A, 0x9A, 0x3F, 0x18,
	0x96, 0x27, 0xE4, 0x62, 0x87, 0xC1, 0x7B, 0x74, 0x62, 0x53, 0xFC, 0x61,
	0x27, 0xA8, 0x7A, 0x91, 0x09, 0x9D, 0xB6, 0xF1, 0x4D, 0x9C, 0x54, 0x0F,
	0x58, 0x06, 0xEE, 0x49, 0x74, 0x07, 0xCE, 0x55, 0x7E, 0x23, 0xCE, 0x16,
	0xF6, 0xCA, 0xDC, 0x5A, 0x61, 0x01, 0x7E, 0xC9, 0x71, 0xB5, 0x4D, 0xF6,
	0xDC, 0x34, 0x29, 0x87, 0x68, 0xF6, 0x5E, 0x20, 0x93, 0xB3, 0xDB, 0xF5,
	0xE4, 0x09, 0x6C, 0x41, 0x17, 0x95, 0x92, 0xEB, 0x01, 0xB5, 0x73, 0xA5,
	0x6A, 0x7E, 0xD8, 0x32, 0xED, 0x0E, 0x02, 0xB8,
};

DSA *
get_dsa1024()
{
	DSA *dsa;

	if ((dsa = DSA_new()) == NULL)
		return (NULL);
	dsa->priv_key = BN_bin2bn(dsa1024_priv, sizeof(dsa1024_priv), NULL);
	dsa->pub_key = BN_bin2bn(dsa1024_pub, sizeof(dsa1024_pub), NULL);
	dsa->p = BN_bin2bn(dsa1024_p, sizeof(dsa1024_p), NULL);
	dsa->q = BN_bin2bn(dsa1024_q, sizeof(dsa1024_q), NULL);
	dsa->g = BN_bin2bn(dsa1024_g, sizeof(dsa1024_g), NULL);
	if ((dsa->priv_key == NULL) || (dsa->pub_key == NULL) ||
	    (dsa->p == NULL) || (dsa->q == NULL) || (dsa->g == NULL))
		return (NULL);
	return (dsa);
}

static unsigned char dsa2048_priv[] = {
	0x32, 0x67, 0x92, 0xf6, 0xc4, 0xe2, 0xe2, 0xe8, 0xa0, 0x8b, 0x6b, 0x45,
	0x0c, 0x8a, 0x76, 0xb0, 0xee, 0xcf, 0x91, 0xa7,
};
static unsigned char dsa2048_pub[] = {
	0x17, 0x8f, 0xa8, 0x11, 0x84, 0x92, 0xec, 0x83, 0x47, 0xc7, 0x6a, 0xb0,
	0x92, 0xaf, 0x5a, 0x20, 0x37, 0xa3, 0x64, 0x79, 0xd2, 0xd0, 0x3d, 0xcd,
	0xe0, 0x61, 0x88, 0x88, 0x21, 0xcc, 0x74, 0x5d, 0xce, 0x4c, 0x51, 0x47,
	0xf0, 0xc5, 0x5c, 0x4c, 0x82, 0x7a, 0xaf, 0x72, 0xad, 0xb9, 0xe0, 0x53,
	0xf2, 0x78, 0xb7, 0xf0, 0xb5, 0x48, 0x7f, 0x8a, 0x3a, 0x18, 0xd1, 0x9f,
	0x8b, 0x7d, 0xa5, 0x47, 0xb7, 0x95, 0xab, 0x98, 0xf8, 0x7b, 0x74, 0x50,
	0x56, 0x8e, 0x57, 0xf0, 0xee, 0xf5, 0xb7, 0xba, 0xab, 0x85, 0x86, 0xf9,
	0x2b, 0xef, 0x41, 0x56, 0xa0, 0xa4, 0x9f, 0xb7, 0x38, 0x00, 0x46, 0x0a,
	0xa6, 0xf1, 0xfc, 0x1f, 0xd8, 0x4e, 0x85, 0x44, 0x92, 0x43, 0x21, 0x5d,
	0x6e, 0xcc, 0xc2, 0xcb, 0x26, 0x31, 0x0d, 0x21, 0xc4, 0xbd, 0x8d, 0x24,
	0xbc, 0xd9, 0x18, 0x19, 0xd7, 0xdc, 0xf1, 0xe7, 0x93, 0x50, 0x48, 0x03,
	0x2c, 0xae, 0x2e, 0xe7, 0x49, 0x88, 0x5f, 0x93, 0x57, 0x27, 0x99, 0x36,
	0xb4, 0x20, 0xab, 0xfc, 0xa7, 0x2b, 0xf2, 0xd9, 0x98, 0xd7, 0xd4, 0x34,
	0x9d, 0x96, 0x50, 0x58, 0x9a, 0xea, 0x54, 0xf3, 0xee, 0xf5, 0x63, 0x14,
	0xee, 0x85, 0x83, 0x74, 0x76, 0xe1, 0x52, 0x95, 0xc3, 0xf7, 0xeb, 0x04,
	0x04, 0x7b, 0xa7, 0x28, 0x1b, 0xcc, 0xea, 0x4a, 0x4e, 0x84, 0xda, 0xd8,
	0x9c, 0x79, 0xd8, 0x9b, 0x66, 0x89, 0x2f, 0xcf, 0xac, 0xd7, 0x79, 0xf9,
	0xa9, 0xd8, 0x45, 0x13, 0x78, 0xb9, 0x00, 0x14, 0xc9, 0x7e, 0x22, 0x51,
	0x86, 0x67, 0xb0, 0x9f, 0x26, 0x11, 0x23, 0xc8, 0x38, 0xd7, 0x70, 0x1d,
	0x15, 0x8e, 0x4d, 0x4f, 0x95, 0x97, 0x40, 0xa1, 0xc2, 0x7e, 0x01, 0x18,
	0x72, 0xf4, 0x10, 0xe6, 0x8d, 0x52, 0x16, 0x7f, 0xf2, 0xc9, 0xf8, 0x33,
	0x8b, 0x33, 0xb7, 0xce,
};
static unsigned char dsa2048_p[] = {
	0xA0, 0x25, 0xFA, 0xAD, 0xF4, 0x8E, 0xB9, 0xE5, 0x99, 0xF3, 0x5D, 0x6F,
	0x4F, 0x83, 0x34, 0xE2, 0x7E, 0xCF, 0x6F, 0xBF, 0x30, 0xAF, 0x6F, 0x81,
	0xEB, 0xF8, 0xC4, 0x13, 0xD9, 0xA0, 0x5D, 0x8B, 0x5C, 0x8E, 0xDC, 0xC2,
	0x1D, 0x0B, 0x41, 0x32, 0xB0, 0x1F, 0xFE, 0xEF, 0x0C, 0xC2, 0xA2, 0x7E,
	0x68, 0x5C, 0x28, 0x21, 0xE9, 0xF5, 0xB1, 0x58, 0x12, 0x63, 0x4C, 0x19,
	0x4E, 0xFF, 0x02, 0x4B, 0x92, 0xED, 0xD2, 0x07, 0x11, 0x4D, 0x8C, 0x58,
	0x16, 0x5C, 0x55, 0x8E, 0xAD, 0xA3, 0x67, 0x7D, 0xB9, 0x86, 0x6E, 0x0B,
	0xE6, 0x54, 0x6F, 0x40, 0xAE, 0x0E, 0x67, 0x4C, 0xF9, 0x12, 0x5B, 0x3C,
	0x08, 0x7A, 0xF7, 0xFC, 0x67, 0x86, 0x69, 0xE7, 0x0A, 0x94, 0x40, 0xBF,
	0x8B, 0x76, 0xFE, 0x26, 0xD1, 0xF2, 0xA1, 0x1A, 0x84, 0xA1, 0x43, 0x56,
	0x28, 0xBC, 0x9A, 0x5F, 0xD7, 0x3B, 0x69, 0x89, 0x8A, 0x36, 0x2C, 0x51,
	0xDF, 0x12, 0x77, 0x2F, 0x57, 0x7B, 0xA0, 0xAA, 0xDD, 0x7F, 0xA1, 0x62,
	0x3B, 0x40, 0x7B, 0x68, 0x1A, 0x8F, 0x0D, 0x38, 0xBB, 0x21, 0x5D, 0x18,
	0xFC, 0x0F, 0x46, 0xF7, 0xA3, 0xB0, 0x1D, 0x23, 0xC3, 0xD2, 0xC7, 0x72,
	0x51, 0x18, 0xDF, 0x46, 0x95, 0x79, 0xD9, 0xBD, 0xB5, 0x19, 0x02, 0x2C,
	0x87, 0xDC, 0xE7, 0x57, 0x82, 0x7E, 0xF1, 0x8B, 0x06, 0x3D, 0x00, 0xA5,
	0x7B, 0x6B, 0x26, 0x27, 0x91, 0x0F, 0x6A, 0x77, 0xE4, 0xD5, 0x04, 0xE4,
	0x12, 0x2C, 0x42, 0xFF, 0xD2, 0x88, 0xBB, 0xD3, 0x92, 0xA0, 0xF9, 0xC8,
	0x51, 0x64, 0x14, 0x5C, 0xD8, 0xF9, 0x6C, 0x47, 0x82, 0xB4, 0x1C, 0x7F,
	0x09, 0xB8, 0xF0, 0x25, 0x83, 0x1D, 0x3F, 0x3F, 0x05, 0xB3, 0x21, 0x0A,
	0x5D, 0xA7, 0xD8, 0x54, 0xC3, 0x65, 0x7D, 0xC3, 0xB0, 0x1D, 0xBF, 0xAE,
	0xF8, 0x68, 0xCF, 0x9B,
};
static unsigned char dsa2048_q[] = {
	0x97, 0xE7, 0x33, 0x4D, 0xD3, 0x94, 0x3E, 0x0B, 0xDB, 0x62, 0x74, 0xC6,
	0xA1, 0x08, 0xDD, 0x19, 0xA3, 0x75, 0x17, 0x1B,
};
static unsigned char dsa2048_g[] = {
	0x2C, 0x78, 0x16, 0x59, 0x34, 0x63, 0xF4, 0xF3, 0x92, 0xFC, 0xB5, 0xA5,
	0x4F, 0x13, 0xDE, 0x2F, 0x1C, 0xA4, 0x3C, 0xAE, 0xAD, 0x38, 0x3F, 0x7E,
	0x90, 0xBF, 0x96, 0xA6, 0xAE, 0x25, 0x90, 0x72, 0xF5, 0x8E, 0x80, 0x0C,
	0x39, 0x1C, 0xD9, 0xEC, 0xBA, 0x90, 0x5B, 0x3A, 0xE8, 0x58, 0x6C, 0x9E,
	0x30, 0x42, 0x37, 0x02, 0x31, 0x82, 0xBC, 0x6A, 0xDF, 0x6A, 0x09, 0x29,
	0xE3, 0xC0, 0x46, 0xD1, 0xCB, 0x85, 0xEC, 0x0C, 0x30, 0x5E, 0xEA, 0xC8,
	0x39, 0x8E, 0x22, 0x9F, 0x22, 0x10, 0xD2, 0x34, 0x61, 0x68, 0x37, 0x3D,
	0x2E, 0x4A, 0x5B, 0x9A, 0xF5, 0xC1, 0x48, 0xC6, 0xF6, 0xDC, 0x63, 0x1A,
	0xD3, 0x96, 0x64, 0xBA, 0x34, 0xC9, 0xD1, 0xA0, 0xD1, 0xAE, 0x6C, 0x2F,
	0x48, 0x17, 0x93, 0x14, 0x43, 0xED, 0xF0, 0x21, 0x30, 0x19, 0xC3, 0x1B,
	0x5F, 0xDE, 0xA3, 0xF0, 0x70, 0x78, 0x18, 0xE1, 0xA8, 0xE4, 0xEE, 0x2E,
	0x00, 0xA5, 0xE4, 0xB3, 0x17, 0xC8, 0x0C, 0x7D, 0x6E, 0x42, 0xDC, 0xB7,
	0x46, 0x00, 0x36, 0x4D, 0xD4, 0x46, 0xAA, 0x3D, 0x3C, 0x46, 0x89, 0x40,
	0xBF, 0x1D, 0x84, 0x77, 0x0A, 0x75, 0xF3, 0x87, 0x1D, 0x08, 0x4C, 0xA6,
	0xD1, 0xA9, 0x1C, 0x1E, 0x12, 0x1E, 0xE1, 0xC7, 0x30, 0x28, 0x76, 0xA5,
	0x7F, 0x6C, 0x85, 0x96, 0x2B, 0x6F, 0xDB, 0x80, 0x66, 0x26, 0xAE, 0xF5,
	0x93, 0xC7, 0x8E, 0xAE, 0x9A, 0xED, 0xE4, 0xCA, 0x04, 0xEA, 0x3B, 0x72,
	0xEF, 0xDC, 0x87, 0xED, 0x0D, 0xA5, 0x4C, 0x4A, 0xDD, 0x71, 0x22, 0x64,
	0x59, 0x69, 0x4E, 0x8E, 0xBF, 0x43, 0xDC, 0xAB, 0x8E, 0x66, 0xBB, 0x01,
	0xB6, 0xF4, 0xE7, 0xFD, 0xD2, 0xAD, 0x9F, 0x36, 0xC1, 0xA0, 0x29, 0x99,
	0xD1, 0x96, 0x70, 0x59, 0x06, 0x78, 0x35, 0xBD, 0x65, 0x55, 0x52, 0x9E,
	0xF8, 0xB2, 0xE5, 0x38,
};

DSA *
get_dsa2048()
{
	DSA *dsa;

	if ((dsa = DSA_new()) == NULL)
		return (NULL);
	dsa->priv_key = BN_bin2bn(dsa2048_priv, sizeof(dsa2048_priv), NULL);
	dsa->pub_key = BN_bin2bn(dsa2048_pub, sizeof(dsa2048_pub), NULL);
	dsa->p = BN_bin2bn(dsa2048_p, sizeof(dsa2048_p), NULL);
	dsa->q = BN_bin2bn(dsa2048_q, sizeof(dsa2048_q), NULL);
	dsa->g = BN_bin2bn(dsa2048_g, sizeof(dsa2048_g), NULL);
	if ((dsa->priv_key == NULL) || (dsa->pub_key == NULL) ||
	    (dsa->p == NULL) || (dsa->q == NULL) || (dsa->g == NULL))
		return (NULL);
	return (dsa);
}
