/*
 * THIS FILE IS AUTOMATICALLY GENERATED
 * DONT EDIT THIS FILE
 */

/*	$OpenBSD: cn30xxpipreg.h,v 1.6 2016/08/06 04:32:24 visa Exp $	*/

/*
 * Copyright (c) 2007 Internet Initiative Japan, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Cavium Networks OCTEON CN30XX Hardware Reference Manual
 * CN30XX-HM-1.0
 * 7.8 PIP Registers
 */

#ifndef _CN30XXPIPREG_H_
#define _CN30XXPIPREG_H_

#define	PIP_BIST_STATUS				0x00011800a0000000ULL
#define	PIP_INT_REG				0x00011800a0000008ULL
#define	PIP_INT_EN				0x00011800a0000010ULL
#define	PIP_STAT_CTL				0x00011800a0000018ULL
#define	PIP_GBL_CTL				0x00011800a0000020ULL
#define	PIP_GBL_CFG				0x00011800a0000028ULL
#define	PIP_SOFT_RST				0x00011800a0000030ULL
#define	PIP_IP_OFFSET				0x00011800a0000060ULL
#define	PIP_TAG_SECRET				0x00011800a0000068ULL
#define	PIP_TAG_MASK				0x00011800a0000070ULL
#define	PIP_DEC_IPSEC0				0x00011800a0000080ULL
#define	PIP_DEC_IPSEC1				0x00011800a0000088ULL
#define	PIP_DEC_IPSEC2				0x00011800a0000090ULL
#define	PIP_DEC_IPSEC3				0x00011800a0000098ULL
#define	PIP_RAW_WORD				0x00011800a00000b0ULL
#define	PIP_QOS_VLAN0				0x00011800a00000c0ULL
#define	PIP_QOS_VLAN1				0x00011800a00000c8ULL
#define	PIP_QOS_VLAN2				0x00011800a00000d0ULL
#define	PIP_QOS_VLAN3				0x00011800a00000d8ULL
#define	PIP_QOS_VLAN4				0x00011800a00000e0ULL
#define	PIP_QOS_VLAN5				0x00011800a00000e8ULL
#define	PIP_QOS_VLAN6				0x00011800a00000f0ULL
#define	PIP_QOS_VLAN7				0x00011800a00000f8ULL
#define	PIP_QOS_WATCH0				0x00011800a0000100ULL
#define	PIP_QOS_WATCH1				0x00011800a0000108ULL
#define	PIP_QOS_WATCH2				0x00011800a0000110ULL
#define	PIP_QOS_WATCH3				0x00011800a0000118ULL
#define	PIP_PRT_CFG0				0x00011800a0000200ULL
#define	PIP_PRT_CFG1				0x00011800a0000208ULL
#define	PIP_PRT_CFG2				0x00011800a0000210ULL
#define	PIP_PRT_CFG32				0x00011800a0000300ULL
#define	PIP_PRT_TAG0				0x00011800a0000400ULL
#define	PIP_PRT_TAG1				0x00011800a0000408ULL
#define	PIP_PRT_TAG2				0x00011800a0000410ULL
#define	PIP_PRT_TAG32				0x00011800a0000500ULL
#define	PIP_QOS_DIFF0				0x00011800a0000600ULL
/* PIP_QOS_DIFF[1-63] */
/* PIP_STAT[0-9]_PRT{0,1,2,32} */
#define	PIP_STAT0_PRT0				0x00011800a0000800ULL
#define	PIP_STAT0_PRT1				0x00011800a0000850ULL
#define	PIP_STAT0_PRT2				0x00011800a00008a0ULL
#define	PIP_STAT0_PRT32				0x00011800a0001200ULL
#define	PIP_TAG_INC0				0x00011800a0001800ULL
/* PIP_TAG_INC[1-63] */
#define	PIP_STAT_INB_PKTS0			0x00011800a0001a00ULL
#define	PIP_STAT_INB_PKTS1			0x00011800a0001a20ULL
#define	PIP_STAT_INB_PKTS2			0x00011800a0001a40ULL
#define	PIP_STAT_INB_PKTS32			0x00011800a0001e00ULL
#define	PIP_STAT_INB_OCTS0			0x00011800a0001a08ULL
#define	PIP_STAT_INB_OCTS1			0x00011800a0001a28ULL
#define	PIP_STAT_INB_OCTS2			0x00011800a0001a48ULL
#define	PIP_STAT_INB_OCTS32			0x00011800a0001e08ULL
#define	PIP_STAT_INB_ERRS0			0x00011800a0001a10ULL
#define	PIP_STAT_INB_ERRS1			0x00011800a0001a30ULL
#define	PIP_STAT_INB_ERRS2			0x00011800a0001a50ULL
#define	PIP_STAT_INB_ERRS32			0x00011800a0001e10ULL

#define PIP_BASE 0x00011800a0000000ULL
#define PIP_SIZE 0x1e50ULL

#define	PIP_BIST_STATUS_OFFSET				0x0ULL
#define	PIP_INT_REG_OFFSET				0x8ULL
#define	PIP_INT_EN_OFFSET				0x10ULL
#define	PIP_STAT_CTL_OFFSET				0x18ULL
#define	PIP_GBL_CTL_OFFSET				0x20ULL
#define	PIP_GBL_CFG_OFFSET				0x28ULL
#define	PIP_SOFT_RST_OFFSET				0x30ULL
#define	PIP_IP_OFFSET_OFFSET				0x60ULL
#define	PIP_TAG_SECRET_OFFSET				0x68ULL
#define	PIP_TAG_MASK_OFFSET				0x70ULL
#define	PIP_DEC_IPSEC0_OFFSET				0x80ULL
#define	PIP_DEC_IPSEC1_OFFSET				0x88ULL
#define	PIP_DEC_IPSEC2_OFFSET				0x90ULL
#define	PIP_DEC_IPSEC3_OFFSET				0x98ULL
#define	PIP_RAW_WORD_OFFSET				0xb0ULL
#define	PIP_QOS_VLAN0_OFFSET				0xc0ULL
#define	PIP_QOS_VLAN1_OFFSET				0xc8ULL
#define	PIP_QOS_VLAN2_OFFSET				0xd0ULL
#define	PIP_QOS_VLAN3_OFFSET				0xd8ULL
#define	PIP_QOS_VLAN4_OFFSET				0xe0ULL
#define	PIP_QOS_VLAN5_OFFSET				0xe8ULL
#define	PIP_QOS_VLAN6_OFFSET				0xf0ULL
#define	PIP_QOS_VLAN7_OFFSET				0xf8ULL
#define	PIP_QOS_WATCH0_OFFSET				0x100ULL
#define	PIP_QOS_WATCH1_OFFSET				0x108ULL
#define	PIP_QOS_WATCH2_OFFSET				0x110ULL
#define	PIP_QOS_WATCH3_OFFSET				0x118ULL
#define	PIP_PRT_CFG0_OFFSET				0x200ULL
#define	PIP_PRT_CFG1_OFFSET				0x208ULL
#define	PIP_PRT_CFG2_OFFSET				0x210ULL
#define	PIP_PRT_CFG32_OFFSET				0x300ULL
#define	PIP_PRT_TAG0_OFFSET				0x400ULL
#define	PIP_PRT_TAG1_OFFSET				0x408ULL
#define	PIP_PRT_TAG2_OFFSET				0x410ULL
#define	PIP_PRT_TAG32_OFFSET				0x500ULL
#define	PIP_QOS_DIFF0_OFFSET				0x600ULL
/* PIP_QOS_DIFF[1-63] */
#define	PIP_STAT0_PRT0_OFFSET				0x800ULL
#define	PIP_STAT0_PRT1_OFFSET				0x850ULL
#define	PIP_STAT0_PRT2_OFFSET				0x8a0ULL
#define	PIP_STAT0_PRT32_OFFSET				0x1200ULL
#define	PIP_STAT0_PRT33_OFFSET				0x1250ULL
#define	PIP_STAT1_PRT0_OFFSET				0x800ULL
#define	PIP_STAT1_PRT1_OFFSET				0x850ULL
#define	PIP_STAT1_PRT2_OFFSET				0x8a0ULL
#define	PIP_STAT1_PRT32_OFFSET				0x1200ULL
#define	PIP_STAT1_PRT33_OFFSET				0x1250ULL
#define	PIP_STAT2_PRT0_OFFSET				0x810ULL
#define	PIP_STAT2_PRT1_OFFSET				0x860ULL
#define	PIP_STAT2_PRT2_OFFSET				0x8b0ULL
#define	PIP_STAT2_PRT32_OFFSET				0x1210ULL
#define	PIP_STAT2_PRT33_OFFSET				0x1260ULL
#define	PIP_STAT3_PRT0_OFFSET				0x818ULL
#define	PIP_STAT3_PRT1_OFFSET				0x868ULL
#define	PIP_STAT3_PRT2_OFFSET				0x8b8ULL
#define	PIP_STAT3_PRT32_OFFSET				0x1218ULL
#define	PIP_STAT3_PRT33_OFFSET				0x1268ULL
#define	PIP_STAT4_PRT0_OFFSET				0x820ULL
#define	PIP_STAT4_PRT1_OFFSET				0x870ULL
#define	PIP_STAT4_PRT2_OFFSET				0x8c0ULL
#define	PIP_STAT4_PRT32_OFFSET				0x1220ULL
#define	PIP_STAT4_PRT33_OFFSET				0x1270ULL
#define	PIP_STAT5_PRT0_OFFSET				0x828ULL
#define	PIP_STAT5_PRT1_OFFSET				0x878ULL
#define	PIP_STAT5_PRT2_OFFSET				0x8c8ULL
#define	PIP_STAT5_PRT32_OFFSET				0x1228ULL
#define	PIP_STAT5_PRT33_OFFSET				0x1278ULL
#define	PIP_STAT6_PRT0_OFFSET				0x830ULL
#define	PIP_STAT6_PRT1_OFFSET				0x880ULL
#define	PIP_STAT6_PRT2_OFFSET				0x8d0ULL
#define	PIP_STAT6_PRT32_OFFSET				0x1238ULL
#define	PIP_STAT6_PRT33_OFFSET				0x1288ULL
#define	PIP_STAT7_PRT0_OFFSET				0x838ULL
#define	PIP_STAT7_PRT1_OFFSET				0x888ULL
#define	PIP_STAT7_PRT2_OFFSET				0x8d8ULL
#define	PIP_STAT7_PRT32_OFFSET				0x1238ULL
#define	PIP_STAT7_PRT33_OFFSET				0x1288ULL
#define	PIP_STAT8_PRT0_OFFSET				0x840ULL
#define	PIP_STAT8_PRT1_OFFSET				0x890ULL
#define	PIP_STAT8_PRT2_OFFSET				0x8e0ULL
#define	PIP_STAT8_PRT32_OFFSET				0x1240ULL
#define	PIP_STAT8_PRT33_OFFSET				0x1290ULL
#define	PIP_STAT9_PRT0_OFFSET				0x848ULL
#define	PIP_STAT9_PRT1_OFFSET				0x898ULL
#define	PIP_STAT9_PRT2_OFFSET				0x8e8ULL
#define	PIP_STAT9_PRT32_OFFSET				0x1248ULL
#define	PIP_STAT9_PRT33_OFFSET				0x1298ULL
#define	PIP_TAG_INC0_OFFSET				0x1800ULL
/* PIP_TAG_INC[1-63] */
#define	PIP_STAT_INB_PKTS0_OFFSET			0x1a00ULL
#define	PIP_STAT_INB_PKTS1_OFFSET			0x1a20ULL
#define	PIP_STAT_INB_PKTS2_OFFSET			0x1a40ULL
#define	PIP_STAT_INB_PKTS32_OFFSET			0x1e00ULL
#define	PIP_STAT_INB_OCTS0_OFFSET			0x1a08ULL
#define	PIP_STAT_INB_OCTS1_OFFSET			0x1a28ULL
#define	PIP_STAT_INB_OCTS2_OFFSET			0x1a48ULL
#define	PIP_STAT_INB_OCTS32_OFFSET			0x1e08ULL
#define	PIP_STAT_INB_ERRS0_OFFSET			0x1a10ULL
#define	PIP_STAT_INB_ERRS1_OFFSET			0x1a30ULL
#define	PIP_STAT_INB_ERRS2_OFFSET			0x1a50ULL
#define	PIP_STAT_INB_ERRS32_OFFSET			0x1e10ULL
#define	PIP_STAT_INB_ERRS33_OFFSET			0x1e30ULL

/*
 * PIP_BIST_STATUS
 */
#define	PIP_BIST_STATUS_63_13			0xfffffffffffc0000ULL
#define	PIP_BIST_STATUS_BIST			0x000000000003ffffULL

/*
 * PIP_INT_REG
 */
#define	PIP_INT_REG_63_9			0xfffffffffffffe00ULL
#define	PIP_INT_REG_BEPERR			0x0000000000000100ULL
#define	PIP_INT_REG_FEPERR			0x0000000000000080ULL
#define	PIP_INT_REG_6				0x0000000000000040ULL
#define	PIP_INT_REG_SKPRUNT			0x0000000000000020ULL
#define	PIP_INT_REG_BADTAG			0x0000000000000010ULL
#define	PIP_INT_REG_PRTNXA			0x0000000000000008ULL
#define	PIP_INT_REG_2_1				0x00000006
#define	PIP_INT_REG_PKTDRP			0x00000001

/*
 * PIP_INT_EN
 */
#define	PIP_INT_EN_63_9				0xfffffffffffffe00ULL
#define	PIP_INT_EN_BEPERR			0x0000000000000100ULL
#define	PIP_INT_EN_FEPERR			0x0000000000000080ULL
#define	PIP_INT_EN_6				0x0000000000000040ULL
#define	PIP_INT_EN_SKPRUNT			0x0000000000000020ULL
#define	PIP_INT_EN_BADTAG			0x0000000000000010ULL
#define	PIP_INT_EN_PRTNXA			0x0000000000000008ULL
#define	PIP_INT_EN_2_1				0x00000006
#define	PIP_INT_EN_PKTDRP			0x00000001

/*
 * PIP_STAT_CTL
 */
#define	PIP_STAT_CTL_63_1			0xfffffffffffffffeULL
#define	PIP_STAT_CTL_RDCLR			0x0000000000000001ULL

/*
 * PIP_GBL_CTL
 */
#define	PIP_GBL_CTL_63_17			0xfffffffffffe0000ULL
#define	PIP_GBL_CTL_IGNRS			0x0000000000010000ULL
#define	PIP_GBL_CTL_VS_WQE			0x0000000000008000ULL
#define	PIP_GBL_CTL_VS_QOS			0x0000000000004000ULL
#define	PIP_GBL_CTL_L2MAL			0x0000000000002000ULL
#define	PIP_GBL_CTL_TCP_FLAG			0x0000000000001000ULL
#define	PIP_GBL_CTL_L4_LEN			0x0000000000000800ULL
#define	PIP_GBL_CTL_L4_CHK			0x0000000000000400ULL
#define	PIP_GBL_CTL_L4_PRT			0x0000000000000200ULL
#define	PIP_GBL_CTL_L4_MAL			0x0000000000000100ULL
#define	PIP_GBL_CTL_7_6				0x00000000000000c0ULL
#define	PIP_GBL_CTL_IP6_EEXT			0x0000000000000030ULL
#define	PIP_GBL_CTL_IP4_OPTS			0x0000000000000008ULL
#define	PIP_GBL_CTL_IP_HOP			0x0000000000000004ULL
#define	PIP_GBL_CTL_IP_MAL			0x0000000000000002ULL
#define	PIP_GBL_CTL_IP_CHK			0x0000000000000001ULL

/*
 * PIP_GBL_CFG
 */
/* XXX CN30XX-HM-1.0 says 63_17 is reserved */
#define	PIP_GBL_CFG_63_19			0xfffffffffff80000ULL
#define	PIP_GBL_CFG_TAG_SYN			0x0000000000040000ULL
#define	PIP_GBL_CFG_IP6_UDP			0x0000000000020000ULL
#define	PIP_GBL_CFG_MAX_L2			0x0000000000010000ULL
#define	PIP_GBL_CFG_15_11			0x000000000000f800ULL
#define	PIP_GBL_CFG_RAW_SHF			0x0000000000000700ULL
#define	PIP_GBL_CFG_7_3				0x00000000000000f8ULL
#define	PIP_GBL_CFG_NIP_SHF_MASK		0x0000000000000007ULL
#define	PIP_GBL_CFG_NIP_SHF_SHIFT		0

/*
 * PIP_SFT_RST
 */
#define	PIP_SFT_RST_63_17			0xfffffffffffffffeULL
#define	PIP_SFT_RST_RST				0x0000000000000001ULL

/*
 * PIP_IP_OFFSET
 */
#define	PIP_IP_OFFSET_63_3			0xfffffffffffffff8ULL
/* PIP_IP_OFFSET_OFFSET is defined above - conflict! */
#define	PIP_IP_OFFSET_MASK_OFFSET		0x0000000000000007ULL

/*
 * PIP_TAG_SECRET
 */
#define	PIP_TAG_SECRET_63_3			0xffffffff00000000ULL
#define	PIP_TAG_SECRET_DST			0x00000000ffff0000ULL
#define	PIP_TAG_SECRET_SRC			0x000000000000ffffULL

/*
 * PIP_TAG_MASK
 */
#define	PIP_TAG_MASK_63_16			0xffffffffffff0000ULL
#define	PIP_TAG_MASK_MASK			0x000000000000ffffULL

/*
 * PIP_DEC_IPSECN
 */
#define	PIP_DEC_IPSECN_63_18			0xfffffffffffc0000ULL
#define	PIP_DEC_IPSECN_TCP			0x0000000000020000ULL
#define	PIP_DEC_IPSECN_UDP			0x0000000000010000ULL
#define	PIP_DEC_IPSECN_DPRT			0x000000000000ffffULL

/*
 * PIP_RAW_WORD
 */
#define	PIP_RAW_WORD_63_56			0xff00000000000000ULL
#define	PIP_RAW_WORD_WORD			0x00ffffffffffffffULL

/*
 * PIP_QOS_VLAN
 */
#define	PIP_QOS_VLAN_63_3			0xfffffffffffffff8ULL
#define	PIP_QOS_VLAN_QOS			0x0000000000000007ULL

/*
 * PIP_QOS_WATCHN
 */
#define	PIP_QOS_WATCHN_63_48			0xffff000000000000ULL
#define	PIP_QOS_WATCHN_MASK			0x0000ffff00000000ULL
#define	PIP_QOS_WATCHN_31_28			0x00000000f0000000ULL
#define	PIP_QOS_WATCHN_GRP			0x000000000f000000ULL
#define	PIP_QOS_WATCHN_23			0x0000000000800000ULL
#define	PIP_QOS_WATCHN_WATCHER			0x0000000000700000ULL
#define	PIP_QOS_WATCHN_19_18			0x00000000000c0000ULL
#define	PIP_QOS_WATCHN_TYPE			0x0000000000030000ULL
#define	PIP_QOS_WATCHN_15_0			0x000000000000ffffULL

/*
 * PIP_PRT_CFGN
 */
#define	PIP_PRT_CFGN_63_37			0xffffffe000000000ULL
#define	PIP_PRT_CFGN_RAWDRP			0x0000001000000000ULL
#define	PIP_PRT_CFGN_TAG_INC			0x0000000c00000000ULL
#define	PIP_PRT_CFGN_DYN_RS			0x0000000200000000ULL
#define	PIP_PRT_CFGN_INST_HDR			0x0000000100000000ULL
#define	PIP_PRT_CFGN_GRP_WAT			0x00000000f0000000ULL
#define	PIP_PRT_CFGN_27				0x0000000008000000ULL
#define	PIP_PRT_CFGN_QOS			0x0000000007000000ULL
#define	PIP_PRT_CFGN_QOS_WAT			0x0000000000f00000ULL
#define	PIP_PRT_CFGN_19				0x0000000000080000ULL
#define	PIP_PRT_CFGN_SPARE			0x0000000000040000ULL
#define	PIP_PRT_CFGN_QOS_DIFF			0x0000000000020000ULL
#define	PIP_PRT_CFGN_QOS_VLAN			0x0000000000010000ULL
#define	PIP_PRT_CFGN_15_13			0x000000000000e000ULL
#define	PIP_PRT_CFGN_CRC_EN			0x0000000000001000ULL
#define	PIP_PRT_CFGN_11_10			0x0000000000000c00ULL
#define	PIP_PRT_CFGN_MODE			0x0000000000000300ULL
#define	 PIP_PRT_CFGN_MODE_SHIFT		8
#define   PIP_PORT_CFG_MODE_NONE		(0ULL << PIP_PRT_CFGN_MODE_SHIFT)
#define   PIP_PORT_CFG_MODE_L2			(1ULL << PIP_PRT_CFGN_MODE_SHIFT)
#define   PIP_PORT_CFG_MODE_IP			(2ULL << PIP_PRT_CFGN_MODE_SHIFT)
#define   PIP_PORT_CFG_MODE_PCI			(3ULL << PIP_PRT_CFGN_MODE_SHIFT)
#define	PIP_PRT_CFGN_7				0x0000000000000080ULL
#define	PIP_PRT_CFGN_SKIP			0x000000000000007fULL

/*
 * PIP_PRT_TAGN
 */
#define	PIP_PRT_TAGN_63_40			0xffffff0000000000ULL
#define	PIP_PRT_TAGN_GRPTAGBASE			0x000000f000000000ULL
#define	PIP_PRT_TAGN_GRPTAGMASK			0x0000000f00000000ULL
#define	PIP_PRT_TAGN_GRPTAG			0x0000000080000000ULL
#define	PIP_PRT_TAGN_SPARE			0x0000000040000000ULL
#define	PIP_PRT_TAGN_TAG_MODE			0x0000000030000000ULL
#define	PIP_PRT_TAGN_INC_VS			0x000000000c000000ULL
#define	PIP_PRT_TAGN_INC_VLAN			0x0000000002000000ULL
#define	PIP_PRT_TAGN_INC_PRT			0x0000000001000000ULL
#define	PIP_PRT_TAGN_IP6_DPRT			0x0000000000800000ULL
#define	PIP_PRT_TAGN_IP4_DPRT			0x0000000000400000ULL
#define	PIP_PRT_TAGN_IP6_SPRT			0x0000000000200000ULL
#define	PIP_PRT_TAGN_IP4_SPRT			0x0000000000100000ULL
#define	PIP_PRT_TAGN_IP6_NXTH			0x0000000000080000ULL
#define	PIP_PRT_TAGN_IP4_PCTL			0x0000000000040000ULL
#define	PIP_PRT_TAGN_IP6_DST			0x0000000000020000ULL
#define	PIP_PRT_TAGN_IP4_SRC			0x0000000000010000ULL
#define	PIP_PRT_TAGN_IP6_SRC			0x0000000000008000ULL
#define	PIP_PRT_TAGN_IP4_DST			0x0000000000004000ULL
#define	PIP_PRT_TAGN_TCP6_TAG			0x0000000000003000ULL
#define	 PIP_PRT_TAGN_TCP6_TAG			0x0000000000003000ULL
#define	  PIP_PRT_TAGN_TCP6_TAG_SHIFT		12
#define	   PIP_PRT_TAGN_TCP6_TAG_ORDERED	(0ULL << PIP_PRT_TAGN_TCP6_TAG_SHIFT)
#define	   PIP_PRT_TAGN_TCP6_TAG_ATOMIC		(1ULL << PIP_PRT_TAGN_TCP6_TAG_SHIFT)
#define	   PIP_PRT_TAGN_TCP6_TAG_NULL		(2ULL << PIP_PRT_TAGN_TCP6_TAG_SHIFT)
#define	   PIP_PRT_TAGN_TCP6_TAG_XXX_3		(3ULL << PIP_PRT_TAGN_TCP6_TAG_SHIFT)
#define	PIP_PRT_TAGN_TCP4_TAG			0x0000000000000c00ULL
#define	  PIP_PRT_TAGN_TCP4_TAG_SHIFT		10
#define	   PIP_PRT_TAGN_TCP4_TAG_ORDERED	(0ULL << PIP_PRT_TAGN_TCP4_TAG_SHIFT)
#define	   PIP_PRT_TAGN_TCP4_TAG_ATOMIC		(1ULL << PIP_PRT_TAGN_TCP4_TAG_SHIFT)
#define	   PIP_PRT_TAGN_TCP4_TAG_NULL		(2ULL << PIP_PRT_TAGN_TCP4_TAG_SHIFT)
#define	   PIP_PRT_TAGN_TCP4_TAG_XXX_3		(3ULL << PIP_PRT_TAGN_TCP4_TAG_SHIFT)
#define	PIP_PRT_TAGN_IP6_TAG			0x0000000000000300ULL
#define	  PIP_PRT_TAGN_IP6_TAG_SHIFT		8
#define	   PIP_PRT_TAGN_IP6_TAG_ORDERED		(0ULL << PIP_PRT_TAGN_IP6_TAG_SHIFT)
#define	   PIP_PRT_TAGN_IP6_TAG_ATOMIC		(1ULL << PIP_PRT_TAGN_IP6_TAG_SHIFT)
#define	   PIP_PRT_TAGN_IP6_TAG_NULL		(2ULL << PIP_PRT_TAGN_IP6_TAG_SHIFT)
#define	   PIP_PRT_TAGN_IP6_TAG_XXX_3		(3ULL << PIP_PRT_TAGN_IP6_TAG_SHIFT)
#define	PIP_PRT_TAGN_IP4_TAG			0x00000000000000c0ULL
#define	  PIP_PRT_TAGN_IP4_TAG_SHIFT		6
#define	   PIP_PRT_TAGN_IP4_TAG_ORDERED		(0ULL << PIP_PRT_TAGN_IP4_TAG_SHIFT)
#define	   PIP_PRT_TAGN_IP4_TAG_ATOMIC		(1ULL << PIP_PRT_TAGN_IP4_TAG_SHIFT)
#define	   PIP_PRT_TAGN_IP4_TAG_NULL		(2ULL << PIP_PRT_TAGN_IP4_TAG_SHIFT)
#define	   PIP_PRT_TAGN_IP4_TAG_XXX_3		(3ULL << PIP_PRT_TAGN_IP4_TAG_SHIFT)
#define	PIP_PRT_TAGN_NON_TAG			0x0000000000000030ULL
#define	  PIP_PRT_TAGN_NON_TAG_SHIFT		4
#define	   PIP_PRT_TAGN_NON_TAG_ORDERED		(0ULL << PIP_PRT_TAGN_NON_TAG_SHIFT)
#define	   PIP_PRT_TAGN_NON_TAG_ATOMIC		(1ULL << PIP_PRT_TAGN_NON_TAG_SHIFT)
#define	   PIP_PRT_TAGN_NON_TAG_NULL		(2ULL << PIP_PRT_TAGN_NON_TAG_SHIFT)
#define	   PIP_PRT_TAGN_NON_TAG_XXX_3		(3ULL << PIP_PRT_TAGN_NON_TAG_SHIFT)
#define	PIP_PRT_TAGN_GRP			0x000000000000000fULL

/*
 * PIP_QOS_DIFFN
 */
#define	PIP_QOS_DIFF_63_3			0xfffffffffffffff8ULL
#define	PIP_QOS_DIFF_QOS			0x0000000000000007ULL

/*
 * PIP_TAG_INCN
 */
#define	PIP_TAG_INCN_63_8			0xffffffffffffff00ULL
#define	PIP_TAG_INCN_EN				0x00000000000000ffULL

/*
 * PIP_STAT0_PRTN
 */
#define	PIP_STAT0_PRTN_DRP_PKTS			0xffffffff00000000ULL
#define	PIP_STAT0_PRTN_DRP_OCTS			0x00000000ffffffffULL

/*
 * PIP_STAT1_PRTN
 */
#define	PIP_STAT1_PRTN_63_48			0xffff000000000000ULL
#define	PIP_STAT1_PRTN_OCTS			0x0000ffffffffffffULL

/*
 * PIP_STAT2_PRTN
 */
#define	PIP_STAT2_PRTN_PKTS			0xffffffff00000000ULL
#define	PIP_STAT2_PRTN_RAW			0x00000000ffffffffULL

/*
 * PIP_STAT3_PRTN
 */
#define	PIP_STAT3_PRTN_BCST			0xffffffff00000000ULL
#define	PIP_STAT3_PRTN_MCST			0x00000000ffffffffULL

/*
 * PIP_STAT4_PRTN
 */
#define	PIP_STAT4_PRTN_H65TO127			0xffffffff00000000ULL
#define	PIP_STAT4_PRTN_H64			0x00000000ffffffffULL

/*
 * PIP_STAT5_PRTN
 */
#define	PIP_STAT5_PRTN_H256TO511		0xffffffff00000000ULL
#define	PIP_STAT5_PRTN_H128TO255		0x00000000ffffffffULL

/*
 * PIP_STAT6_PRTN
 */
#define	PIP_STAT6_PRTN_H1024TO1518		0xffffffff00000000ULL
#define	PIP_STAT6_PRTN_H512TO1023		0x00000000ffffffffULL

/*
 * PIP_STAT7_PRTN
 */
#define	PIP_STAT7_PRTN_FCS			0xffffffff00000000ULL
#define	PIP_STAT7_PRTN_H1519			0x00000000ffffffffULL

/*
 * PIP_STAT8_PRTN
 */
#define	PIP_STAT8_PRTN_FRAG			0xffffffff00000000ULL
#define	PIP_STAT8_PRTN_UNDERSZ			0x00000000ffffffffULL

/*
 * PIP_STAT9_PRTN
 */
#define	PIP_STAT9_PRTN_JABBER			0xffffffff00000000ULL
#define	PIP_STAT9_PRTN_OVERSZ			0x00000000ffffffffULL

/*
 * PIP_STAT_INB_PKTN
 */
#define	PIP_STAT_INB_PKTSN			0xffffffff00000000ULL
#define	PIP_STAT_INB_PKTSN_PKTS			0x00000000ffffffffULL

/*
 * PIP_STAT_INB_OCTSN
 */
#define	PIP_STAT_INB_OCTSN			0xffff000000000000ULL
#define	PIP_STAT_INB_OCTSN_OCTS			0x0000ffffffffffffULL

/*
 * PIP_STAT_INB_ERRS
 */
#define	PIP_STAT_INB_ERRSN			0xffffffffffff0000ULL
#define	PIP_STAT_INB_ERRSN_OCTS			0x000000000000ffffULL

/*
 * Work-Queue Entry Format
 */
/* WORD0 */
#define PIP_WQE_WORD0_HW_CSUM			0xffff000000000000ULL
#define PIP_WQE_WORD0_47_40			0x0000ff0000000000ULL
#define PIP_WQE_WORD0_POW_NEXT_PTR		0x000000ffffffffffULL

/* WORD 1 */
#define PIP_WQE_WORD1_LEN			0xffff000000000000ULL
#define PIP_WQE_WORD1_IPRT			0x0000fc0000000000ULL
#define PIP_WQE_WORD1_QOS			0x0000038000000000ULL
#define PIP_WQE_WORD1_GRP			0x0000007800000000ULL
#define PIP_WQE_WORD1_TT			0x0000000700000000ULL
#define PIP_WQE_WORD1_TAG			0x00000000ffffffffULL

/* WORD 2 */
#define PIP_WQE_WORD2_RAWFULL_BUFS		0xff00000000000000ULL
#define PIP_WQE_WORD2_RAWFULL_PIP_RAW_WORD	0x00ffffffffffffffULL

#define PIP_WQE_WORD2_IP_BUFS			0xff00000000000000ULL
#define   PIP_WQE_WORD2_IP_BUFS_SHIFT		56
#define PIP_WQE_WORD2_IP_OFFSET			0x00ff000000000000ULL
#define   PIP_WQE_WORD2_IP_OFFSET_SHIFT		48
#define PIP_WQE_WORD2_IP_VV			0x0000800000000000ULL
#define PIP_WQE_WORD2_IP_VS			0x0000400000000000ULL
#define PIP_WQE_WORD2_IP_45			0x0000200000000000ULL
#define PIP_WQE_WORD2_IP_VC			0x0000100000000000ULL
#define PIP_WQE_WORD2_IP_VLAN_ID		0x00000fff00000000ULL
#define PIP_WQE_WORD2_IP_31_20			0x00000000fff00000ULL
#define PIP_WQE_WORD2_IP_CO			0x0000000000080000ULL
#define PIP_WQE_WORD2_IP_TU			0x0000000000040000ULL
#define PIP_WQE_WORD2_IP_SE			0x0000000000020000ULL
#define PIP_WQE_WORD2_IP_V6			0x0000000000010000ULL
#define PIP_WQE_WORD2_IP_15			0x0000000000008000ULL
#define PIP_WQE_WORD2_IP_LE			0x0000000000004000ULL
#define PIP_WQE_WORD2_IP_FR			0x0000000000002000ULL
#define PIP_WQE_WORD2_IP_IE			0x0000000000001000ULL
#define PIP_WQE_WORD2_IP_B			0x0000000000000800ULL
#define PIP_WQE_WORD2_IP_M			0x0000000000000400ULL
#define PIP_WQE_WORD2_IP_NI			0x0000000000000200ULL
#define PIP_WQE_WORD2_IP_RE			0x0000000000000100ULL
#define PIP_WQE_WORD2_IP_OPECODE		0x00000000000000ffULL

#define PIP_WQE_WORD2_NOIP_BUFS			0xff00000000000000ULL
#define PIP_WQE_WORD2_NOIP_55_48		0x00ff000000000000ULL
#define PIP_WQE_WORD2_NOIP_VV			0x0000800000000000ULL
#define PIP_WQE_WORD2_NOIP_VS			0x0000400000000000ULL
#define PIP_WQE_WORD2_NOIP_45			0x0000200000000000ULL
#define PIP_WQE_WORD2_NOIP_VC			0x0000100000000000ULL
#define PIP_WQE_WORD2_NOIP_VLAN_ID		0x00000fff00000000ULL
#define PIP_WQE_WORD2_NOIP_31_14		0x00000000ffffc000ULL
#define PIP_WQE_WORD2_NOIP_IR			0x0000000000002000ULL
#define PIP_WQE_WORD2_NOIP_IA			0x0000000000001000ULL
#define PIP_WQE_WORD2_NOIP_B			0x0000000000000800ULL
#define PIP_WQE_WORD2_NOIP_M			0x0000000000000400ULL
#define PIP_WQE_WORD2_NOIP_NI			0x0000000000000200ULL
#define PIP_WQE_WORD2_NOIP_RE			0x0000000000000100ULL
#define PIP_WQE_WORD2_NOIP_OPECODE		0x00000000000000ffULL

/* WORD 3 */
#define PIP_WQE_WORD3_63			0x8000000000000000ULL
#define PIP_WQE_WORD3_BACK			0x7800000000000000ULL
#define   PIP_WQE_WORD3_BACK_SHIFT		59
#define PIP_WQE_WORD3_58_56			0x0700000000000000ULL
#define PIP_WQE_WORD3_SIZE			0x00ffff0000000000ULL
#define PIP_WQE_WORD3_ADDR			0x000000ffffffffffULL

/* opcode for WORD2[LE] */
#define PIP_WQE_WORD2_LE_OPCODE_MAL		1ULL
#define PIP_WQE_WORD2_LE_OPCODE_CSUM		2ULL
#define PIP_WQE_WORD2_LE_OPCODE_UDPLEN		3ULL
#define PIP_WQE_WORD2_LE_OPCODE_PORT		4ULL
#define PIP_WQE_WORD2_LE_OPCODE_XXX_5		5ULL
#define PIP_WQE_WORD2_LE_OPCODE_XXX_6		6ULL
#define PIP_WQE_WORD2_LE_OPCODE_XXX_7		7ULL
#define PIP_WQE_WORD2_LE_OPCODE_FINO		8ULL
#define PIP_WQE_WORD2_LE_OPCODE_NOFL		9ULL
#define PIP_WQE_WORD2_LE_OPCODE_FINRST		10ULL
#define PIP_WQE_WORD2_LE_OPCODE_SYNURG		11ULL
#define PIP_WQE_WORD2_LE_OPCODE_SYNRST		12ULL
#define PIP_WQE_WORD2_LE_OPCODE_SYNFIN		13ULL

/* opcode for WORD2[IE] */
#define PIP_WQE_WORD2_IE_OPCODE_NOTIP		1ULL
#define PIP_WQE_WORD2_IE_OPCODE_CSUM		2ULL
#define PIP_WQE_WORD2_IE_OPCODE_MALHDR		3ULL
#define PIP_WQE_WORD2_IE_OPCODE_MAL		4ULL
#define PIP_WQE_WORD2_IE_OPCODE_TTL		5ULL
#define PIP_WQE_WORD2_IE_OPCODE_OPT		6ULL

/* opcode for WORD2[RE] */
#define PIP_WQE_WORD2_RE_OPCODE_PARTIAL		1ULL
#define PIP_WQE_WORD2_RE_OPCODE_JABBER		2ULL
#define PIP_WQE_WORD2_RE_OPCODE_OVRRUN		3ULL
#define PIP_WQE_WORD2_RE_OPCODE_OVRSZ		4ULL
#define PIP_WQE_WORD2_RE_OPCODE_ALIGN		5ULL
#define PIP_WQE_WORD2_RE_OPCODE_FRAG		6ULL
#define PIP_WQE_WORD2_RE_OPCODE_GMXFCS		7ULL
#define PIP_WQE_WORD2_RE_OPCODE_UDRSZ		8ULL
#define PIP_WQE_WORD2_RE_OPCODE_EXTEND		9ULL
#define PIP_WQE_WORD2_RE_OPCODE_LENGTH		10ULL
#define PIP_WQE_WORD2_RE_OPCODE_MIIRX		11ULL 
#define PIP_WQE_WORD2_RE_OPCODE_MIISKIP		12ULL
#define PIP_WQE_WORD2_RE_OPCODE_MIINBL		13ULL
#define PIP_WQE_WORD2_RE_OPCODE_XXX_14		14ULL
#define PIP_WQE_WORD2_RE_OPCODE_XXX_15		15ULL
#define PIP_WQE_WORD2_RE_OPCODE_XXX_16		16ULL
#define PIP_WQE_WORD2_RE_OPCODE_SKIP		17ULL
#define PIP_WQE_WORD2_RE_OPCODE_L2MAL		18ULL

#endif /* _CN30XXPIPREG_H_ */
