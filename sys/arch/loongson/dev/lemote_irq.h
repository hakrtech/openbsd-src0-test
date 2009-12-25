/*	$OpenBSD: lemote_irq.h,v 1.1.1.1 2009/12/25 21:11:07 miod Exp $	*/

/*
 * Copyright (c) 2009 Miodrag Vallat.
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

/*
 * Lemote Yeelong specific hardware defines
 */

/*
 * Bonito interrupt assignments
 */

#define	YEELONG_INTR_GPIO0		0
#define	YEELONG_INTR_GPIO1		1
#define	YEELONG_INTR_GPIO2		2
#define	YEELONG_INTR_GPIO3		3

/* pci interrupts */
#define	YEELONG_INTR_PCIA		4
#define	YEELONG_INTR_PCIB		5
#define	YEELONG_INTR_PCIC		6
#define	YEELONG_INTR_PCID		7

#define	YEELONG_INTR_PCI_PARERR		8
#define	YEELONG_INTR_PCI_SYSERR		9
#define	YEELONG_INTR_DRAM_PARERR	10

/* isa interrupts on i8259 */
#define	YEELONG_INTR_INT0		11
#define	YEELONG_INTR_INT1		12
#define	YEELONG_INTR_INT2		13
#define	YEELONG_INTR_INT3		14

#define	YEELONG_INTRMASK_GPIO0		0x00000001	/* can't interrupt */
#define	YEELONG_INTRMASK_GPIO1		0x00000002
#define	YEELONG_INTRMASK_GPIO2		0x00000004
#define	YEELONG_INTRMASK_GPIO3		0x00000008

#define	YEELONG_INTRMASK_GPIO		0x0000000f

/* pci interrupts */
#define	YEELONG_INTRMASK_PCIA		0x00000010
#define	YEELONG_INTRMASK_PCIB		0x00000020
#define	YEELONG_INTRMASK_PCIC		0x00000040
#define	YEELONG_INTRMASK_PCID		0x00000080

#define	YEELONG_INTRMASK_PCI_PARERR	0x00000100
#define	YEELONG_INTRMASK_PCI_SYSERR	0x00000200
#define	YEELONG_INTRMASK_DRAM_PARERR	0x00000400

/* isa interrupts on i8259 */
#define	YEELONG_INTRMASK_INT0		0x00000800
#define	YEELONG_INTRMASK_INT1		0x00001000
#define	YEELONG_INTRMASK_INT2		0x00002000
#define	YEELONG_INTRMASK_INT3		0x00004000

#define	YEELONG_INTRMASK_LVL4		0x000007ff
#define	YEELONG_INTRMASK_LVL0		0x00007800 /* not maskable in bonito */
