/*
 * THIS FILE IS AUTOMATICALLY GENERATED
 * DONT EDIT THIS FILE
 */

/*	$OpenBSD: cn30xxipdreg.h,v 1.2 2014/08/11 18:29:56 miod Exp $	*/

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
 * 7.9 IPD Registers
 */

#ifndef _CN30XXIPDREG_H_
#define _CN30XXIPDREG_H_

#define	IPD_1ST_MBUFF_SKIP		0x00014f0000000000ULL
#define	IPD_NOT_1ST_MBUFF_SKIP		0x00014f0000000008ULL
#define	IPD_PACKET_MBUFF_SIZE		0x00014f0000000010ULL
#define	IPD_CTL_STATUS			0x00014f0000000018ULL
#define	IPD_WQE_FPA_QUEUE		0x00014f0000000020ULL
#define	IPD_PORT0_BP_PAGE_CNT		0x00014f0000000028ULL
#define	IPD_PORT1_BP_PAGE_CNT		0x00014f0000000030ULL
#define	IPD_PORT2_BP_PAGE_CNT		0x00014f0000000038ULL
#define	IPD_PORT32_BP_PAGE_CNT		0x00014f0000000128ULL
#define	IPD_SUB_PORT_BP_PAGE_CNT	0x00014f0000000148ULL
#define	IPD_1ST_NEXT_PTR_BACK		0x00014f0000000150ULL
#define	IPD_2ND_NEXT_PTR_BACK		0x00014f0000000158ULL
#define	IPD_INT_ENB			0x00014f0000000160ULL
#define	IPD_INT_SUM			0x00014f0000000168ULL
#define	IPD_SUB_PORT_FCS		0x00014f0000000170ULL
#define	IPD_QOS0_RED_MARKS		0x00014f0000000178ULL
#define	IPD_QOS1_RED_MARKS		0x00014f0000000180ULL
#define	IPD_QOS2_RED_MARKS		0x00014f0000000188ULL
#define	IPD_QOS3_RED_MARKS		0x00014f0000000190ULL
#define	IPD_QOS4_RED_MARKS		0x00014f0000000198ULL
#define	IPD_QOS5_RED_MARKS		0x00014f00000001a0ULL
#define	IPD_QOS6_RED_MARKS		0x00014f00000001a8ULL
#define	IPD_QOS7_RED_MARKS		0x00014f00000001b0ULL
#define	IPD_PORT_BP_COUNTERS_PAIR0	0x00014f00000001b8ULL
#define	IPD_PORT_BP_COUNTERS_PAIR1	0x00014f00000001c0ULL
#define	IPD_PORT_BP_COUNTERS_PAIR2	0x00014f00000001c8ULL
#define	IPD_PORT_BP_COUNTERS_PAIR32	0x00014f00000002b8ULL
#define	IPD_RED_PORT_ENABLE		0x00014f00000002d8ULL
#define	IPD_RED_QUE0_PARAM		0x00014f00000002e0ULL
#define	IPD_RED_QUE1_PARAM		0x00014f00000002e8ULL
#define	IPD_RED_QUE2_PARAM		0x00014f00000002f0ULL
#define	IPD_RED_QUE3_PARAM		0x00014f00000002f8ULL
#define	IPD_RED_QUE4_PARAM		0x00014f0000000300ULL
#define	IPD_RED_QUE5_PARAM		0x00014f0000000308ULL
#define	IPD_RED_QUE6_PARAM		0x00014f0000000310ULL
#define	IPD_RED_QUE7_PARAM		0x00014f0000000318ULL
#define	IPD_PTR_COUNT			0x00014f0000000320ULL
#define	IPD_BP_PRT_RED_END		0x00014f0000000328ULL
#define	IPD_QUE0_FREE_PAGE_CNT		0x00014f0000000330ULL
#define	IPD_CLK_COUNT			0x00014f0000000338ULL
#define	IPD_PWP_PTR_FIFO_CTL		0x00014f0000000340ULL
#define	IPD_PRC_HOLD_PTR_FIFO_CTL	0x00014f0000000348ULL
#define	IPD_PRC_PORT_PTR_FIFO_CTL	0x00014f0000000350ULL
#define	IPD_PKT_PTR_VALID		0x00014f0000000358ULL
#define	IPD_WQE_PTR_VALID		0x00014f0000000360ULL
#define	IPD_BIST_STATUS			0x00014f00000007f8ULL

#define	IPD_BASE			0x00014f0000000000ULL
#define	IPD_SIZE			0x800ULL

#define	IPD_1ST_MBUFF_SKIP_OFFSET		0x0ULL
#define	IPD_NOT_1ST_MBUFF_SKIP_OFFSET		0x8ULL
#define	IPD_PACKET_MBUFF_SIZE_OFFSET		0x10ULL
#define	IPD_CTL_STATUS_OFFSET			0x18ULL
#define	IPD_WQE_FPA_QUEUE_OFFSET		0x20ULL
#define	IPD_PORT0_BP_PAGE_CNT_OFFSET		0x28ULL
#define	IPD_PORT1_BP_PAGE_CNT_OFFSET		0x30ULL
#define	IPD_PORT2_BP_PAGE_CNT_OFFSET		0x38ULL
#define	IPD_PORT32_BP_PAGE_CNT_OFFSET		0x128ULL
#define	IPD_SUB_PORT_BP_PAGE_CNT_OFFSET		0x148ULL
#define	IPD_1ST_NEXT_PTR_BACK_OFFSET		0x150ULL
#define	IPD_2ND_NEXT_PTR_BACK_OFFSET		0x158ULL
#define	IPD_INT_ENB_OFFSET			0x160ULL
#define	IPD_INT_SUM_OFFSET			0x168ULL
#define	IPD_SUB_PORT_FCS_OFFSET			0x170ULL
#define	IPD_QOS0_RED_MARKS_OFFSET		0x178ULL
#define	IPD_QOS1_RED_MARKS_OFFSET		0x180ULL
#define	IPD_QOS2_RED_MARKS_OFFSET		0x188ULL
#define	IPD_QOS3_RED_MARKS_OFFSET		0x190ULL
#define	IPD_QOS4_RED_MARKS_OFFSET		0x198ULL
#define	IPD_QOS5_RED_MARKS_OFFSET		0x1a0ULL
#define	IPD_QOS6_RED_MARKS_OFFSET		0x1a8ULL
#define	IPD_QOS7_RED_MARKS_OFFSET		0x1b0ULL
#define	IPD_PORT_BP_COUNTERS_PAIR0_OFFSET	0x1b8ULL
#define	IPD_PORT_BP_COUNTERS_PAIR1_OFFSET	0x1c0ULL
#define	IPD_PORT_BP_COUNTERS_PAIR2_OFFSET	0x1c8ULL
#define	IPD_PORT_BP_COUNTERS_PAIR32_OFFSET	0x2b8ULL
#define	IPD_RED_PORT_ENABLE_OFFSET		0x2d8ULL
#define	IPD_RED_QUE0_PARAM_OFFSET		0x2e0ULL
#define	IPD_RED_QUE1_PARAM_OFFSET		0x2e8ULL
#define	IPD_RED_QUE2_PARAM_OFFSET		0x2f0ULL
#define	IPD_RED_QUE3_PARAM_OFFSET		0x2f8ULL
#define	IPD_RED_QUE4_PARAM_OFFSET		0x300ULL
#define	IPD_RED_QUE5_PARAM_OFFSET		0x308ULL
#define	IPD_RED_QUE6_PARAM_OFFSET		0x310ULL
#define	IPD_RED_QUE7_PARAM_OFFSET		0x318ULL
#define	IPD_PTR_COUNT_OFFSET			0x320ULL
#define	IPD_BP_PRT_RED_END_OFFSET		0x328ULL
#define	IPD_QUE0_FREE_PAGE_CNT_OFFSET		0x330ULL
#define	IPD_CLK_COUNT_OFFSET			0x338ULL
#define	IPD_PWP_PTR_FIFO_CTL_OFFSET		0x340ULL
#define	IPD_PRC_HOLD_PTR_FIFO_CTL_OFFSET	0x348ULL
#define	IPD_PRC_PORT_PTR_FIFO_CTL_OFFSET	0x350ULL
#define	IPD_PKT_PTR_VALID_OFFSET		0x358ULL
#define	IPD_WQE_PTR_VALID_OFFSET		0x360ULL
#define	IPD_BIST_STATUS_OFFSET			0x7f8ULL

/* ----- */
/*
 * Work Queue Entry Format (for input packet)
 * 7.5 Work Queue Entry
 * Figure 7-8. PIP/IPD Hardware Work-Queue Entry
 */

/* 
 * word 2
 * Figure. 7-9 Work-Queue Entry format; Word2 Cases
 */
/* RAWFULL */
#define IPD_WQE_WORD2_RAW_BUFS		0xff00000000000000ULL
#define IPD_WQE_WORD2_RAW_WORD		0x00ffffffffffffffULL

/* is IP */
#define IPD_WQE_WORD2_IP_BUFS		0xff00000000000000ULL
#define IPD_WQE_WORD2_IP_IPOFF		0x00ff000000000000ULL
#define IPD_WQE_WORD2_IP_VV		0x0000800000000000ULL
#define IPD_WQE_WORD2_IP_VS		0x0000400000000000ULL
#define IPD_WQE_WORD2_IP_45		0x0000200000000000ULL
#define IPD_WQE_WORD2_IP_VC		0x0000100000000000ULL
#define IPD_WQE_WORD2_IP_VLANID		0x00000fff00000000ULL
#define IPD_WQE_WORD2_IP_31_20		0x00000000fff00000ULL
#define IPD_WQE_WORD2_IP_CO		0x0000000000080000ULL
#define IPD_WQE_WORD2_IP_TU		0x0000000000040000ULL
#define IPD_WQE_WORD2_IP_SE		0x0000000000020000ULL
#define IPD_WQE_WORD2_IP_V6		0x0000000000010000ULL
#define IPD_WQE_WORD2_IP_15		0x0000000000008000ULL
#define IPD_WQE_WORD2_IP_LE		0x0000000000004000ULL
#define IPD_WQE_WORD2_IP_FR		0x0000000000002000ULL
#define IPD_WQE_WORD2_IP_IE		0x0000000000001000ULL
#define IPD_WQE_WORD2_IP_B		0x0000000000000800ULL
#define IPD_WQE_WORD2_IP_M		0x0000000000000400ULL
#define IPD_WQE_WORD2_IP_NI		0x0000000000000200ULL
#define IPD_WQE_WORD2_IP_RE		0x0000000000000100ULL
#define IPD_WQE_WORD2_IP_OPCODE		0x00000000000000ffULL

/* All other */
#define IPD_WQE_WORD2_OTH_BUFS		0xff00000000000000ULL
#define IPD_WQE_WORD2_OTH_55_48		0x00ff000000000000ULL
#define IPD_WQE_WORD2_OTH_VV		0x0000800000000000ULL
#define IPD_WQE_WORD2_OTH_VS		0x0000400000000000ULL
#define IPD_WQE_WORD2_OTH_45		0x0000200000000000ULL
#define IPD_WQE_WORD2_OTH_VC		0x0000100000000000ULL
#define IPD_WQE_WORD2_OTH_VLANID	0x00000fff00000000ULL
#define IPD_WQE_WORD2_OTH_31_14		0x00000000ffffc000ULL
#define IPD_WQE_WORD2_OTH_IR		0x0000000000002000ULL
#define IPD_WQE_WORD2_OTH_IA		0x0000000000001000ULL
#define IPD_WQE_WORD2_OTH_B		0x0000000000000800ULL
#define IPD_WQE_WORD2_OTH_M		0x0000000000000400ULL
#define IPD_WQE_WORD2_OTH_NI		0x0000000000000200ULL
#define IPD_WQE_WORD2_OTH_RE		0x0000000000000100ULL
#define IPD_WQE_WORD2_OTH_OPCODE	0x00000000000000ffULL

/*
 * word 3
 */
#define IPD_WQE_WORD3_63		0x8000000000000000ULL
#define IPD_WQE_WORD3_BACK		0x7800000000000000ULL
#define IPD_WQE_WORD3_58_56		0x0700000000000000ULL
#define IPD_WQE_WORD3_SIZE		0x00ffff0000000000ULL
#define IPD_WQE_WORD3_ADDR		0x000000ffffffffffULL

/*
 * IPD_1ST_MBUFF_SKIP 
 */
#define IPD_1ST_MBUFF_SKIP_63_6		0xffffffffffffffc0ULL
#define IPD_1ST_MBUFF_SKIP_SZ		0x000000000000003fULL

/*
 * IPD_NOT_1ST_MBUFF_SKIP 
 */
#define IPD_NOT_1ST_MBUFF_SKIP_63_6	0xffffffffffffffc0ULL
#define IPD_NOT_1ST_MBUFF_SKIP_SZ	0x000000000000003fULL

/*
 * IPD_PACKET_MBUFF_SIZE 
 */
#define IPD_PACKET_MBUFF_SIZE_63_12	0xfffffffffffff000ULL
#define IPD_PACKET_MBUFF_SIZE_MB_SIZE	0x0000000000000fffULL

/*
 * IPD_CTL_STATUS 
 */
#define IPD_CTL_STATUS_63_10		0xfffffffffffffc00ULL
#define IPD_CTL_STATUS_LEN_M8		0x0000000000000200ULL
#define IPD_CTL_STATUS_RESET		0x0000000000000100ULL
#define IPD_CTL_STATUS_ADDPKT		0x0000000000000080ULL
#define IPD_CTL_STATUS_NADDBUF		0x0000000000000040ULL
#define IPD_CTL_STATUS_PKT_LEND		0x0000000000000020ULL
#define IPD_CTL_STATUS_WQE_LEND		0x0000000000000010ULL
#define IPD_CTL_STATUS_PBP_EN		0x0000000000000008ULL
#define IPD_CTL_STATUS_OPC_MODE		0x0000000000000006ULL
#define  IPD_CTL_STATUS_OPC_MODE_SHIFT	1
#define   IPD_CTL_STATUS_OPC_MODE_NONE	(0ULL << IPD_CTL_STATUS_OPC_MODE_SHIFT)
#define   IPD_CTL_STATUS_OPC_MODE_ALL	(1ULL << IPD_CTL_STATUS_OPC_MODE_SHIFT)
#define   IPD_CTL_STATUS_OPC_MODE_ONE	(2ULL << IPD_CTL_STATUS_OPC_MODE_SHIFT)
#define   IPD_CTL_STATUS_OPC_MODE_TWO	(3ULL << IPD_CTL_STATUS_OPC_MODE_SHIFT)
#define IPD_CTL_STATUS_IPD_EN		0x0000000000000001ULL

/*
 * IPD_WQE_FPA_QUEUE
 */
#define IPD_WQE_FPA_QUEUE_63_3		0xfffffffffffffff8ULL
#define IPD_WQE_FPA_QUEUE_WQE_QUE	0x0000000000000007ULL

/*
 * IPD_PORTN_BP_PAGE_CNT
 */
#define IPD_PORTN_BP_PAGE_CNT_63_18	0xfffffffffffc0000ULL
#define IPD_PORTN_BP_PAGE_CNT_BP_ENB	0x0000000000020000ULL
#define IPD_PORTN_BP_PAGE_CNT_PAGE_CNT	0x000000000001ffffULL

/*
 * IPD_SUB_PORT_BP_PAGE_CNT
 */
#define IPD_SUB_PORT_BP_PAGE_CNT_63_18		0xffffffff80000000ULL
#define IPD_SUB_PORT_BP_PAGE_CNT_PORT		0x000000007e000000ULL
#define IPD_SUB_PORT_BP_PAGE_CNT_PAGE_CNT	0x0000000001ffffffULL

/*
 * IPD_1ST_NEXT_PTR_BACK
 */
#define IPD_1ST_NEXT_PTR_BACK_63_4		0xfffffffffffffff0ULL
#define IPD_1ST_NEXT_PTR_BACK_BACK		0x000000000000000fULL

/*
 * IPD_2ND_NEXT_PTR_BACK
 */
#define IPD_2ND_NEXT_PTR_BACK_63_4		0xfffffffffffffff0ULL
#define IPD_2ND_NEXT_PTR_BACK_BACK		0x000000000000000fULL

/*
 * IPD_INT_ENB
 */
#define IPD_INT_ENB_63_4		0xffffffffffffffe0ULL
#define IPD_INT_ENB_BP_SUB		0x0000000000000010ULL
#define IPD_INT_ENB_PRC_PAR3		0x0000000000000008ULL
#define IPD_INT_ENB_PRC_PAR2		0x0000000000000004ULL
#define IPD_INT_ENB_PRC_PAR1		0x0000000000000002ULL
#define IPD_INT_ENB_PRC_PAR0		0x0000000000000001ULL

/*
 * IPD_INT_SUM
 */
#define IPD_INT_SUM_63_4		0xffffffffffffffe0ULL
#define IPD_INT_SUM_BP_SUB		0x0000000000000010ULL
#define IPD_INT_SUM_PRC_PAR3		0x0000000000000008ULL
#define IPD_INT_SUM_PRC_PAR2		0x0000000000000004ULL
#define IPD_INT_SUM_PRC_PAR1		0x0000000000000002ULL
#define IPD_INT_SUM_PRC_PAR0		0x0000000000000001ULL

/*
 * IPD_SUB_PORT_FCS
 */
#define IPD_SUB_PORT_FCS_63_3		0xfffffffffffffff8ULL
#define IPD_SUB_PORT_FCS_PORT_BIT	0x0000000000000007ULL

/*
 * IPD_QOSN_RED_MARKS
 */
#define IPD_QOSN_READ_MARKS_DROP	0xffffffff00000000ULL
#define IPD_QOSN_READ_MARKS_PASS	0x00000000ffffffffULL

/*
 * IPD_PORT_BP_COUNTERS_PAIRN
 */
#define IPD_PORT_BP_COUNTERS_PAIRN_63_25	0xfffffffffe000000ULL
#define IPD_PORT_BP_COUNTERS_PAIRN_CNT_VAL	0x0000000001ffffffULL

/*
 * IPD_RED_PORT_ENABLE
 */
#define IPD_RED_PORT_ENABLE_PRB_DLY	0xfffc000000000000ULL
#define IPD_RED_PORT_ENABLE_AVG_DLY	0x0003fff000000000ULL
#define IPD_RED_PORT_ENABLE_PRT_ENB	0x0000000fffffffffULL

/*
 * IPD_RED_QUEN_PARAM
 */
#define IPD_RED_QUEN_PARAM_63_49	0xfffe000000000000ULL
#define IPD_RED_QUEN_PARAM_USE_PCNT	0x0001000000000000ULL
#define IPD_RED_QUEN_PARAM_NEW_CON	0x0000ff0000000000ULL
#define IPD_RED_QUEN_PARAM_AVG_CON	0x000000ff00000000ULL
#define IPD_RED_QUEN_PARAM_PRB_CON	0x00000000ffffffffULL

/*
 * IPD_PTR_COUNT
 */
#define IPD_PTR_COUNT_63_19		0xfffffffffff80000ULL
#define IPD_PTR_COUNT_PKTV_CNT		0x0000000000040000ULL
#define IPD_PTR_COUNT_WQEV_CNT		0x0000000000020000ULL
#define IPD_PTR_COUNT_PFIF_CNT		0x000000000001c000ULL
#define IPD_PTR_COUNT_PKT_PCNT		0x0000000000003f80ULL
#define IPD_PTR_COUNT_WQE_PCNT		0x000000000000007fULL

/*
 * IPD_BP_PRT_RED_END
 */
#define IPD_BP_PRT_RED_END_63_36	0xfffffff000000000ULL
#define IPD_BP_PRT_RED_END_PRT_ENB	0x0000000fffffffffULL

/*
 * IPD_QUE0_FREE_PAGE_CNT
 */
#define IPD_QUE0_FREE_PAGE_CNT_63_32	0xffffffff00000000ULL
#define IPD_QUE0_FREE_PAGE_CNT_Q0_PCNT	0x00000000ffffffffULL

/*
 * IPD_CLK_COUNT
 */
#define IPD_CLK_COUNT_CLK_CNT		0xffffffffffffffffULL

/*
 * IPD_PWP_PTR_FIFO_CTL
 */
#define IPD_PWP_PTR_FIFO_CTL_63_61	0xe000000000000000ULL
#define IPD_PWP_PTR_FIFO_CTL_MAX_CNTS	0x1fc0000000000000ULL
#define IPD_PWP_PTR_FIFO_CTL_WRADDR	0x003fc00000000000ULL
#define IPD_PWP_PTR_FIFO_CTL_PRADDR	0x00003fc000000000ULL
#define IPD_PWP_PTR_FIFO_CTL_PTR	0x0000003ffffffe00ULL
#define IPD_PWP_PTR_FIFO_CTL_CENA	0x0000000000000100ULL
#define IPD_PWP_PTR_FIFO_CTL_RADDR	0x00000000000000ffULL

/*
 * IPD_PRC_HOLD_PTR_FIFO_CTL
 */
#define IPD_PRC_HOLD_PTR_FIFO_CTL_63_39		0xffffff8000000000ULL
#define IPD_PRC_HOLD_PTR_FIFO_CTL_MAX_PTR	0x0000007000000000ULL
#define IPD_PRC_HOLD_PTR_FIFO_CTL_PRADDR	0x0000000e00000000ULL
#define IPD_PRC_HOLD_PTR_FIFO_CTL_PTR		0x00000001fffffff0ULL
#define IPD_PRC_HOLD_PTR_FIFO_CTL_CENA		0x0000000000000008ULL
#define IPD_PRC_HOLD_PTR_FIFO_CTL_RADDR		0x0000000000000007ULL

/*
 * IPD_PRC_PORT_PTR_FIFO_CTL
 */
#define IPD_PRC_PORT_PTR_FIFO_CTL_63_44		0xfffff00000000000ULL
#define IPD_PRC_PORT_PTR_FIFO_CTL_MAX_PTR	0x00000fe000000000ULL
#define IPD_PRC_PORT_PTR_FIFO_CTL_PTR		0x0000001fffffff00ULL
#define IPD_PRC_PORT_PTR_FIFO_CTL_CENA		0x0000000000000080ULL
#define IPD_PRC_PORT_PTR_FIFO_CTL_RADDR		0x000000000000007fULL

/*
 * IPD_PKT_PTR_VALID
 */
#define IPD_PKT_PTR_VALID_63_29	0xffffffffe0000000ULL
#define IPD_PKT_PTR_VALID_PTR	0x000000001fffffffULL

/*
 * IPD_WQE_PTR_VALID
 */
#define IPD_WQE_PTR_VALID_63_29	0xffffffffe0000000ULL
#define IPD_WQE_PTR_VALID_PTR	0x000000001fffffffULL

/*
 * IPD_BIST_STATUS
 */
#define IPD_BIST_STATUS_63_29		0xffffffffffff0000ULL
#define IPD_BIST_STATUS_PWQ_WQED	0x0000000000008000ULL
#define IPD_BIST_STATUS_PWQ_WP1		0x0000000000004000ULL
#define IPD_BIST_STATUS_PWQ_POW		0x0000000000002000ULL
#define IPD_BIST_STATUS_IPQ_PBE1	0x0000000000001000ULL
#define IPD_BIST_STATUS_IPQ_PBE0	0x0000000000000800ULL
#define IPD_BIST_STATUS_PBM3		0x0000000000000400ULL
#define IPD_BIST_STATUS_PBM2		0x0000000000000200ULL
#define IPD_BIST_STATUS_PBM1		0x0000000000000100ULL
#define IPD_BIST_STATUS_PBM0		0x0000000000000080ULL
#define IPD_BIST_STATUS_PBM_WORD	0x0000000000000040ULL
#define IPD_BIST_STATUS_PWQ1		0x0000000000000020ULL
#define IPD_BIST_STATUS_PWQ0		0x0000000000000010ULL
#define IPD_BIST_STATUS_PRC_OFF		0x0000000000000008ULL
#define IPD_BIST_STATUS_IPD_OLD		0x0000000000000004ULL
#define IPD_BIST_STATUS_IPD_NEW		0x0000000000000002ULL
#define IPD_BIST_STATUS_PWP		0x0000000000000001ULL

/*
 * word2[Opcode]
 */
/* L3 (IP) error */
#define IPD_WQE_L3_NOT_IP		1
#define IPD_WQE_L3_V4_CSUM_ERR		2
#define IPD_WQE_L3_HEADER_MALFORMED	3
#define IPD_WQE_L3_MELFORMED		4
#define IPD_WQE_L3_TTL_HOP		5
#define IPD_WQE_L3_IP_OPT		6

/* L4 (UDP/TCP) error */
#define IPD_WQE_L4_MALFORMED		1
#define IPD_WQE_L4_CSUM_ERR		2
#define IPD_WQE_L4_UDP_LEN_ERR		3
#define IPD_WQE_L4_BAD_PORT		4
#define IPD_WQE_L4_FIN_ONLY		8
#define IPD_WQE_L4_NO_FLAGS		9
#define IPD_WQE_L4_FIN_RST		10
#define IPD_WQE_L4_SYN_URG		11
#define IPD_WQE_L4_SYN_RST		12
#define IPD_WQE_L4_SYN_FIN		13

#endif /* _CN30XXIPDREG_H_ */
