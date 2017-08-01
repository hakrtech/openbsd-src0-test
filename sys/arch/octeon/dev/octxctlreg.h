/*	$OpenBSD: octxctlreg.h,v 1.1 2017/08/01 16:18:12 visa Exp $	*/

/*
 * Copyright (c) 2017 Visa Hankala
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

#ifndef _OCTXCTLREG_H_
#define _OCTXCTLREG_H_

#define XCTL_CTL			0x00
#define   XCTL_CTL_CLEAR_BIST			0x8000000000000000ull
#define   XCTL_CTL_START_BIST			0x4000000000000000ull
#define   XCTL_CTL_REFCLK_SEL			0x3000000000000000ull
#define   XCTL_CTL_REFCLK_SEL_SHIFT		60
#define   XCTL_CTL_SSC_EN			0x0800000000000000ull
#define   XCTL_CTL_SSC_RANG			0x0700000000000000ull
#define   XCTL_CTL_SSC_REFCLK_SEL		0x00ff800000000000ull
#define   XCTL_CTL_MPLL_MULT			0x00007f0000000000ull
#define   XCTL_CTL_MPLL_MULT_SHIFT		40
#define   XCTL_CTL_REFCLK_SSP_EN		0x0000008000000000ull
#define   XCTL_CTL_REFCLK_DIV2			0x0000004000000000ull
#define   XCTL_CTL_REFCLK_FSEL			0x0000003f00000000ull
#define   XCTL_CTL_REFCLK_FSEL_SHIFT		32
#define   XCTL_CTL_CLK_EN			0x0000000040000000ull
#define   XCTL_CTL_CLK_BYP_SEL			0x0000000020000000ull
#define   XCTL_CTL_CLKDIV_RST			0x0000000010000000ull
#define   XCTL_CTL_CLKDIV_SEL			0x0000000007000000ull
#define   XCTL_CTL_CLKDIV_SEL_SHIFT		24
#define   XCTL_CTL_USB3_PORT_PERM_ATTACH	0x0000000000200000ull
#define   XCTL_CTL_USB2_PORT_PERM_ATTACH	0x0000000000100000ull
#define   XCTL_CTL_USB3_PORT_DIS		0x0000000000040000ull
#define   XCTL_CTL_USB2_PORT_DIS		0x0000000000010000ull
#define   XCTL_CTL_SSPOWER_EN			0x0000000000004000ull
#define   XCTL_CTL_HSPOWER_EN			0x0000000000001000ull
#define   XCTL_CTL_CSCLK_EN			0x0000000000000010ull
#define   XCTL_CTL_DRD_MODE			0x0000000000000008ull
#define   XCTL_CTL_UPHY_RST			0x0000000000000004ull
#define   XCTL_CTL_UAHC_RST			0x0000000000000002ull
#define   XCTL_CTL_UCTL_RST			0x0000000000000001ull

#define XCTL_HOST_CFG			0xe0
#define   XCTL_HOST_CFG_PPC_EN			0x0000000002000000ull
#define   XCTL_HOST_CFG_PPC_ACTIVE_HIGH_EN	0x0000000001000000ull

#define XCTL_SHIM_CFG			0xe8
#define   XCTL_SHIM_CFG_DMA_BYTE_SWAP		0x0000000000000300ull
#define   XCTL_SHIM_CFG_DMA_BYTE_SWAP_SHIFT	8
#define   XCTL_SHIM_CFG_CSR_BYTE_SWAP		0x0000000000000003ull
#define   XCTL_SHIM_CFG_CSR_BYTE_SWAP_SHIFT	0

/*
 * DWC3 core control registers.
 * These are relative to the xHCI register space.
 */

#define DWC3_GCTL			0xc110
#define   DWC3_GCTL_PRTCAP_MASK			0x00003000u
#define   DWC3_GCTL_PRTCAP_HOST			0x00001000u
#define   DWC3_GCTL_SOFITPSYNC			0x00000400u
#define   DWC3_GCTL_SCALEDOWN_MASK		0x00000030u
#define   DWC3_GCTL_DISSCRAMBLE			0x00000004u
#define   DWC3_GCTL_DSBLCLKGTNG			0x00000001u

#define DWC3_GSNPSID			0xc120

#define DWC3_GUSB2PHYCFG(n)		(0xc200 + ((n) * 0x04))
#define   DWC3_GUSB2PHYCFG_SUSPHY		0x00000060u

#define DWC3_GUSB3PIPECTL(n)		(0xc2c0 + ((n) * 0x04))
#define   DWC3_GUSB3PIPECTL_UX_EXIT_PX		0x08000000u
#define   DWC3_GUSB3PIPECTL_SUSPHY		0x00020000u

/* DWC3 revision numbers. */
#define DWC3_REV_210A			0x210a
#define DWC3_REV_250A			0x250a

#endif /* !_OCTXCTLREG_H_ */
