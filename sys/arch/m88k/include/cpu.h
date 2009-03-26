/*	$OpenBSD: cpu.h,v 1.47 2009/03/26 17:24:33 oga Exp $ */
/*
 * Copyright (c) 1996 Nivas Madhur
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This software was developed by the Computer Systems Engineering group
 * at Lawrence Berkeley Laboratory under DARPA contract BG 91-66 and
 * contributed to Berkeley.
 *
 * All advertising materials mentioning features or use of this software
 * must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Lawrence Berkeley Laboratory.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
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

#ifndef __M88K_CPU_H__
#define __M88K_CPU_H__

/*
 * CTL_MACHDEP definitions.
 */
#define	CPU_CONSDEV	1	/* dev_t: console terminal device */
#define	CPU_CPUTYPE	2	/* int: cpu type */
#define	CPU_MAXID	3	/* number of valid machdep ids */

#define	CTL_MACHDEP_NAMES { \
	{ 0, 0 }, \
	{ "console_device", CTLTYPE_STRUCT }, \
	{ "cputype", CTLTYPE_INT }, \
}

#ifdef _KERNEL

#include <machine/atomic.h>
#include <machine/pcb.h>
#include <machine/psl.h>
#include <machine/intr.h>
#include <sys/queue.h>
#include <sys/sched.h>

#if defined(MULTIPROCESSOR)
#if !defined(MAX_CPUS) || MAX_CPUS > 4
#undef	MAX_CPUS
#define	MAX_CPUS	4
#endif
#else
#if !defined(MAX_CPUS)
#undef	MAX_CPUS
#define	MAX_CPUS	1
#endif
#endif

#ifndef _LOCORE

extern u_int max_cpus;

#include <machine/lock.h>

/*
 * Per-CPU data structure
 */

struct cpu_info {
	u_int		 ci_flags;
#define	CIF_ALIVE		0x01		/* cpu initialized */
#define	CIF_PRIMARY		0x02		/* primary cpu */

	struct proc	*ci_curproc;		/* current process... */
	struct pcb	*ci_curpcb;		/* ...and its pcb */

	u_int		 ci_cpuid;		/* cpu number */

	/*
	 * Function pointers used within mplock to ensure
	 * non-interruptability.
	 */
	uint32_t	(*ci_mp_atomic_begin)
			    (__cpu_simple_lock_t *lock, uint *csr);
	void		(*ci_mp_atomic_end)
			    (uint32_t psr, __cpu_simple_lock_t *lock, uint csr);

	/*
	 * The following fields are used differently depending on
	 * the processor type.  Think of them as an anonymous union
	 * of two anonymous structs.
	 */
	u_int		 ci_cpudep0;
	u_int		 ci_cpudep1;
	u_int		 ci_cpudep2;
	u_int		 ci_cpudep3;
	u_int		 ci_cpudep4;
	u_int		 ci_cpudep5;
	u_int		 ci_cpudep6;
	u_int		 ci_cpudep7;

	/* 88100 fields */
#define	ci_pfsr_i0	 ci_cpudep0		/* instruction... */
#define	ci_pfsr_i1	 ci_cpudep1
#define	ci_pfsr_d0	 ci_cpudep2		/* ...and data CMMU PFSRs */
#define	ci_pfsr_d1	 ci_cpudep3

	/* 88110 fields */
#define	ci_ipi_arg1	 ci_cpudep0		/* Complex IPI arguments */
#define	ci_ipi_arg2	 ci_cpudep1
#define	ci_h_sxip	 ci_cpudep2		/* trapframe values */
#define	ci_h_epsr	 ci_cpudep3		/* for hardclock */
#define	ci_s_sxip	 ci_cpudep4		/* and softclock */
#define	ci_s_epsr	 ci_cpudep5
#define	ci_pmap_ipi	 ci_cpudep6		/* delayed pmap tlb ipi */

	struct schedstate_percpu
			 ci_schedstate;		/* scheduling state */
	int		 ci_want_resched;	/* need_resched() invoked */

	u_int		 ci_intrdepth;		/* interrupt depth */

	u_long		 ci_spin_locks;		/* spin locks counter */

	int		 ci_ddb_state;		/* ddb status */
#define	CI_DDB_RUNNING	0
#define	CI_DDB_ENTERDDB	1
#define	CI_DDB_INDDB	2
#define	CI_DDB_PAUSE	3

	u_int32_t	 ci_randseed;		/* per-cpu random seed */

	int		 ci_ipi;		/* pending ipis */
#define	CI_IPI_NOTIFY		0x00000001
#define	CI_IPI_HARDCLOCK	0x00000002
#define	CI_IPI_STATCLOCK	0x00000004
#define	CI_IPI_DDB		0x00000008
/* 88110 simple ipi */
#define	CI_IPI_TLB_FLUSH_KERNEL	0x00000010
#define	CI_IPI_TLB_FLUSH_USER	0x00000020
/* 88110 complex ipi */
#define	CI_IPI_CACHE_FLUSH	0x00000040
#define	CI_IPI_ICACHE_FLUSH	0x00000080
#define	CI_IPI_DMA_CACHECTL	0x00000100
	void		(*ci_softipi_cb)(void);	/* 88110 softipi callback */
};

extern cpuid_t master_cpu;
extern struct cpu_info m88k_cpus[MAX_CPUS];

#define	CPU_INFO_ITERATOR	cpuid_t
#define	CPU_INFO_FOREACH(cii, ci) \
	for ((cii) = 0; (cii) < MAX_CPUS; (cii)++) \
		if (((ci) = &m88k_cpus[cii])->ci_flags & CIF_ALIVE)
#define	CPU_INFO_UNIT(ci)	((ci)->ci_cpuid)
#define MAXCPUS	MAX_CPUS

#if defined(MULTIPROCESSOR)

static __inline__ struct cpu_info *
curcpu(void)
{
	struct cpu_info *cpuptr;

	__asm__ __volatile__ ("ldcr %0, cr17" : "=r" (cpuptr));
	return cpuptr;
}

#define	CPU_IS_PRIMARY(ci)	((ci)->ci_flags & CIF_PRIMARY)

void	cpu_boot_secondary_processors(void);
__dead void cpu_emergency_disable(void);
void	cpu_unidle(struct cpu_info *);
void	m88k_send_ipi(int, cpuid_t);
void	m88k_broadcast_ipi(int);

#else	/* MULTIPROCESSOR */

#define	curcpu()	(&m88k_cpus[0])
#define	cpu_unidle(ci)	do { /* nothing */ } while (0)
#define	CPU_IS_PRIMARY(ci)	1

#endif	/* MULTIPROCESSOR */

void	set_cpu_number(cpuid_t);

/*
 * The md code may hardcode this in some very specific situations.
 */
#if !defined(cpu_number)
#define	cpu_number()		curcpu()->ci_cpuid
#endif

#define	curpcb			curcpu()->ci_curpcb

#endif /* _LOCORE */

/*
 * definitions of cpu-dependent requirements
 * referenced in generic code
 */
#define	cpu_exec(p)		do { /* nothing */ } while (0)

#define	cpu_idle_enter()	do { /* nothing */ } while (0)
#define	cpu_idle_cycle()	do { /* nothing */ } while (0)
#define	cpu_idle_leave()	do { /* nothing */ } while (0)

#if defined(MULTIPROCESSOR)
#include <sys/lock.h>
#include <sys/mplock.h>
#endif

/*
 * Arguments to hardclock and gatherstats encapsulate the previous
 * machine state in an opaque clockframe. CLKF_INTR is only valid
 * if the process is in kernel mode. Clockframe is really trapframe,
 * so pointer to clockframe can be safely cast into a pointer to
 * trapframe.
 */
struct clockframe {
	struct trapframe tf;
};

#define	CLKF_USERMODE(framep)	(((framep)->tf.tf_epsr & PSR_MODE) == 0)
#define	CLKF_PC(framep)		((framep)->tf.tf_sxip & XIP_ADDR)
#define	CLKF_INTR(framep) \
	(((struct cpu_info *)(framep)->tf.tf_cpu)->ci_intrdepth > 1)

#define	aston(p)		((p)->p_md.md_astpending = 1)

/*
 * This is used during profiling to integrate system time.
 */
#define	PC_REGS(regs)							\
	(CPU_IS88110 ? ((regs)->exip & XIP_ADDR) :			\
	 ((regs)->sxip & XIP_V ? (regs)->sxip & XIP_ADDR :		\
	  ((regs)->snip & NIP_V ? (regs)->snip & NIP_ADDR :		\
				   (regs)->sfip & FIP_ADDR)))
#define	PROC_PC(p)	PC_REGS((struct reg *)((p)->p_md.md_tf))

#define clear_resched(ci) 	(ci)->ci_want_resched = 0

/*
 * Give a profiling tick to the current process when the user profiling
 * buffer pages are invalid.  On the m88k, request an ast to send us
 * through trap(), marking the proc as needing a profiling tick.
 */
#define	need_proftick(p)	aston(p)

void	need_resched(struct cpu_info *);
void	signotify(struct proc *);
void	softipi(void);

int	badaddr(vaddr_t addr, int size);

#endif /* _KERNEL */
#endif /* __M88K_CPU_H__ */
