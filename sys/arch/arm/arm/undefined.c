/*	$OpenBSD: undefined.c,v 1.9 2017/04/30 13:04:49 mpi Exp $	*/
/*	$NetBSD: undefined.c,v 1.22 2003/11/29 22:21:29 bjh21 Exp $	*/

/*
 * Copyright (c) 2001 Ben Harris.
 * Copyright (c) 1995 Mark Brinicombe.
 * Copyright (c) 1995 Brini.
 * All rights reserved.
 *
 * This code is derived from software written for Brini by Mark Brinicombe
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
 *    must display the following acknowledgement:
 *	This product includes software developed by Brini.
 * 4. The name of the company nor the name of the author may be used to
 *    endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY BRINI ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL BRINI OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * RiscBSD kernel project
 *
 * undefined.c
 *
 * Fault handler
 *
 * Created      : 06/01/95
 */

#include <sys/param.h>

#include <sys/malloc.h>
#include <sys/queue.h>
#include <sys/signal.h>
#include <sys/signalvar.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <sys/user.h>
#include <sys/syslog.h>
#include <sys/vmmeter.h>

#include <uvm/uvm_extern.h>

#include <machine/cpu.h>
#include <machine/frame.h>
#include <arm/undefined.h>
#include <machine/trap.h>


static int gdb_trapper(u_int, u_int, struct trapframe *, int);

LIST_HEAD(, undefined_handler) undefined_handlers[MAX_COPROCS];


void *
install_coproc_handler(int coproc, undef_handler_t handler)
{
	struct undefined_handler *uh;

	KASSERT(coproc >= 0 && coproc < MAX_COPROCS);
	KASSERT(handler != NULL); /* Used to be legal. */

	/* XXX: M_TEMP??? */
	uh = (struct undefined_handler *)malloc(sizeof(*uh), M_TEMP, M_WAITOK);
	uh->uh_handler = handler;
	install_coproc_handler_static(coproc, uh);
	return uh;
}

void
install_coproc_handler_static(int coproc, struct undefined_handler *uh)
{

	LIST_INSERT_HEAD(&undefined_handlers[coproc], uh, uh_link);
}

void
remove_coproc_handler(void *cookie)
{
	struct undefined_handler *uh = cookie;

	LIST_REMOVE(uh, uh_link);
	free(uh, M_TEMP, 0);
}


static int
gdb_trapper(u_int addr, u_int insn, struct trapframe *frame, int code)
{
	union sigval sv;
	struct proc *p;
	p = (curproc == NULL) ? &proc0 : curproc;

	if (insn == GDB_BREAKPOINT || insn == GDB5_BREAKPOINT) {
		if (code == FAULT_USER) {
			sv.sival_int = addr;
			trapsignal(p, SIGTRAP, 0, TRAP_BRKPT, sv);
			return 0;
		}
	}
	return 1;
}

static struct undefined_handler gdb_uh;

void
undefined_init()
{
	int loop;

	/* Not actually necessary -- the initialiser is just NULL */
	for (loop = 0; loop < MAX_COPROCS; ++loop)
		LIST_INIT(&undefined_handlers[loop]);

	/* Install handler for GDB breakpoints */
	gdb_uh.uh_handler = gdb_trapper;
	install_coproc_handler_static(0, &gdb_uh);
}


void
undefinedinstruction(trapframe_t *frame)
{
	struct proc *p;
	u_int fault_pc;
	int fault_instruction;
	int fault_code;
	int coprocessor;
	struct undefined_handler *uh;
#ifdef VERBOSE_ARM32
	int s;
#endif
	union sigval sv;

	/* Enable interrupts if they were enabled before the exception. */
	if (!(frame->tf_spsr & PSR_I))
		enable_interrupts(PSR_I);

	frame->tf_pc -= INSN_SIZE;
	fault_pc = frame->tf_pc;

	/* Get the current proc structure or proc0 if there is none. */
	p = (curproc == NULL) ? &proc0 : curproc;

	/*
	 * Make sure the program counter is correctly aligned so we
	 * don't take an alignment fault trying to read the opcode.
	 */
	if (__predict_false((fault_pc & 3) != 0)) {
		/* Give the user an illegal instruction signal. */
		sv.sival_int = (u_int32_t) fault_pc;
		trapsignal(p, SIGILL, 0, ILL_ILLOPC, sv);
		userret(p);
		return;
	}

	/*
	 * Should use copyin() here .. but in the interests of squeezing every
	 * bit of speed we will just read it directly. We know the instruction
	 * can be read as was just executed so this will never fail unless the
	 * kernel is screwed up in which case it does not really matter does
	 * it?
	 */

	fault_instruction = *(u_int32_t *)fault_pc;

	/* Update vmmeter statistics */
	uvmexp.traps++;

	/* Check for coprocessor instruction */

	/*
	 * According to the datasheets you only need to look at bit 27 of the
	 * instruction to tell the difference between an undefined
	 * instruction and a coprocessor instruction following an undefined
	 * instruction trap.
	 */

	if ((fault_instruction & (1 << 27)) != 0)
		coprocessor = (fault_instruction >> 8) & 0x0f;
	else
		coprocessor = 0;

	if ((frame->tf_spsr & PSR_MODE) == PSR_USR32_MODE) {
		/*
		 * Modify the fault_code to reflect the USR/SVC state at
		 * time of fault.
		 */
		fault_code = FAULT_USER;
		p->p_addr->u_pcb.pcb_tf = frame;
	} else
		fault_code = 0;

	/* OK this is were we do something about the instruction. */
	LIST_FOREACH(uh, &undefined_handlers[coprocessor], uh_link)
	    if (uh->uh_handler(fault_pc, fault_instruction, frame,
			       fault_code) == 0)
		    break;

	if (uh == NULL) {
		/* Fault has not been handled */
		
#ifdef VERBOSE_ARM32
		s = spltty();

		if ((fault_instruction & 0x0f000010) == 0x0e000000) {
			printf("CDP\n");
			disassemble(fault_pc);
		} else if ((fault_instruction & 0x0e000000) == 0x0c000000) {
			printf("LDC/STC\n");
			disassemble(fault_pc);
		} else if ((fault_instruction & 0x0f000010) == 0x0e000010) {
			printf("MRC/MCR\n");
			disassemble(fault_pc);
		} else if ((fault_instruction & ~INSN_COND_MASK)
			 != (KERNEL_BREAKPOINT & ~INSN_COND_MASK)) {
			printf("Undefined instruction\n");
			disassemble(fault_pc);
		}

		splx(s);
#endif
        
		if ((fault_code & FAULT_USER) == 0) {
			printf("Undefined instruction in kernel\n");
#ifdef DDB
			Debugger();
#endif
		}

		sv.sival_int = frame->tf_pc;
		trapsignal(p, SIGILL, 0, ILL_ILLOPC, sv);
	}

	if ((fault_code & FAULT_USER) == 0)
		return;

	userret(p);
}
