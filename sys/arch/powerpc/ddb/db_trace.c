/*	$OpenBSD: db_trace.c,v 1.11 2017/01/24 00:58:55 mpi Exp $	*/
/*	$NetBSD: db_trace.c,v 1.15 1996/02/22 23:23:41 gwr Exp $	*/

/*
 * Mach Operating System
 * Copyright (c) 1992 Carnegie Mellon University
 * All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
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

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <sys/user.h>

#include <uvm/uvm_extern.h>

#include <machine/db_machdep.h>
#include <machine/signal.h>
#include <machine/pcb.h>
#include <machine/pmap.h>

#include <ddb/db_access.h>
#include <ddb/db_sym.h>
#include <ddb/db_variables.h>
#include <ddb/db_interface.h>
#include <ddb/db_output.h>

db_regs_t ddb_regs;

struct db_variable db_regs[] = {
	{ "r0",  (long *)&ddb_regs.fixreg[0],	FCN_NULL },
	{ "r1",  (long *)&ddb_regs.fixreg[1],	FCN_NULL },
	{ "r2",  (long *)&ddb_regs.fixreg[2],	FCN_NULL },
	{ "r3",  (long *)&ddb_regs.fixreg[3],	FCN_NULL },
	{ "r4",  (long *)&ddb_regs.fixreg[4],	FCN_NULL },
	{ "r5",  (long *)&ddb_regs.fixreg[5],	FCN_NULL },
	{ "r6",  (long *)&ddb_regs.fixreg[6],	FCN_NULL },
	{ "r7",  (long *)&ddb_regs.fixreg[7],	FCN_NULL },
	{ "r8",  (long *)&ddb_regs.fixreg[8],	FCN_NULL },
	{ "r9",  (long *)&ddb_regs.fixreg[9],	FCN_NULL },
	{ "r10", (long *)&ddb_regs.fixreg[10],	FCN_NULL },
	{ "r11", (long *)&ddb_regs.fixreg[11],	FCN_NULL },
	{ "r12", (long *)&ddb_regs.fixreg[12],	FCN_NULL },
	{ "r13", (long *)&ddb_regs.fixreg[13],	FCN_NULL },
	{ "r14", (long *)&ddb_regs.fixreg[13],	FCN_NULL },
	{ "r15", (long *)&ddb_regs.fixreg[13],	FCN_NULL },
	{ "r16", (long *)&ddb_regs.fixreg[13],	FCN_NULL },
	{ "r17", (long *)&ddb_regs.fixreg[17],	FCN_NULL },
	{ "r18", (long *)&ddb_regs.fixreg[18],	FCN_NULL },
	{ "r19", (long *)&ddb_regs.fixreg[19],	FCN_NULL },
	{ "r20", (long *)&ddb_regs.fixreg[20],	FCN_NULL },
	{ "r21", (long *)&ddb_regs.fixreg[21],	FCN_NULL },
	{ "r22", (long *)&ddb_regs.fixreg[22],	FCN_NULL },
	{ "r23", (long *)&ddb_regs.fixreg[23],	FCN_NULL },
	{ "r24", (long *)&ddb_regs.fixreg[24],	FCN_NULL },
	{ "r25", (long *)&ddb_regs.fixreg[25],	FCN_NULL },
	{ "r26", (long *)&ddb_regs.fixreg[26],	FCN_NULL },
	{ "r27", (long *)&ddb_regs.fixreg[27],	FCN_NULL },
	{ "r28", (long *)&ddb_regs.fixreg[28],	FCN_NULL },
	{ "r29", (long *)&ddb_regs.fixreg[29],	FCN_NULL },
	{ "r30", (long *)&ddb_regs.fixreg[30],	FCN_NULL },
	{ "r31", (long *)&ddb_regs.fixreg[31],	FCN_NULL },
	{ "lr",  (long *)&ddb_regs.lr,		FCN_NULL },
	{ "cr",  (long *)&ddb_regs.cr,		FCN_NULL },
	{ "xer", (long *)&ddb_regs.xer,		FCN_NULL },
	{ "ctr", (long *)&ddb_regs.ctr,		FCN_NULL },
	{ "iar", (long *)&ddb_regs.srr0,		FCN_NULL },
	{ "msr", (long *)&ddb_regs.srr1,		FCN_NULL },
};

struct db_variable *db_eregs = db_regs + nitems(db_regs);

/*
 * this is probably hackery.
 */
void
db_save_regs(struct trapframe *frame)
{
	bcopy(frame, &ddb_regs, sizeof (struct trapframe));
}

/* from locore.S */
extern db_addr_t trapexit;
extern db_addr_t esym;
#define	INTSTK		(8*1024)	/* 8K interrupt stack */

#define	INKERNEL(va)	(((vaddr_t)(va)) >= VM_MIN_KERNEL_ADDRESS &&	\
			((vaddr_t)(va)) < VM_MAX_KERNEL_ADDRESS)

#define	ININTSTK(va)	(((vaddr_t)(va)) >= round_page(esym) &&		\
			((vaddr_t)(va)) < (round_page(esym) + INTSTK))

/*
 *	Frame tracing.
 */
void
db_stack_trace_print(db_expr_t addr, int have_addr, db_expr_t count,
    char *modif, int (*pr)(const char *, ...))
{
	db_addr_t	 lr, sp, lastsp;
	db_expr_t	 offset;
	db_sym_t	 sym;
	char		*name;
	char		 c, *cp = modif;
	int		 trace_proc = 0;

	while ((c = *cp++) != 0) {
		if (c == 'p')
			trace_proc = 1;
	}

	if (!have_addr) {
		sp = ddb_regs.fixreg[1];
		lr = ddb_regs.srr0;
	} else {
		if (trace_proc) {
			struct proc *p = tfind((pid_t)addr);
			if (p == NULL) {
				(*pr) ("not found\n");
				return;
			}
			addr = p->p_addr->u_pcb.pcb_sp;
		}
		sp = addr;
		db_read_bytes(sp + 4, sizeof(db_addr_t), (char *)&lr);
	}

	while (count && sp != 0) {
		/*
		 * lr contains the return address, so adjust its value
		 * to display the offset of the calling address.
		 */
		sym = db_search_symbol(lr - 4, DB_STGY_ANY, &offset);
		db_symbol_values(sym, &name, NULL);

		if (name == NULL || strcmp(name, "end") == 0) {
			(*pr)("at 0x%lx", lr - 4);
		} else {
			(*pr)("%s() at ", name);
			db_printsym(lr - 4, DB_STGY_PROC, pr);
		}
		(*pr)("\n");

		lastsp = sp;

		/*
		 * Abuse the fact that the return address of the trap()
		 * function is always 'trapexit'.
		 */
		if (lr == (db_addr_t)&trapexit) {
			struct trapframe *tf = (struct trapframe *)(sp + 8);
			uint32_t code = tf->fixreg[0];
			uint32_t type = tf->exc;

			if (tf->srr1 & PSL_PR)
				type |= EXC_USER;

			if (type == (EXC_SC|EXC_USER))
				(*pr)("--- syscall (number %d) ---\n", code);
			else
				(*pr)("--- trap (type 0x%x) ---\n", type);
		}

		db_read_bytes(sp, sizeof(db_addr_t), (char *)&sp);
		if (sp == 0)
			break;

		db_read_bytes(sp + 4, sizeof(db_addr_t), (char *)&lr);

		if (INKERNEL(sp)) {
			if (sp <= lastsp) {
				(*pr)("Bad frame pointer: 0x%lx\n", sp);
				break;
			}

			if (ININTSTK(lastsp))
				(*pr)("--- interrupt ---\n");

		} else  {
			if (!ININTSTK(sp)) {
				(*pr)("End of kernel: 0x%lx\n", sp);
				break;
			}
		}
		--count;
	}
	(*pr)("end trace frame: 0x%lx, count: %d\n", sp, count);
}
