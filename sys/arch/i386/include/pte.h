/*	$OpenBSD: pte.h,v 1.8 2006/04/27 15:37:53 mickey Exp $	*/
/*	$NetBSD: pte.h,v 1.11 1998/02/06 21:58:05 thorpej Exp $	*/

/*
 *
 * Copyright (c) 1997 Charles D. Cranor and Washington University.
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
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgment:
 *      This product includes software developed by Charles D. Cranor and
 *      Washington University.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * pte.h rewritten by chuck based on the jolitz version, plus random
 * info on the pentium and other processors found on the net.   the
 * goal of this rewrite is to provide enough documentation on the MMU
 * hardware that the reader will be able to understand it without having
 * to refer to a hardware manual.
 */

#ifndef _I386_PTE_H_
#define _I386_PTE_H_

/*
 * now we define various for playing with virtual addresses
 */

#define	PDSHIFT		22		/* offset of PD index in VA */
#define	NBPD		(1 << PDSHIFT)	/* # bytes mapped by PD (4MB) */

/*
 * here we define the bits of the PDE/PTE, as described above:
 *
 * XXXCDC: need to rename these (PG_u == ugly).
 */

#define	PG_V		0x00000001	/* valid entry */
#define	PG_RO		0x00000000	/* read-only page */
#define	PG_RW		0x00000002	/* read-write page */
#define	PG_u		0x00000004	/* user accessible page */
#define	PG_PROT		0x00000806	/* all protection bits */
#define	PG_N		0x00000018	/* non-cacheable */
#define	PG_U		0x00000020	/* has been used */
#define	PG_M		0x00000040	/* has been modified */
#define PG_PS		0x00000080	/* 4MB page size */
#define PG_G		0x00000100	/* global, don't TLB flush */
#define PG_AVAIL1	0x00000200	/* ignored by hardware */
#define PG_AVAIL2	0x00000400	/* ignored by hardware */
#define PG_AVAIL3	0x00000800	/* ignored by hardware */

/*
 * various short-hand protection codes
 */

#define	PG_KR		0x00000000	/* kernel read-only */
#define	PG_KW		0x00000002	/* kernel read-write */

/*
 * page protection exception bits
 */

#define PGEX_P		0x01	/* protection violation (vs. no mapping) */
#define PGEX_W		0x02	/* exception during a write cycle */
#define PGEX_U		0x04	/* exception while in user mode (upl) */
#define PGEX_I		0x10	/* instruction fetch blocked by NX */

#endif /* _I386_PTE_H_ */
