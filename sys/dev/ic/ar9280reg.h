/*	$OpenBSD: ar9280reg.h,v 1.7 2016/01/05 18:41:15 stsp Exp $	*/

/*-
 * Copyright (c) 2009 Damien Bergamini <damien.bergamini@free.fr>
 * Copyright (c) 2008-2009 Atheros Communications Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
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

#define AR9280_MAX_CHAINS	2

#define AR9280_PD_GAIN_BOUNDARY_DEFAULT	56
#define AR9280_PHY_CCA_MAX_GOOD_VALUE	(-112)

#define AR9280_PHY_SYNTH_CONTROL	0x9874

/* Bits for AR9280_PHY_SYNTH_CONTROL. */
#define AR9280_BMODE		0x20000000
#define AR9280_FRACMODE		0x10000000
#define AR9280_AMODE_REFSEL_M	0x0c000000
#define AR9280_AMODE_REFSEL_S	26

/*
 * NB: The AR9280 uses the same ROM layout than the AR5416.
 */

/* Macro to "pack" registers to 16-bit to save some .rodata space. */
#define P(x)	(x)

/*
 * AR9280 2.0 initialization values.
 */
static const uint16_t ar9280_2_0_regs[] = {
	P(0x01030), P(0x01070), P(0x010b0), P(0x010f0), P(0x08014),
	P(0x0801c), P(0x08120), P(0x081d0), P(0x08318), P(0x09804),
	P(0x09820), P(0x09824), P(0x09828), P(0x09834), P(0x09838),
	P(0x09840), P(0x09844), P(0x09850), P(0x09858), P(0x0985c),
	P(0x09860), P(0x09864), P(0x09868), P(0x0986c), P(0x09914),
	P(0x09918), P(0x09924), P(0x09944), P(0x09960), P(0x0a960),
	P(0x09964), P(0x0c968), P(0x099b8), P(0x099bc), P(0x099c0),
	P(0x0a204), P(0x0a20c), P(0x0b20c), P(0x0a21c), P(0x0a230),
	P(0x0a23c), P(0x0a250), P(0x0a358), P(0x0a388), P(0x0a3d8),
	P(0x07894)
};

static const uint32_t ar9280_2_0_vals_5g20[] = {
	0x00000230, 0x00000168, 0x00000e60, 0x00000000, 0x03e803e8,
	0x128d8027, 0x08f04800, 0x00003210, 0x00003e80, 0x00000300,
	0x02020200, 0x01000e0e, 0x0a020001, 0x00000e0e, 0x00000007,
	0x206a022e, 0x0372161e, 0x6c4000e2, 0x7ec88d2e, 0x31395d5e,
	0x00048d18, 0x0001ce00, 0x5ac640d0, 0x06903081, 0x000007d0,
	0x0000000a, 0xd00a8a0b, 0xffbc1010, 0x00000010, 0x00000010,
	0x00000210, 0x000003b5, 0x0000001c, 0x00000a00, 0x05eea6d4,
	0x00000444, 0x00000014, 0x00000014, 0x1883800a, 0x00000000,
	0x13c88000, 0x001ff000, 0x7999aa02, 0x0c000000, 0x00000000,
	0x5a508000
};

static const uint32_t ar9280_2_0_vals_5g40[] = {
	0x00000460, 0x000002d0, 0x00001cc0, 0x00000000, 0x07d007d0,
	0x128d804f, 0x08f04800, 0x00003210, 0x00007d00, 0x000003c4,
	0x02020200, 0x01000e0e, 0x0a020001, 0x00000e0e, 0x00000007,
	0x206a022e, 0x0372161e, 0x6d4000e2, 0x7ec88d2e, 0x3139605e,
	0x00048d18, 0x0001ce00, 0x5ac640d0, 0x06903081, 0x00000fa0,
	0x00000014, 0xd00a8a0b, 0xffbc1010, 0x00000010, 0x00000010,
	0x00000210, 0x000003b5, 0x0000001c, 0x00000a00, 0x05eea6d4,
	0x00000444, 0x00000014, 0x00000014, 0x1883800a, 0x00000000,
	0x13c88000, 0x001ff000, 0x7999aa02, 0x0c000000, 0x00000000,
	0x5a508000
};

static const uint32_t ar9280_2_0_vals_2g40[] = {
	0x000002c0, 0x00000318, 0x00007c70, 0x00000000, 0x10801600,
	0x12e00057, 0x08f04810, 0x0000320a, 0x00006880, 0x000003c4,
	0x02020200, 0x01000e0e, 0x0a020001, 0x00000e0e, 0x00000007,
	0x206a012e, 0x037216a0, 0x6d4000e2, 0x7ec84d2e, 0x3139605e,
	0x00048d20, 0x0001ce00, 0x5ac640d0, 0x06903881, 0x00001130,
	0x00000268, 0xd00a8a0d, 0xffbc1010, 0x00000010, 0x00000010,
	0x00000210, 0x000003ce, 0x0000001c, 0x00000c00, 0x05eea6d4,
	0x00000444, 0x0001f019, 0x0001f019, 0x1883800a, 0x00000210,
	0x13c88001, 0x0004a000, 0x7999aa0e, 0x08000000, 0x00000000,
	0x5a508000
};

static const uint32_t ar9280_2_0_vals_2g20[] = {
	0x00000160, 0x0000018c, 0x00003e38, 0x00000000, 0x08400b00,
	0x12e0002b, 0x08f04810, 0x0000320a, 0x00003440, 0x00000300,
	0x02020200, 0x01000e0e, 0x0a020001, 0x00000e0e, 0x00000007,
	0x206a012e, 0x037216a0, 0x6c4000e2, 0x7ec84d2e, 0x31395d5e,
	0x00048d20, 0x0001ce00, 0x5ac640d0, 0x06903881, 0x00000898,
	0x0000000b, 0xd00a8a0d, 0xffbc1010, 0x00000010, 0x00000010,
	0x00000210, 0x000003ce, 0x0000001c, 0x00000c00, 0x05eea6d4,
	0x00000444, 0x0001f019, 0x0001f019, 0x1883800a, 0x00000108,
	0x13c88000, 0x0004a000, 0x7999aa0e, 0x0c000000, 0x00000000,
	0x5a508000
};

static const uint16_t ar9280_2_0_cm_regs[] = {
	P(0x0000c), P(0x00030), P(0x00034), P(0x00040), P(0x00044),
	P(0x00048), P(0x0004c), P(0x00050), P(0x00054), P(0x00800),
	P(0x00804), P(0x00808), P(0x0080c), P(0x00810), P(0x00814),
	P(0x00818), P(0x0081c), P(0x00820), P(0x00824), P(0x01040),
	P(0x01044), P(0x01048), P(0x0104c), P(0x01050), P(0x01054),
	P(0x01058), P(0x0105c), P(0x01060), P(0x01064), P(0x01230),
	P(0x01270), P(0x01038), P(0x01078), P(0x010b8), P(0x010f8),
	P(0x01138), P(0x01178), P(0x011b8), P(0x011f8), P(0x01238),
	P(0x01278), P(0x012b8), P(0x012f8), P(0x01338), P(0x01378),
	P(0x013b8), P(0x013f8), P(0x01438), P(0x01478), P(0x014b8),
	P(0x014f8), P(0x01538), P(0x01578), P(0x015b8), P(0x015f8),
	P(0x01638), P(0x01678), P(0x016b8), P(0x016f8), P(0x01738),
	P(0x01778), P(0x017b8), P(0x017f8), P(0x0103c), P(0x0107c),
	P(0x010bc), P(0x010fc), P(0x0113c), P(0x0117c), P(0x011bc),
	P(0x011fc), P(0x0123c), P(0x0127c), P(0x012bc), P(0x012fc),
	P(0x0133c), P(0x0137c), P(0x013bc), P(0x013fc), P(0x0143c),
	P(0x0147c), P(0x04030), P(0x0403c), P(0x04024), P(0x04060),
	P(0x04064), P(0x07010), P(0x07034), P(0x07038), P(0x08004),
	P(0x08008), P(0x0800c), P(0x08018), P(0x08020), P(0x08038),
	P(0x0803c), P(0x08048), P(0x08054), P(0x08058), P(0x0805c),
	P(0x08060), P(0x08064), P(0x08070), P(0x080c0), P(0x080c4),
	P(0x080c8), P(0x080cc), P(0x080d0), P(0x080d4), P(0x080d8),
	P(0x080e0), P(0x080e4), P(0x080e8), P(0x080ec), P(0x080f0),
	P(0x080f4), P(0x080f8), P(0x080fc), P(0x08100), P(0x08104),
	P(0x08108), P(0x0810c), P(0x08110), P(0x08118), P(0x0811c),
	P(0x08124), P(0x08128), P(0x0812c), P(0x08130), P(0x08134),
	P(0x08138), P(0x0813c), P(0x08144), P(0x08168), P(0x0816c),
	P(0x08170), P(0x08174), P(0x08178), P(0x0817c), P(0x081c0),
	P(0x081ec), P(0x081f0), P(0x081f4), P(0x081f8), P(0x081fc),
	P(0x08200), P(0x08204), P(0x08208), P(0x0820c), P(0x08210),
	P(0x08214), P(0x08218), P(0x0821c), P(0x08220), P(0x08224),
	P(0x08228), P(0x0822c), P(0x08230), P(0x08234), P(0x08238),
	P(0x0823c), P(0x08240), P(0x08244), P(0x08248), P(0x0824c),
	P(0x08250), P(0x08254), P(0x08258), P(0x0825c), P(0x08260),
	P(0x08264), P(0x08270), P(0x08274), P(0x08278), P(0x0827c),
	P(0x08284), P(0x08288), P(0x0828c), P(0x08294), P(0x08298),
	P(0x0829c), P(0x08300), P(0x08314), P(0x08328), P(0x0832c),
	P(0x08330), P(0x08334), P(0x08338), P(0x0833c), P(0x08340),
	P(0x08344), P(0x09808), P(0x0980c), P(0x09810), P(0x09814),
	P(0x0981c), P(0x0982c), P(0x09830), P(0x0983c), P(0x0984c),
	P(0x0a84c), P(0x09854), P(0x09900), P(0x09904), P(0x09908),
	P(0x0990c), P(0x09910), P(0x0991c), P(0x09920), P(0x0a920),
	P(0x09928), P(0x0992c), P(0x09934), P(0x09938), P(0x0993c),
	P(0x09948), P(0x0994c), P(0x09954), P(0x09958), P(0x09940),
	P(0x0c95c), P(0x09970), P(0x09974), P(0x09978), P(0x0997c),
	P(0x09980), P(0x09984), P(0x09988), P(0x0998c), P(0x09990),
	P(0x09994), P(0x09998), P(0x0999c), P(0x099a0), P(0x099a4),
	P(0x099a8), P(0x099ac), P(0x099b0), P(0x099b4), P(0x099c4),
	P(0x099c8), P(0x099cc), P(0x099d0), P(0x099d4), P(0x099d8),
	P(0x099dc), P(0x099e0), P(0x099e4), P(0x099e8), P(0x099ec),
	P(0x099f0), P(0x099fc), P(0x0a208), P(0x0a210), P(0x0a214),
	P(0x0a218), P(0x0a220), P(0x0a224), P(0x0a228), P(0x0a22c),
	P(0x0a234), P(0x0a238), P(0x0a240), P(0x0a244), P(0x0a248),
	P(0x0a24c), P(0x0a254), P(0x0a258), P(0x0a25c), P(0x0a260),
	P(0x0a268), P(0x0a26c), P(0x0b26c), P(0x0d270), P(0x0a278),
	P(0x0d35c), P(0x0d360), P(0x0d364), P(0x0d368), P(0x0d36c),
	P(0x0d370), P(0x0d374), P(0x0d378), P(0x0d37c), P(0x0d380),
	P(0x0d384), P(0x0a38c), P(0x0a390), P(0x0a394), P(0x0a398),
	P(0x0a39c), P(0x0a3a0), P(0x0a3a4), P(0x0a3a8), P(0x0a3ac),
	P(0x0a3b0), P(0x0a3b4), P(0x0a3b8), P(0x0a3bc), P(0x0a3c0),
	P(0x0a3c4), P(0x0a3c8), P(0x0a3cc), P(0x0a3d0), P(0x0a3d4),
	P(0x0a3dc), P(0x0a3e0), P(0x0a3e4), P(0x0a3e8), P(0x07800),
	P(0x07804), P(0x07808), P(0x0780c), P(0x07810), P(0x07818),
	P(0x07824), P(0x07828), P(0x0782c), P(0x07830), P(0x07834),
	P(0x0783c), P(0x07848), P(0x0784c), P(0x07850), P(0x07854),
	P(0x07858), P(0x07860), P(0x07864), P(0x07868), P(0x0786c),
	P(0x07870), P(0x07874), P(0x07878), P(0x0787c), P(0x07880),
	P(0x07884), P(0x07888), P(0x0788c), P(0x07890), P(0x07898)
};

static const uint32_t ar9280_2_0_cm_vals[] = {
	0x00000000, 0x00020015, 0x00000005, 0x00000000, 0x00000008,
	0x00000008, 0x00000010, 0x00000000, 0x0000001f, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x002ffc0f,
	0x002ffc0f, 0x002ffc0f, 0x002ffc0f, 0x002ffc0f, 0x002ffc0f,
	0x002ffc0f, 0x002ffc0f, 0x002ffc0f, 0x002ffc0f, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000002, 0x00000002, 0x0000001f, 0x00000000,
	0x00000000, 0x00000033, 0x00000002, 0x000004c2, 0x00000000,
	0x00000000, 0x00000000, 0x00000700, 0x00000000, 0x00000000,
	0x00000000, 0x40000000, 0x00000000, 0x00000000, 0x000fc78f,
	0x0000000f, 0x00000000, 0x00000000, 0x2a80001a, 0x05dc01e0,
	0x1f402710, 0x01f40000, 0x00001e00, 0x00000000, 0x00400000,
	0xffffffff, 0x0000ffff, 0x003f3f3f, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00020000, 0x00020000, 0x00000001,
	0x00000052, 0x00000000, 0x00000168, 0x000100aa, 0x00003210,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0xffffffff, 0x00000000, 0x00000000,
	0x32143320, 0xfaa4fa50, 0x00000100, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00100000, 0x0010f400, 0x00000100, 0x0001e800,
	0x00000000, 0x00000000, 0x00000000, 0x400000ff, 0x00080922,
	0x88a00010, 0x00000000, 0x40000000, 0x003e4180, 0x00000000,
	0x0000002c, 0x0000002c, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000040, 0x00000000, 0x00000000, 0x00000007,
	0x00000302, 0x00000e00, 0x00ff0000, 0x00000000, 0x000107ff,
	0x00481043, 0x00000000, 0xafa68e30, 0xfd14e000, 0x9c0a9f6b,
	0x00000000, 0x0000a000, 0x00000000, 0x00200400, 0x0040233c,
	0x0040233c, 0x00000044, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x01002310, 0x10000fff, 0x04900000, 0x04900000,
	0x00000001, 0x00000004, 0x1e1f2022, 0x0a0b0c0d, 0x00000000,
	0x9280c00a, 0x00020028, 0x5f3ca3de, 0x2108ecff, 0x14750604,
	0x004b6a8e, 0x190fb514, 0x00000000, 0x00000001, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000001,
	0x201fff00, 0x006f0000, 0x03051000, 0x00000820, 0x06336f77,
	0x6af6532f, 0x08f186c8, 0x00046384, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0xaaaaaaaa, 0x3c466478, 0x0cc80caa,
	0x00000000, 0x00001042, 0x803e4788, 0x4080a333, 0x40206c10,
	0x009c4060, 0x01834061, 0x00000400, 0x000003b5, 0x233f7180,
	0x20202020, 0x20202020, 0x38490a20, 0x00007bb6, 0x0fff3ffc,
	0x00000000, 0x00000000, 0x0cdbd380, 0x0f0f0f01, 0xdfa91f01,
	0x00000000, 0x0e79e5c6, 0x0e79e5c6, 0x00820820, 0x1ce739ce,
	0x07ffffef, 0x0fffffe7, 0x17ffffe5, 0x1fffffe4, 0x37ffffe3,
	0x3fffffe3, 0x57ffffe3, 0x5fffffe2, 0x7fffffe2, 0x7f3c7bba,
	0xf3307ff0, 0x20202020, 0x20202020, 0x1ce739ce, 0x000001ce,
	0x00000001, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000246, 0x20202020, 0x20202020, 0x20202020,
	0x1ce739ce, 0x000001ce, 0x00000000, 0x18c43433, 0x00040000,
	0xdb005012, 0x04924914, 0x21084210, 0x6d801300, 0x07e41000,
	0x00040000, 0xdb005012, 0x04924914, 0x21084210, 0x6d801300,
	0x07e40000, 0x00100000, 0x773f0567, 0x54214514, 0x12035828,
	0x9259269a, 0x52802000, 0x0a8e370e, 0xc0102850, 0x812d4000,
	0x807ec400, 0x001b6db0, 0x00376b63, 0x06db6db6, 0x006d8000,
	0xffeffffe, 0xffeffffe, 0x00010000, 0x02060aeb, 0x2a850160
};

static const uint16_t ar9280_2_0_fast_clock_regs[] = {
	P(0x01030), P(0x01070), P(0x010b0), P(0x08014), P(0x0801c),
	P(0x08318), P(0x09820), P(0x09824), P(0x09828), P(0x09834),
	P(0x09844), P(0x09914), P(0x09918)
};

static const uint32_t ar9280_2_0_fast_clock_vals_5g20[] = {
	0x00000268, 0x0000018c, 0x00000fd0, 0x044c044c, 0x148ec02b,
	0x000044c0, 0x02020200, 0x01000f0f, 0x0b020001, 0x00000f0f,
	0x03721821, 0x00000898, 0x0000000b
};

static const uint32_t ar9280_2_0_fast_clock_vals_5g40[] = {
	0x000004d0, 0x00000318, 0x00001fa0, 0x08980898, 0x148ec057,
	0x00008980, 0x02020200, 0x01000f0f, 0x0b020001, 0x00000f0f,
	0x03721821, 0x00001130, 0x00000016
};

static const struct athn_ini ar9280_2_0_ini = {
	nitems(ar9280_2_0_regs),
	ar9280_2_0_regs,
	ar9280_2_0_vals_5g20,
	ar9280_2_0_vals_5g40,
	ar9280_2_0_vals_2g40,
	ar9280_2_0_vals_2g20,
	nitems(ar9280_2_0_cm_regs),
	ar9280_2_0_cm_regs,
	ar9280_2_0_cm_vals,
	nitems(ar9280_2_0_fast_clock_regs),
	ar9280_2_0_fast_clock_regs,
	ar9280_2_0_fast_clock_vals_5g20,
	ar9280_2_0_fast_clock_vals_5g40
};

/*
 * AR9280 2.0 Tx gains.
 */
static const uint16_t ar9280_2_0_tx_gain_regs[] = {
	P(0x0a274), P(0x0a27c), P(0x0a300), P(0x0a304), P(0x0a308),
	P(0x0a30c), P(0x0a310), P(0x0a314), P(0x0a318), P(0x0a31c),
	P(0x0a320), P(0x0a324), P(0x0a328), P(0x0a32c), P(0x0a330),
	P(0x0a334), P(0x0a338), P(0x0a33c), P(0x0a340), P(0x0a344),
	P(0x0a348), P(0x0a34c), P(0x0a350), P(0x0a354), P(0x0a3ec),
	P(0x07814), P(0x07838), P(0x0781c), P(0x07840), P(0x07820),
	P(0x07844)
};

static const uint32_t ar9280_2_0_tx_gain_vals_5g[] = {
	0x0a19c652, 0x050701ce, 0x00000000, 0x00003002, 0x00006004,
	0x0000a006, 0x0000e012, 0x00011014, 0x0001504a, 0x0001904c,
	0x0001c04e, 0x00020092, 0x0002410a, 0x0002710c, 0x0002b18b,
	0x0002e1cc, 0x000321ec, 0x000321ec, 0x000321ec, 0x000321ec,
	0x000321ec, 0x000321ec, 0x000321ec, 0x000321ec, 0x00f70081,
	0x0019beff, 0x0019beff, 0x00392000, 0x00392000, 0x92592480,
	0x92592480
};

static const uint32_t ar9280_2_0_tx_gain_vals_2g[] = {
	0x0a1aa652, 0x050701ce, 0x00000000, 0x00003002, 0x00008009,
	0x0000b00b, 0x0000e012, 0x00012048, 0x0001604a, 0x0001a211,
	0x0001e213, 0x0002121b, 0x00024412, 0x00028414, 0x0002b44a,
	0x00030649, 0x0003364b, 0x00038a49, 0x0003be48, 0x0003ee4a,
	0x00042e88, 0x00046e8a, 0x00049ec9, 0x0004bf42, 0x00f70081,
	0x0019beff, 0x0019beff, 0x00392000, 0x00392000, 0x92592480,
	0x92592480
};

static const struct athn_gain ar9280_2_0_tx_gain = {
	nitems(ar9280_2_0_tx_gain_regs),
	ar9280_2_0_tx_gain_regs,
	ar9280_2_0_tx_gain_vals_5g,
	ar9280_2_0_tx_gain_vals_2g
};

static const uint32_t ar9280_2_0_tx_gain_high_power_vals_5g[] = {
	0x0a19e652, 0x050739ce, 0x00000000, 0x00003002, 0x00006004,
	0x0000a006, 0x0000e012, 0x00011014, 0x0001504a, 0x0001904c,
	0x0001c04e, 0x00021092, 0x0002510a, 0x0002910c, 0x0002c18b,
	0x0002f1cc, 0x000321eb, 0x000341ec, 0x000341ec, 0x000341ec,
	0x000341ec, 0x000341ec, 0x000341ec, 0x000341ec, 0x00f70081,
	0x00198eff, 0x00198eff, 0x00172000, 0x00172000, 0xf258a480,
	0xf258a480
};

static const uint32_t ar9280_2_0_tx_gain_high_power_vals_2g[] = {
	0x0a1aa652, 0x050739ce, 0x00000000, 0x00004002, 0x00007008,
	0x0000c010, 0x00010012, 0x00013014, 0x0001820a, 0x0001b211,
	0x0001e213, 0x00022411, 0x00025413, 0x00029811, 0x0002c813,
	0x00030a14, 0x00035a50, 0x00039c4c, 0x0003de8a, 0x00042e92,
	0x00046ed2, 0x0004bed5, 0x0004ff54, 0x00055fd5, 0x00f70081,
	0x00198eff, 0x00198eff, 0x00172000, 0x00172000, 0xf258a480,
	0xf258a480
};

static const struct athn_gain ar9280_2_0_tx_gain_high_power = {
	nitems(ar9280_2_0_tx_gain_regs),
	ar9280_2_0_tx_gain_regs,
	ar9280_2_0_tx_gain_high_power_vals_5g,
	ar9280_2_0_tx_gain_high_power_vals_2g
};

/*
 * AR9280 2.0 Rx gains.
 */
static const uint16_t ar9280_2_0_rx_gain_regs[] = {
	P(0x09a00), P(0x09a04), P(0x09a08), P(0x09a0c), P(0x09a10),
	P(0x09a14), P(0x09a18), P(0x09a1c), P(0x09a20), P(0x09a24),
	P(0x09a28), P(0x09a2c), P(0x09a30), P(0x09a34), P(0x09a38),
	P(0x09a3c), P(0x09a40), P(0x09a44), P(0x09a48), P(0x09a4c),
	P(0x09a50), P(0x09a54), P(0x09a58), P(0x09a5c), P(0x09a60),
	P(0x09a64), P(0x09a68), P(0x09a6c), P(0x09a70), P(0x09a74),
	P(0x09a78), P(0x09a7c), P(0x09a80), P(0x09a84), P(0x09a88),
	P(0x09a8c), P(0x09a90), P(0x09a94), P(0x09a98), P(0x09a9c),
	P(0x09aa0), P(0x09aa4), P(0x09aa8), P(0x09aac), P(0x09ab0),
	P(0x09ab4), P(0x09ab8), P(0x09abc), P(0x09ac0), P(0x09ac4),
	P(0x09ac8), P(0x09acc), P(0x09ad0), P(0x09ad4), P(0x09ad8),
	P(0x09adc), P(0x09ae0), P(0x09ae4), P(0x09ae8), P(0x09aec),
	P(0x09af0), P(0x09af4), P(0x09af8), P(0x09afc), P(0x09b00),
	P(0x09b04), P(0x09b08), P(0x09b0c), P(0x09b10), P(0x09b14),
	P(0x09b18), P(0x09b1c), P(0x09b20), P(0x09b24), P(0x09b28),
	P(0x09b2c), P(0x09b30), P(0x09b34), P(0x09b38), P(0x09b3c),
	P(0x09b40), P(0x09b44), P(0x09b48), P(0x09b4c), P(0x09b50),
	P(0x09b54), P(0x09b58), P(0x09b5c), P(0x09b60), P(0x09b64),
	P(0x09b68), P(0x09b6c), P(0x09b70), P(0x09b74), P(0x09b78),
	P(0x09b7c), P(0x09b80), P(0x09b84), P(0x09b88), P(0x09b8c),
	P(0x09b90), P(0x09b94), P(0x09b98), P(0x09b9c), P(0x09ba0),
	P(0x09ba4), P(0x09ba8), P(0x09bac), P(0x09bb0), P(0x09bb4),
	P(0x09bb8), P(0x09bbc), P(0x09bc0), P(0x09bc4), P(0x09bc8),
	P(0x09bcc), P(0x09bd0), P(0x09bd4), P(0x09bd8), P(0x09bdc),
	P(0x09be0), P(0x09be4), P(0x09be8), P(0x09bec), P(0x09bf0),
	P(0x09bf4), P(0x09bf8), P(0x09bfc), P(0x09848), P(0x0a848)
};

static const uint32_t ar9280_2_0_rx_gain_vals_5g[] = {
	0x00008184, 0x00008188, 0x0000818c, 0x00008190, 0x00008194,
	0x00008200, 0x00008204, 0x00008208, 0x0000820c, 0x00008210,
	0x00008214, 0x00008280, 0x00008284, 0x00008288, 0x0000828c,
	0x00008290, 0x00008300, 0x00008304, 0x00008308, 0x0000830c,
	0x00008310, 0x00008314, 0x00008380, 0x00008384, 0x00008388,
	0x0000838c, 0x00008390, 0x00008394, 0x0000a380, 0x0000a384,
	0x0000a388, 0x0000a38c, 0x0000a390, 0x0000a394, 0x0000a780,
	0x0000a784, 0x0000a788, 0x0000a78c, 0x0000a790, 0x0000a794,
	0x0000ab84, 0x0000ab88, 0x0000ab8c, 0x0000ab90, 0x0000ab94,
	0x0000af80, 0x0000af84, 0x0000af88, 0x0000af8c, 0x0000af90,
	0x0000af94, 0x0000b380, 0x0000b384, 0x0000b388, 0x0000b38c,
	0x0000b390, 0x0000b394, 0x0000b398, 0x0000b780, 0x0000b784,
	0x0000b788, 0x0000b78c, 0x0000b790, 0x0000b794, 0x0000b798,
	0x0000d784, 0x0000d788, 0x0000d78c, 0x0000d790, 0x0000f780,
	0x0000f784, 0x0000f788, 0x0000f78c, 0x0000f790, 0x0000f794,
	0x0000f7a4, 0x0000f7a8, 0x0000f7ac, 0x0000f7b0, 0x0000f7b4,
	0x0000f7a1, 0x0000f7a5, 0x0000f7a9, 0x0000f7ad, 0x0000f7b1,
	0x0000f7b5, 0x0000f7c5, 0x0000f7c9, 0x0000f7cd, 0x0000f7d1,
	0x0000f7d5, 0x0000f7c2, 0x0000f7c6, 0x0000f7ca, 0x0000f7ce,
	0x0000f7d2, 0x0000f7d6, 0x0000f7c3, 0x0000f7c7, 0x0000f7cb,
	0x0000f7d3, 0x0000f7d7, 0x0000f7db, 0x0000f7db, 0x0000f7db,
	0x0000f7db, 0x0000f7db, 0x0000f7db, 0x0000f7db, 0x0000f7db,
	0x0000f7db, 0x0000f7db, 0x0000f7db, 0x0000f7db, 0x0000f7db,
	0x0000f7db, 0x0000f7db, 0x0000f7db, 0x0000f7db, 0x0000f7db,
	0x0000f7db, 0x0000f7db, 0x0000f7db, 0x0000f7db, 0x0000f7db,
	0x0000f7db, 0x0000f7db, 0x0000f7db, 0x00001066, 0x00001066
};

static const uint32_t ar9280_2_0_rx_gain_vals_2g[] = {
	0x00008000, 0x00008000, 0x00008000, 0x00008000, 0x00008000,
	0x00008000, 0x00008004, 0x00008008, 0x0000800c, 0x00008080,
	0x00008084, 0x00008088, 0x0000808c, 0x00008100, 0x00008104,
	0x00008108, 0x0000810c, 0x00008110, 0x00008114, 0x00008180,
	0x00008184, 0x00008188, 0x0000818c, 0x00008190, 0x00008194,
	0x000081a0, 0x0000820c, 0x000081a8, 0x00008284, 0x00008288,
	0x00008224, 0x00008290, 0x00008300, 0x00008304, 0x00008308,
	0x0000830c, 0x00008380, 0x00008384, 0x00008700, 0x00008704,
	0x00008708, 0x0000870c, 0x00008780, 0x00008784, 0x00008b00,
	0x00008b04, 0x00008b08, 0x00008b0c, 0x00008b80, 0x00008b84,
	0x00008b88, 0x00008b8c, 0x00008b90, 0x00008f80, 0x00008f84,
	0x00008f88, 0x00008f8c, 0x00008f90, 0x0000930c, 0x00009310,
	0x00009384, 0x00009388, 0x00009324, 0x00009704, 0x000096a4,
	0x000096a8, 0x00009710, 0x00009714, 0x00009720, 0x00009724,
	0x00009728, 0x0000972c, 0x000097a0, 0x000097a4, 0x000097a8,
	0x000097b0, 0x000097b4, 0x000097b8, 0x000097a5, 0x000097a9,
	0x000097ad, 0x000097b1, 0x000097b5, 0x000097b9, 0x000097c5,
	0x000097c9, 0x000097d1, 0x000097d5, 0x000097d9, 0x000097c6,
	0x000097ca, 0x000097ce, 0x000097d2, 0x000097d6, 0x000097c3,
	0x000097c7, 0x000097cb, 0x000097cf, 0x000097d7, 0x000097db,
	0x000097db, 0x000097db, 0x000097db, 0x000097db, 0x000097db,
	0x000097db, 0x000097db, 0x000097db, 0x000097db, 0x000097db,
	0x000097db, 0x000097db, 0x000097db, 0x000097db, 0x000097db,
	0x000097db, 0x000097db, 0x000097db, 0x000097db, 0x000097db,
	0x000097db, 0x000097db, 0x000097db, 0x000097db, 0x000097db,
	0x000097db, 0x000097db, 0x000097db, 0x00001063, 0x00001063
};

static const struct athn_gain ar9280_2_0_rx_gain = {
	nitems(ar9280_2_0_rx_gain_regs),
	ar9280_2_0_rx_gain_regs,
	ar9280_2_0_rx_gain_vals_5g,
	ar9280_2_0_rx_gain_vals_2g
};

static const uint32_t ar9280_2_0_rx_gain_13db_backoff_vals_5g[] = {
	0x00008184, 0x00008188, 0x0000818c, 0x00008190, 0x00008194,
	0x00008200, 0x00008204, 0x00008208, 0x0000820c, 0x00008210,
	0x00008214, 0x00008280, 0x00008284, 0x00008288, 0x0000828c,
	0x00008290, 0x00008300, 0x00008304, 0x00008308, 0x0000830c,
	0x00008310, 0x00008314, 0x00008380, 0x00008384, 0x00008388,
	0x0000838c, 0x00008390, 0x00008394, 0x0000a380, 0x0000a384,
	0x0000a388, 0x0000a38c, 0x0000a390, 0x0000a394, 0x0000a780,
	0x0000a784, 0x0000a788, 0x0000a78c, 0x0000a790, 0x0000a794,
	0x0000ab84, 0x0000ab88, 0x0000ab8c, 0x0000ab90, 0x0000ab94,
	0x0000af80, 0x0000af84, 0x0000af88, 0x0000af8c, 0x0000af90,
	0x0000af94, 0x0000b380, 0x0000b384, 0x0000b388, 0x0000b38c,
	0x0000b390, 0x0000b394, 0x0000b398, 0x0000b780, 0x0000b784,
	0x0000b788, 0x0000b78c, 0x0000b790, 0x0000b794, 0x0000b798,
	0x0000d784, 0x0000d788, 0x0000d78c, 0x0000d790, 0x0000f780,
	0x0000f784, 0x0000f788, 0x0000f78c, 0x0000f790, 0x0000f794,
	0x0000f7a4, 0x0000f7a8, 0x0000f7ac, 0x0000f7b0, 0x0000f7b4,
	0x0000f7a1, 0x0000f7a5, 0x0000f7a9, 0x0000f7ad, 0x0000f7b1,
	0x0000f7b5, 0x0000f7c5, 0x0000f7c9, 0x0000f7cd, 0x0000f7d1,
	0x0000f7d5, 0x0000f7c2, 0x0000f7c6, 0x0000f7ca, 0x0000f7ce,
	0x0000f7d2, 0x0000f7d6, 0x0000f7c3, 0x0000f7c7, 0x0000f7cb,
	0x0000f7d3, 0x0000f7d7, 0x0000f7db, 0x0000f7db, 0x0000f7db,
	0x0000f7db, 0x0000f7db, 0x0000f7db, 0x0000f7db, 0x0000f7db,
	0x0000f7db, 0x0000f7db, 0x0000f7db, 0x0000f7db, 0x0000f7db,
	0x0000f7db, 0x0000f7db, 0x0000f7db, 0x0000f7db, 0x0000f7db,
	0x0000f7db, 0x0000f7db, 0x0000f7db, 0x0000f7db, 0x0000f7db,
	0x0000f7db, 0x0000f7db, 0x0000f7db, 0x00001066, 0x00001066
};

static const uint32_t ar9280_2_0_rx_gain_13db_backoff_vals_2g[] = {
	0x00000290, 0x00000300, 0x00000304, 0x00000308, 0x0000030c,
	0x00008000, 0x00008004, 0x00008008, 0x0000800c, 0x00008080,
	0x00008084, 0x00008088, 0x0000808c, 0x00008100, 0x00008104,
	0x00008108, 0x0000810c, 0x00008110, 0x00008114, 0x00008180,
	0x00008184, 0x00008188, 0x0000818c, 0x00008190, 0x00008194,
	0x000081a0, 0x0000820c, 0x000081a8, 0x00008284, 0x00008288,
	0x00008224, 0x00008290, 0x00008300, 0x00008304, 0x00008308,
	0x0000830c, 0x00008380, 0x00008384, 0x00008700, 0x00008704,
	0x00008708, 0x0000870c, 0x00008780, 0x00008784, 0x00008b00,
	0x00008b04, 0x00008b08, 0x00008b0c, 0x00008b80, 0x00008b84,
	0x00008b88, 0x00008b8c, 0x00008b90, 0x00008f80, 0x00008f84,
	0x00008f88, 0x00008f8c, 0x00008f90, 0x00009310, 0x00009314,
	0x00009320, 0x00009324, 0x00009328, 0x0000932c, 0x00009330,
	0x00009334, 0x00009321, 0x00009325, 0x00009329, 0x0000932d,
	0x00009331, 0x00009335, 0x00009322, 0x00009326, 0x0000932a,
	0x0000932e, 0x00009332, 0x00009336, 0x00009323, 0x00009327,
	0x0000932b, 0x0000932f, 0x00009333, 0x00009337, 0x00009343,
	0x00009347, 0x0000934b, 0x0000934f, 0x00009353, 0x00009357,
	0x0000935b, 0x0000935b, 0x0000935b, 0x0000935b, 0x0000935b,
	0x0000935b, 0x0000935b, 0x0000935b, 0x0000935b, 0x0000935b,
	0x0000935b, 0x0000935b, 0x0000935b, 0x0000935b, 0x0000935b,
	0x0000935b, 0x0000935b, 0x0000935b, 0x0000935b, 0x0000935b,
	0x0000935b, 0x0000935b, 0x0000935b, 0x0000935b, 0x0000935b,
	0x0000935b, 0x0000935b, 0x0000935b, 0x0000935b, 0x0000935b,
	0x0000935b, 0x0000935b, 0x0000935b, 0x0000935b, 0x0000935b,
	0x0000935b, 0x0000935b, 0x0000935b, 0x0000105a, 0x0000105a
};

static const struct athn_gain ar9280_2_0_rx_gain_13db_backoff = {
	nitems(ar9280_2_0_rx_gain_regs),
	ar9280_2_0_rx_gain_regs,
	ar9280_2_0_rx_gain_13db_backoff_vals_5g,
	ar9280_2_0_rx_gain_13db_backoff_vals_2g
};

static const uint32_t ar9280_2_0_rx_gain_23db_backoff_vals_5g[] = {
	0x00008184, 0x00008188, 0x0000818c, 0x00008190, 0x00008194,
	0x00008200, 0x00008204, 0x00008208, 0x0000820c, 0x00008210,
	0x00008214, 0x00008280, 0x00008284, 0x00008288, 0x0000828c,
	0x00008290, 0x00008300, 0x00008304, 0x00008308, 0x0000830c,
	0x00008310, 0x00008314, 0x00008380, 0x00008384, 0x00008388,
	0x0000838c, 0x00008390, 0x00008394, 0x0000a380, 0x0000a384,
	0x0000a388, 0x0000a38c, 0x0000a390, 0x0000a394, 0x0000a780,
	0x0000a784, 0x0000a788, 0x0000a78c, 0x0000a790, 0x0000a794,
	0x0000ab84, 0x0000ab88, 0x0000ab8c, 0x0000ab90, 0x0000ab94,
	0x0000af80, 0x0000af84, 0x0000af88, 0x0000af8c, 0x0000af90,
	0x0000af94, 0x0000b380, 0x0000b384, 0x0000b388, 0x0000b38c,
	0x0000b390, 0x0000b394, 0x0000b398, 0x0000b780, 0x0000b784,
	0x0000b788, 0x0000b78c, 0x0000b790, 0x0000b794, 0x0000b798,
	0x0000d784, 0x0000d788, 0x0000d78c, 0x0000d790, 0x0000f780,
	0x0000f784, 0x0000f788, 0x0000f78c, 0x0000f790, 0x0000f794,
	0x0000f7a4, 0x0000f7a8, 0x0000f7ac, 0x0000f7b0, 0x0000f7b4,
	0x0000f7a1, 0x0000f7a5, 0x0000f7a9, 0x0000f7ad, 0x0000f7b1,
	0x0000f7b5, 0x0000f7c5, 0x0000f7c9, 0x0000f7cd, 0x0000f7d1,
	0x0000f7d5, 0x0000f7c2, 0x0000f7c6, 0x0000f7ca, 0x0000f7ce,
	0x0000f7d2, 0x0000f7d6, 0x0000f7c3, 0x0000f7c7, 0x0000f7cb,
	0x0000f7d3, 0x0000f7d7, 0x0000f7db, 0x0000f7db, 0x0000f7db,
	0x0000f7db, 0x0000f7db, 0x0000f7db, 0x0000f7db, 0x0000f7db,
	0x0000f7db, 0x0000f7db, 0x0000f7db, 0x0000f7db, 0x0000f7db,
	0x0000f7db, 0x0000f7db, 0x0000f7db, 0x0000f7db, 0x0000f7db,
	0x0000f7db, 0x0000f7db, 0x0000f7db, 0x0000f7db, 0x0000f7db,
	0x0000f7db, 0x0000f7db, 0x0000f7db, 0x00001066, 0x00001066
};

static const uint32_t ar9280_2_0_rx_gain_23db_backoff_vals_2g[] = {
	0x00000290, 0x00000300, 0x00000304, 0x00000308, 0x0000030c,
	0x00008000, 0x00008004, 0x00008008, 0x0000800c, 0x00008080,
	0x00008084, 0x00008088, 0x0000808c, 0x00008100, 0x00008104,
	0x00008108, 0x0000810c, 0x00008110, 0x00008114, 0x00008180,
	0x00008184, 0x00008188, 0x0000818c, 0x00008190, 0x00008194,
	0x000081a0, 0x0000820c, 0x000081a8, 0x00008284, 0x00008288,
	0x00008224, 0x00008290, 0x00008300, 0x00008304, 0x00008308,
	0x0000830c, 0x00008380, 0x00008384, 0x00008700, 0x00008704,
	0x00008708, 0x0000870c, 0x00008780, 0x00008784, 0x00008b00,
	0x00008b04, 0x00008b08, 0x00008b0c, 0x00008b10, 0x00008b80,
	0x00008b84, 0x00008b88, 0x00008b8c, 0x00008b90, 0x00008b94,
	0x00008b98, 0x00008ba4, 0x00008ba8, 0x00008bac, 0x00008bb0,
	0x00008bb4, 0x00008ba1, 0x00008ba5, 0x00008ba9, 0x00008bad,
	0x00008bb1, 0x00008bb5, 0x00008ba2, 0x00008ba6, 0x00008baa,
	0x00008bae, 0x00008bb2, 0x00008bb6, 0x00008ba3, 0x00008ba7,
	0x00008bab, 0x00008baf, 0x00008bb3, 0x00008bb7, 0x00008bc3,
	0x00008bc7, 0x00008bcb, 0x00008bcf, 0x00008bd3, 0x00008bd7,
	0x00008bdb, 0x00008bdb, 0x00008bdb, 0x00008bdb, 0x00008bdb,
	0x00008bdb, 0x00008bdb, 0x00008bdb, 0x00008bdb, 0x00008bdb,
	0x00008bdb, 0x00008bdb, 0x00008bdb, 0x00008bdb, 0x00008bdb,
	0x00008bdb, 0x00008bdb, 0x00008bdb, 0x00008bdb, 0x00008bdb,
	0x00008bdb, 0x00008bdb, 0x00008bdb, 0x00008bdb, 0x00008bdb,
	0x00008bdb, 0x00008bdb, 0x00008bdb, 0x00008bdb, 0x00008bdb,
	0x00008bdb, 0x00008bdb, 0x00008bdb, 0x00008bdb, 0x00008bdb,
	0x00008bdb, 0x00008bdb, 0x00008bdb, 0x00008bdb, 0x00008bdb,
	0x00008bdb, 0x00008bdb, 0x00008bdb, 0x00001055, 0x00001055
};

static const struct athn_gain ar9280_2_0_rx_gain_23db_backoff = {
	nitems(ar9280_2_0_rx_gain_regs),
	ar9280_2_0_rx_gain_regs,
	ar9280_2_0_rx_gain_23db_backoff_vals_5g,
	ar9280_2_0_rx_gain_23db_backoff_vals_2g
};

/*
 * Serializer/Deserializer programming.
 */

static const uint32_t ar9280_2_0_serdes_regs[] = {
	AR_PCIE_SERDES,
	AR_PCIE_SERDES,
	AR_PCIE_SERDES,
	AR_PCIE_SERDES,
	AR_PCIE_SERDES,
	AR_PCIE_SERDES,
	AR_PCIE_SERDES,
	AR_PCIE_SERDES,
	AR_PCIE_SERDES,
	AR_PCIE_SERDES2,
};

static const uint32_t ar9280_2_0_serdes_vals[] = {
	0x9248fd00,
	0x24924924,
	0xa8000019,
	0x13160820,
	0xe5980560,
#ifdef ATHN_PCIE_CLKREQ
	0xc01dcffc,
#else
	0xc01dcffd,
#endif
	0x1aaabe41,
	0xbe105554,
	0x00043007,
	0x00000000
};

static const struct athn_serdes ar9280_2_0_serdes = {
	nitems(ar9280_2_0_serdes_vals),
	ar9280_2_0_serdes_regs,
	ar9280_2_0_serdes_vals
};
