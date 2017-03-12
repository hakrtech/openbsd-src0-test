/*	$OpenBSD: sig_machdep.c,v 1.3 2017/03/12 17:57:12 kettenis Exp $ */

/*
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * William Jolitz and Don Ahn.
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
 *      This product includes software developed by the University of
 *      California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
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
 */
/*
 * Copyright (c) 2001 Opsycon AB  (www.opsycon.se)
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <sys/param.h>

#include <sys/mount.h>		/* XXX only needed by syscallargs.h */
#include <sys/proc.h>
#include <sys/signal.h>
#include <sys/signalvar.h>
#include <sys/syscallargs.h>
#include <sys/systm.h>
#include <sys/user.h>

#include <arm64/armreg.h>

#include <machine/cpu.h>
#include <machine/frame.h>
#include <machine/pcb.h>

static __inline struct trapframe *
process_frame(struct proc *p)
{
	return p->p_addr->u_pcb.pcb_tf;
}

/*
 * Send an interrupt to process.
 *
 * Stack is set up to allow sigcode stored
 * in u. to call routine, followed by kcall
 * to sigreturn routine below.  After sigreturn
 * resets the signal mask, the stack, and the
 * frame pointer, it returns to the user specified pc.
 */
void
sendsig(sig_t catcher, int sig, int returnmask, u_long code, int type,
   union sigval val)
{
	struct proc *p = curproc;
	struct trapframe *tf;
	struct sigframe *fp, frame;
	struct sigacts *psp = p->p_p->ps_sigacts;
	siginfo_t *sip = NULL;
	int i;

	tf = process_frame(p);

	/* Allocate space for the signal handler context. */
	if ((p->p_sigstk.ss_flags & SS_DISABLE) == 0 &&
	    !sigonstack(tf->tf_sp) && (psp->ps_sigonstack & sigmask(sig)))
		fp = (struct sigframe *)((caddr_t)p->p_sigstk.ss_sp +
		    p->p_sigstk.ss_size);
	else
		fp = (struct sigframe *)tf->tf_sp;

	/* make room on the stack */
	fp--;

	/* make the stack aligned */
	fp = (struct sigframe *)STACKALIGN(fp);

	/* Build stack frame for signal trampoline. */
	bzero(&frame, sizeof(frame));
	frame.sf_signum = sig;

	/* Save register context. */
	for (i=0; i < 30; i++)
		frame.sf_sc.sc_x[i] = tf->tf_x[i];
	frame.sf_sc.sc_sp = tf->tf_sp;
	frame.sf_sc.sc_lr = tf->tf_lr;
	frame.sf_sc.sc_elr = tf->tf_elr;
	frame.sf_sc.sc_spsr = tf->tf_spsr;

	/* Save signal mask. */
	frame.sf_sc.sc_mask = returnmask;

	/* XXX Save floating point context */

	if (psp->ps_siginfo & sigmask(sig)) {
		sip = &fp->sf_si;
		initsiginfo(&frame.sf_si, sig, code, type, val);
	}

	frame.sf_sc.sc_cookie = (long)&fp->sf_sc ^ p->p_p->ps_sigcookie;
	if (copyout(&frame, fp, sizeof(frame)) != 0) {
		/*
		 * Process has trashed its stack; give it an illegal
		 * instruction to halt it in its tracks.
		 */
		sigexit(p, SIGILL);
		/* NOTREACHED */
	}

	/*
	 * Build context to run handler in.  We invoke the handler
	 * directly, only returning via the trampoline.
         */
	tf->tf_x[0] = sig;
	tf->tf_x[1] = (register_t)sip;
	tf->tf_x[2] = (register_t)&fp->sf_sc;
	tf->tf_elr = (register_t)catcher;
	tf->tf_sp = (register_t)fp;

	tf->tf_lr = p->p_p->ps_sigcode;
}

/*
 * System call to cleanup state after a signal
 * has been taken.  Reset signal mask and
 * stack state from context left by sendsig (above).
 * Return to previous pc and psl as specified by
 * context left by sendsig. Check carefully to
 * make sure that the user has not modified the
 * psr to gain improper privileges or to cause
 * a machine fault.
 */

int
sys_sigreturn(struct proc *p, void *v, register_t *retval)
{
	struct sys_sigreturn_args /* {
		syscallarg(struct sigcontext *) sigcntxp;
	} */ *uap = v;
	struct sigcontext ksc, *scp = SCARG(uap, sigcntxp);
	struct trapframe *tf;
	int i;

	if (PROC_PC(p) != p->p_p->ps_sigcoderet) {
		sigexit(p, SIGILL);
		return (EPERM);
	}

	if (copyin(scp, &ksc, sizeof(*scp)) != 0)
		return (EFAULT);

	if (ksc.sc_cookie != ((long)scp ^ p->p_p->ps_sigcookie)) {
		sigexit(p, SIGILL);
		return (EFAULT);
	}

	/* Prevent reuse of the sigcontext cookie */
	ksc.sc_cookie = 0;
	(void)copyout(&ksc.sc_cookie, (caddr_t)scp +
	    offsetof(struct sigcontext, sc_cookie), sizeof (ksc.sc_cookie));

	/*
	 * Make sure the processor mode has not been tampered with and
	 * interrupts have not been disabled.
	 */
	if ((ksc.sc_spsr & PSR_M_MASK) != PSR_M_EL0t ||
	    (ksc.sc_spsr & (PSR_I | PSR_F)) != 0)
		return (EINVAL);

	/* XXX Restore floating point context */

	/* Restore register context. */
	tf = process_frame(p);
	for (i=0; i < 30; i++)
		tf->tf_x[i] = ksc.sc_x[i];
	tf->tf_sp = ksc.sc_sp;
	tf->tf_lr = ksc.sc_lr;
	tf->tf_elr = ksc.sc_elr;
	tf->tf_spsr = ksc.sc_spsr;

	/* Restore signal mask. */
	p->p_sigmask = ksc.sc_mask & ~sigcantmask;

	return (EJUSTRETURN);
}
