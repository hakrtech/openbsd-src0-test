/*	$OpenBSD: pcb.h,v 1.8 2001/08/12 12:03:02 heko Exp $ */
/*
 * Copyright (c) 1996 Nivas Madhur
 * Mach Operating System
 * Copyright (c) 1993-1992 Carnegie Mellon University
 * All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * CARNEGIE MELLON AND OMRON ALLOW FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON AND OMRON DISCLAIM ANY LIABILITY OF ANY KIND
 * FOR ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 * Carnegie Mellon requests users of this software to return to
 *
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 *
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/* 
 * Motorola 88100 pcb definitions 
 *
 */
/*
 */
#ifndef _M88K_PCB_H_
#define _M88K_PCB_H_

#include <machine/reg.h>

/* 
 * Our PCB is the regular PCB+Save area for kernel frame.
 * Upon entering kernel mode from user land, save the user context
 * in the saved_state area - this is passed as the exception frame.
 * On a context switch, only registers that need to be saved by the
 * C calling convention and few other regs (pc, psr etc) are saved
 * in the kernel_state part of the PCB. Typically, trap fames are
 * save on the stack (by low level handlers or by hardware) but,
 * we just decided to do it in the PCB.
 */

/*
 * This must always be an even number of words long so that our stack
 * will always be properly aligned (88k need 8 byte alignmet). Also,
 * place r14 on double word boundary so that we can use st.d while
 * saving the regs.
 */

struct m88100_pcb {
    unsigned pcb_pc;	/* address to return */
    unsigned pcb_ipl;
    unsigned pcb_r14;
    unsigned pcb_r15;
    unsigned pcb_r16;
    unsigned pcb_r17;
    unsigned pcb_r18;
    unsigned pcb_r19;
    unsigned pcb_r20;
    unsigned pcb_r21;
    unsigned pcb_r22;
    unsigned pcb_r23;
    unsigned pcb_r24;
    unsigned pcb_r25;
    unsigned pcb_r26;
    unsigned pcb_r27;
    unsigned pcb_r28;
    unsigned pcb_r29;
    unsigned pcb_r30;
    unsigned pcb_sp; 	/* kernel stack pointer */
};

#define m88100_saved_state reg
#define trapframe m88100_saved_state

struct pcb
{
  struct m88100_pcb            kernel_state;
  struct m88100_saved_state    user_state;
  int	 		       pcb_onfault;	/* for copyin/copyout faults */
  int	 		       pcb_pad;		/* pad it XXX */
};

typedef	struct pcb	*pcb_t;		/* exported */

/*
 *	Location of saved user registers for the proc.
 */
#define	USER_REGS(p) \
	(((struct m88100_saved_state *)  (&((p)->p_addr->u_pcb.user_state))))
/*
 * The pcb is augmented with machine-dependent additional data for
 * core dumps.  Note that the trapframe here is a copy of the one
 * from the top of the kernel stack (included here so that the kernel
 * stack itself need not be dumped).
 */
struct md_coredump {
	struct	trapframe md_tf;
};

#endif /* _M88K_PCB_H_ */
