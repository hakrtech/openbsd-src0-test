/*	$OpenBSD: cpufunc.h,v 1.25 2017/05/27 12:21:50 tedu Exp $	*/
/*	$NetBSD: cpufunc.h,v 1.8 1994/10/27 04:15:59 cgd Exp $	*/

/*
 * Copyright (c) 1993 Charles Hannum.
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
 *    must display the following acknowledgement:
 *      This product includes software developed by Charles Hannum.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
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

#ifndef _MACHINE_CPUFUNC_H_
#define	_MACHINE_CPUFUNC_H_

#ifdef _KERNEL

/*
 * Functions to provide access to i386-specific instructions.
 */

#include <sys/types.h>

#include <machine/specialreg.h>

static __inline void invlpg(u_int);
static __inline void lidt(void *);
static __inline void lldt(u_short);
static __inline void ltr(u_short);
static __inline void lcr0(u_int);
static __inline u_int rcr0(void);
static __inline u_int rcr2(void);
static __inline void lcr3(u_int);
static __inline u_int rcr3(void);
static __inline void lcr4(u_int);
static __inline u_int rcr4(void);
static __inline void tlbflush(void);
static __inline void disable_intr(void);
static __inline void enable_intr(void);
static __inline u_int read_eflags(void);
static __inline void write_eflags(u_int);
static __inline void wbinvd(void);
static __inline void clflush(u_int32_t addr);
static __inline void mfence(void);
static __inline void wrmsr(u_int, u_int64_t);
static __inline u_int64_t rdmsr(u_int);
static __inline void breakpoint(void);

static __inline void 
invlpg(u_int addr)
{ 
        __asm volatile("invlpg (%0)" : : "r" (addr) : "memory");
}  

static __inline void
lidt(void *p)
{
	__asm volatile("lidt (%0)" : : "r" (p) : "memory");
}

static __inline void
lldt(u_short sel)
{
	__asm volatile("lldt %0" : : "r" (sel));
}

static __inline void
ltr(u_short sel)
{
	__asm volatile("ltr %0" : : "r" (sel));
}

static __inline void
lcr0(u_int val)
{
	__asm volatile("movl %0,%%cr0" : : "r" (val));
}

static __inline u_int
rcr0(void)
{
	u_int val;
	__asm volatile("movl %%cr0,%0" : "=r" (val));
	return val;
}

static __inline u_int
rcr2(void)
{
	u_int val;
	__asm volatile("movl %%cr2,%0" : "=r" (val));
	return val;
}

static __inline void
lcr3(u_int val)
{
	__asm volatile("movl %0,%%cr3" : : "r" (val));
}

static __inline u_int
rcr3(void)
{
	u_int val;
	__asm volatile("movl %%cr3,%0" : "=r" (val));
	return val;
}

static __inline void
lcr4(u_int val)
{
	__asm volatile("movl %0,%%cr4" : : "r" (val));
}

static __inline u_int
rcr4(void)
{
	u_int val;
	__asm volatile("movl %%cr4,%0" : "=r" (val));
	return val;
}

static __inline void
tlbflush(void)
{
	u_int val;
	__asm volatile("movl %%cr3,%0" : "=r" (val));
	__asm volatile("movl %0,%%cr3" : : "r" (val));
}

#ifdef notyet
void	setidt(int idx, /*XXX*/caddr_t func, int typ, int dpl);
#endif


/* XXXX ought to be in psl.h with spl() functions */

static __inline void
disable_intr(void)
{
	__asm volatile("cli");
}

static __inline void
enable_intr(void)
{
	__asm volatile("sti");
}

static __inline u_int
read_eflags(void)
{
	u_int ef;

	__asm volatile("pushfl; popl %0" : "=r" (ef));
	return (ef);
}

static __inline void
write_eflags(u_int ef)
{
	__asm volatile("pushl %0; popfl" : : "r" (ef));
}

static inline u_long
intr_disable(void)
{
	u_long ef;

	ef = read_eflags();
	disable_intr();
	return (ef);
}

static inline void
intr_restore(u_long ef)
{
	write_eflags(ef);
}

static __inline void
wbinvd(void)
{
        __asm volatile("wbinvd");
}

static __inline void
clflush(u_int32_t addr)
{
	__asm volatile("clflush %0" : "+m" (*(volatile char *)addr));
}

static __inline void
mfence(void)
{
	__asm volatile("mfence" : : : "memory");
}

static __inline void
wrmsr(u_int msr, u_int64_t newval)
{
        __asm volatile("wrmsr" : : "A" (newval), "c" (msr));
}

static __inline u_int64_t
rdmsr(u_int msr)
{
        u_int64_t rv;

        __asm volatile("rdmsr" : "=A" (rv) : "c" (msr));
        return (rv);
}

static __inline void
monitor(const volatile void *addr, u_long extensions, u_int hints)
{
	__asm volatile("monitor"
	    : : "a" (addr), "c" (extensions), "d" (hints));
}

static __inline void
mwait(u_long extensions, u_int hints)
{
	__asm volatile("mwait" : : "a" (hints), "c" (extensions));
}

/* 
 * Some of the undocumented AMD64 MSRs need a 'passcode' to access.
 *
 * See LinuxBIOSv2: src/cpu/amd/model_fxx/model_fxx_init.c
 */

#define	OPTERON_MSR_PASSCODE	0x9c5a203a
 
static __inline u_int64_t
rdmsr_locked(u_int msr, u_int code)
{
	uint64_t rv;
	__asm volatile("rdmsr"
	    : "=A" (rv)
	    : "c" (msr), "D" (code));
	return (rv);
}

static __inline void
wrmsr_locked(u_int msr, u_int code, u_int64_t newval)
{
	__asm volatile("wrmsr"
	    :
	    : "A" (newval), "c" (msr), "D" (code));
}

/* Break into DDB. */
static __inline void
breakpoint(void)
{
	__asm volatile("int $3");
}

#define read_psl()	read_eflags()
#define write_psl(x)	write_eflags(x)

void amd64_errata(struct cpu_info *);

#endif /* _KERNEL */
#endif /* !_MACHINE_CPUFUNC_H_ */
