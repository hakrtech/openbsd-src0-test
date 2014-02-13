/*	$OpenBSD: cpu.h,v 1.132 2014/02/13 23:11:06 kettenis Exp $	*/
/*	$NetBSD: cpu.h,v 1.35 1996/05/05 19:29:26 christos Exp $	*/

/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * William Jolitz.
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
 *
 *	@(#)cpu.h	5.4 (Berkeley) 5/9/91
 */

#ifndef _MACHINE_CPU_H_
#define _MACHINE_CPU_H_

/*
 * Definitions unique to i386 cpu support.
 */
#ifdef _KERNEL
#include <machine/frame.h>
#include <machine/psl.h>
#include <machine/segments.h>
#include <machine/intrdefs.h>

#ifdef MULTIPROCESSOR
#include <machine/i82489reg.h>
#include <machine/i82489var.h>
#endif

#endif /* _KERNEL */

/*
 * Arguments to hardclock, softclock and statclock
 * encapsulate the previous machine state in an opaque
 * clockframe; for now, use generic intrframe.
 *
 * XXX intrframe has a lot of gunk we don't need.
 */
#define clockframe intrframe

#include <sys/device.h>
#include <sys/lock.h>			/* will also get LOCKDEBUG */
#include <sys/sched.h>
#include <sys/sensors.h>

struct intrsource;

#ifdef _KERNEL
/* XXX stuff to move to cpuvar.h later */
struct cpu_info {
	struct device ci_dev;		/* our device */
	struct cpu_info *ci_self;	/* pointer to this structure */
	struct schedstate_percpu ci_schedstate; /* scheduler state */
	struct cpu_info *ci_next;	/* next cpu */

	/* 
	 * Public members. 
	 */
	struct proc *ci_curproc; 	/* current owner of the processor */
	struct simplelock ci_slock;	/* lock on this data structure */
	cpuid_t ci_cpuid; 		/* our CPU ID */
	u_int ci_apicid;		/* our APIC ID */
	u_int32_t ci_randseed;

	/*
	 * Private members.
	 */
	struct proc *ci_fpcurproc;	/* current owner of the FPU */
	struct proc *ci_fpsaveproc;
	int ci_fpsaving;		/* save in progress */

	struct pcb *ci_curpcb;		/* VA of current HW PCB */
	struct pcb *ci_idle_pcb;	/* VA of current PCB */
	int ci_idle_tss_sel;		/* TSS selector of idle PCB */
	struct pmap *ci_curpmap;

	struct intrsource *ci_isources[MAX_INTR_SOURCES];
	u_int32_t	ci_ipending;
	int		ci_ilevel;
	int		ci_idepth;
	u_int32_t	ci_imask[NIPL];
	u_int32_t	ci_iunmask[NIPL];
#ifdef DIAGNOSTIC
	int		ci_mutex_level;
#endif

	paddr_t		ci_idle_pcb_paddr; /* PA of idle PCB */
	volatile u_long	ci_flags;	/* flags; see below */
	u_int32_t	ci_ipis; 	/* interprocessor interrupts pending */

	u_int32_t	ci_level;
	u_int32_t	ci_vendor[4];
	u_int32_t	ci_signature;		/* X86 cpuid type */
	u_int32_t	ci_family;		/* extended cpuid family */
	u_int32_t	ci_model;		/* extended cpuid model */
	u_int32_t	ci_feature_flags;	/* X86 CPUID feature bits */
	u_int32_t	ci_feature_sefflags;	/* more CPUID feature bits */
	u_int32_t	cpu_class;		/* CPU class */
	u_int32_t	ci_cflushsz;		/* clflush cache-line size */

	int		ci_inatomic;

	struct cpu_functions *ci_func;	/* start/stop functions */
	void (*cpu_setup)(struct cpu_info *);	/* proc-dependant init */

	u_int		*ci_mwait;
/* bits in ci_mwait[0] */
#define	MWAIT_IN_IDLE		0x1	/* don't need IPI to wake */
#define	MWAIT_KEEP_IDLING	0x2	/* cleared by other cpus to wake me */
#define	MWAIT_IDLING		(MWAIT_IN_IDLE | MWAIT_KEEP_IDLING)

	int		ci_want_resched;

	union descriptor *ci_gdt;
	union descriptor *ci_ldt;	/* per-cpu default LDT */
	int		ci_ldt_len;	/* in bytes */

	volatile int ci_ddb_paused;	/* paused due to other proc in ddb */
#define CI_DDB_RUNNING		0
#define CI_DDB_SHOULDSTOP	1
#define CI_DDB_STOPPED		2
#define CI_DDB_ENTERDDB		3
#define CI_DDB_INDDB		4

	volatile int ci_setperf_state;
#define CI_SETPERF_READY	0
#define CI_SETPERF_SHOULDSTOP	1
#define CI_SETPERF_INTRANSIT	2
#define CI_SETPERF_DONE		3

	struct ksensordev	ci_sensordev;
	struct ksensor		ci_sensor;
#ifdef GPROF
	struct gmonparam	*ci_gmon;
#endif
};

/*
 * Processor flag notes: The "primary" CPU has certain MI-defined
 * roles (mostly relating to hardclock handling); we distinguish
 * betwen the processor which booted us, and the processor currently
 * holding the "primary" role just to give us the flexibility later to
 * change primaries should we be sufficiently twisted.  
 */

#define	CPUF_BSP	0x0001		/* CPU is the original BSP */
#define	CPUF_AP		0x0002		/* CPU is an AP */
#define	CPUF_SP		0x0004		/* CPU is only processor */
#define	CPUF_PRIMARY	0x0008		/* CPU is active primary processor */
#define	CPUF_APIC_CD	0x0010		/* CPU has apic configured */
#define	CPUF_CONST_TSC	0x0020		/* CPU has constant TSC */

#define	CPUF_PRESENT	0x1000		/* CPU is present */
#define	CPUF_RUNNING	0x2000		/* CPU is running */

/*
 * We statically allocate the CPU info for the primary CPU (or,
 * the only CPU on uniprocessors), and the primary CPU is the
 * first CPU on the CPU info list.
 */
extern struct cpu_info cpu_info_primary;
extern struct cpu_info *cpu_info_list;

#define	CPU_INFO_ITERATOR		int
#define	CPU_INFO_FOREACH(cii, ci)	for (cii = 0, ci = cpu_info_list; \
					    ci != NULL; ci = ci->ci_next)

#define CPU_INFO_UNIT(ci)	((ci)->ci_dev.dv_unit)

#ifdef MULTIPROCESSOR

#define MAXCPUS			32	/* because we use a bitmask */

#define CPU_STARTUP(_ci)	((_ci)->ci_func->start(_ci))
#define CPU_STOP(_ci)		((_ci)->ci_func->stop(_ci))
#define CPU_START_CLEANUP(_ci)	((_ci)->ci_func->cleanup(_ci))

static struct cpu_info *curcpu(void);

__inline static struct cpu_info *
curcpu(void)
{
	struct cpu_info *ci;

	/* Can't include sys/param.h for offsetof() since it includes us */
	__asm __volatile("movl %%fs:%1, %0" :
		"=r" (ci) : "m"
		(*(struct cpu_info * const *)&((struct cpu_info *)0)->ci_self));
	return ci;
}
#define cpu_number() 		(curcpu()->ci_cpuid)

#define CPU_IS_PRIMARY(ci)	((ci)->ci_flags & CPUF_PRIMARY)

extern struct cpu_info	*cpu_info[MAXCPUS];

extern void cpu_boot_secondary_processors(void);
extern void cpu_init_idle_pcbs(void);

void cpu_kick(struct cpu_info *);
void cpu_unidle(struct cpu_info *);

#else /* MULTIPROCESSOR */

#define MAXCPUS			1

#define cpu_number()		0
#define	curcpu()		(&cpu_info_primary)

#define CPU_IS_PRIMARY(ci)	1

#define cpu_kick(ci)
#define cpu_unidle(ci)

#endif

#define aston(p)	((p)->p_md.md_astpending = 1)

#define curpcb			curcpu()->ci_curpcb

#define want_resched (curcpu()->ci_want_resched)

/*
 * Preempt the current process if in interrupt from user mode,
 * or after the current trap/syscall if in system mode.
 */
extern void need_resched(struct cpu_info *);
#define clear_resched(ci) (ci)->ci_want_resched = 0

#define	CLKF_USERMODE(frame)	USERMODE((frame)->if_cs, (frame)->if_eflags)
#define	CLKF_PC(frame)		((frame)->if_eip)
#define	CLKF_INTR(frame)	(IDXSEL((frame)->if_cs) == GICODE_SEL)

/*
 * This is used during profiling to integrate system time.
 */
#define	PROC_PC(p)		((p)->p_md.md_regs->tf_eip)
#define	PROC_STACK(p)		((p)->p_md.md_regs->tf_esp)

/*
 * Give a profiling tick to the current process when the user profiling
 * buffer pages are invalid.  On the i386, request an ast to send us
 * through trap(), marking the proc as needing a profiling tick.
 */
#define	need_proftick(p)	aston(p)

/*
 * Notify the current process (p) that it has a signal pending,
 * process as soon as possible.
 */
void signotify(struct proc *);

/*
 * We need a machine-independent name for this.
 */
extern void (*delay_func)(int);
struct timeval;

#define	DELAY(x)		(*delay_func)(x)
#define delay(x)		(*delay_func)(x)

/*
 * High resolution clock support (Pentium only)
 */
void	calibrate_cyclecounter(void);

/*
 * pull in #defines for kinds of processors
 */
#include <machine/cputypes.h>

struct cpu_nocpuid_nameclass {
	int cpu_vendor;
	const char *cpu_vendorname;
	const char *cpu_name;
	int cpu_class;
	void (*cpu_setup)(struct cpu_info *);
};

struct cpu_cpuid_nameclass {
	const char *cpu_id;
	int cpu_vendor;
	const char *cpu_vendorname;
	struct cpu_cpuid_family {
		int cpu_class;
		const char *cpu_models[CPU_MAXMODEL+2];
		void (*cpu_setup)(struct cpu_info *);
	} cpu_family[CPU_MAXFAMILY - CPU_MINFAMILY + 1];
};

struct cpu_cpuid_feature {
	int feature_bit;
	const char *feature_name;
};

/* locore.s */
extern int cpu;
extern int cpu_id;
extern char cpu_vendor[]; /* note: NOT nul-terminated */
extern char cpu_brandstr[];
extern int cpuid_level;
extern int cpu_miscinfo;
extern int cpu_feature;
extern int ecpu_feature;
extern int cpu_ecxfeature;
extern int ecpu_ecxfeature;
extern int cpu_cache_eax;
extern int cpu_cache_ebx;
extern int cpu_cache_ecx;
extern int cpu_cache_edx;
extern int cpu_perf_eax;
extern int cpu_perf_ebx;
extern int cpu_perf_edx;
extern int cpu_apmi_edx;

/* machdep.c */
extern int cpu_apmhalt;
extern int cpu_class;
extern char cpu_model[];
extern const struct cpu_nocpuid_nameclass i386_nocpuid_cpus[];
extern const struct cpu_cpuid_nameclass i386_cpuid_cpus[];
extern void (*cpu_idle_enter_fcn)(void);
extern void (*cpu_idle_cycle_fcn)(void);
extern void (*cpu_idle_leave_fcn)(void);

/* apm.c */
extern int cpu_apmwarn;

extern int cpuspeed;

#if !defined(SMALL_KERNEL)
#define BUS66  6667
#define BUS100 10000
#define BUS133 13333
#define BUS166 16667
#define BUS200 20000
#define BUS266 26667
#define BUS333 33333
extern int bus_clock;
#endif

/* F00F bug fix stuff for pentium cpu */
extern int cpu_f00f_bug;
void fix_f00f(void);

/* dkcsum.c */
void	dkcsumattach(void);

extern int i386_use_fxsave;
extern int i386_has_sse;
extern int i386_has_sse2;

extern void (*update_cpuspeed)(void);

/* machdep.c */
void	dumpconf(void);
void	cpu_reset(void);
void	i386_proc0_tss_ldt_init(void);
void	i386_init_pcb_tss_ldt(struct cpu_info *);
void	cpuid(u_int32_t, u_int32_t *);

/* locore.s */
struct region_descriptor;
void	lgdt(struct region_descriptor *);

struct pcb;
void	savectx(struct pcb *);
void	switch_exit(struct proc *);
void	proc_trampoline(void);

/* clock.c */
extern void (*initclock_func)(void);
void	startclocks(void);
void	rtcdrain(void *);
void	rtcstart(void);
void	rtcstop(void);
void	i8254_delay(int);
void	i8254_initclocks(void);
void	i8254_startclock(void);
void	i8254_inittimecounter(void);
void	i8254_inittimecounter_simple(void);

#if !defined(SMALL_KERNEL)
/* est.c */
void	est_init(struct cpu_info *, int);
void	est_setperf(int);
/* longrun.c */
void	longrun_init(void);
void	longrun_setperf(int);
/* p4tcc.c */
void	p4tcc_init(int, int);
void	p4tcc_setperf(int);
/* powernow.c */
void	k6_powernow_init(void);
void	k6_powernow_setperf(int);
/* powernow-k7.c */
void	k7_powernow_init(void);
void	k7_powernow_setperf(int);
/* powernow-k8.c */
void 	k8_powernow_init(void);
void 	k8_powernow_setperf(int);
/* k1x-pstate.c */
void k1x_init(struct cpu_info *);
void k1x_setperf(int);
#endif

/* npx.c */
void	npxdrop(struct proc *);
void	npxsave_proc(struct proc *, int);
void	npxsave_cpu(struct cpu_info *, int);

#ifdef USER_LDT
/* sys_machdep.h */
extern int user_ldt_enable;
int	i386_get_ldt(struct proc *, void *, register_t *);
int	i386_set_ldt(struct proc *, void *, register_t *);
#endif

/* isa_machdep.c */
void	isa_defaultirq(void);
void	isa_nodefaultirq(void);
int	isa_nmi(void);

/* pmap.c */
void	pmap_bootstrap(vaddr_t);

/* vm_machdep.c */
int	kvtop(caddr_t);

#ifdef MULTIPROCESSOR
/* mp_setperf.c */
void	mp_setperf_init(void);
#endif

#ifdef VM86
/* vm86.c */
void	vm86_gpfault(struct proc *, int);
#endif /* VM86 */

#endif /* _KERNEL */

/* 
 * CTL_MACHDEP definitions.
 */
#define	CPU_CONSDEV		1	/* dev_t: console terminal device */
#define	CPU_BIOS		2	/* BIOS variables */
#define	CPU_BLK2CHR		3	/* convert blk maj into chr one */
#define	CPU_CHR2BLK		4	/* convert chr maj into blk one */
#define CPU_ALLOWAPERTURE	5	/* allow mmap of /dev/xf86 */
#define CPU_CPUVENDOR		6	/* cpuid vendor string */
#define CPU_CPUID		7	/* cpuid */
#define CPU_CPUFEATURE		8	/* cpuid features */
#define CPU_APMWARN		9	/* APM battery warning percentage */
#define CPU_KBDRESET		10	/* keyboard reset under pcvt */
#define CPU_APMHALT		11	/* halt -p hack */
#define CPU_USERLDT		12
#define CPU_OSFXSR		13	/* uses FXSAVE/FXRSTOR */
#define CPU_SSE			14	/* supports SSE */
#define CPU_SSE2		15	/* supports SSE2 */
#define CPU_XCRYPT		16	/* supports VIA xcrypt in userland */
#define CPU_LIDSUSPEND		17	/* lid close causes a suspend */
#define CPU_MAXID		18	/* number of valid machdep ids */

#define	CTL_MACHDEP_NAMES { \
	{ 0, 0 }, \
	{ "console_device", CTLTYPE_STRUCT }, \
	{ "bios", CTLTYPE_INT }, \
	{ "blk2chr", CTLTYPE_STRUCT }, \
	{ "chr2blk", CTLTYPE_STRUCT }, \
	{ "allowaperture", CTLTYPE_INT }, \
	{ "cpuvendor", CTLTYPE_STRING }, \
	{ "cpuid", CTLTYPE_INT }, \
	{ "cpufeature", CTLTYPE_INT }, \
	{ "apmwarn", CTLTYPE_INT }, \
	{ "kbdreset", CTLTYPE_INT }, \
	{ "apmhalt", CTLTYPE_INT }, \
	{ "userldt", CTLTYPE_INT }, \
	{ "osfxsr", CTLTYPE_INT }, \
	{ "sse", CTLTYPE_INT }, \
	{ "sse2", CTLTYPE_INT }, \
	{ "xcrypt", CTLTYPE_INT }, \
	{ "lidsuspend", CTLTYPE_INT }, \
}

/*
 * This needs to be included late since it relies on definitions higher
 * up in this file.
 */
#if defined(MULTIPROCESSOR) && defined(_KERNEL)
#include <sys/mplock.h>
#endif

#endif /* !_MACHINE_CPU_H_ */
