/*	$OpenBSD: SYS.h,v 1.7 2015/04/07 01:27:06 guenther Exp $	*/

/*
 * Copyright (c) 1998-2002 Michael Shalayeff
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF MIND
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/syscall.h>
#include <machine/asm.h>
#undef _LOCORE
#define _LOCORE
#include <machine/frame.h>
#include <machine/vmparam.h>
#undef _LOCORE

#define SYSENTRY(x)				!\
LEAF_ENTRY(__CONCAT(_thread_sys_,x))		!\
	WEAK_ALIAS(x,__CONCAT(_thread_sys_,x))
#define SYSENTRY_HIDDEN(x)			!\
LEAF_ENTRY(__CONCAT(_thread_sys_,x))
#define	SYSEXIT(x)				!\
EXIT(__CONCAT(_thread_sys_,x))

#define	SYSCALL(x)				!\
	std	%rp, HPPA_FRAME_RP(%sr0,%sp)	!\
	ldil	L%SYSCALLGATE, %r1		!\
	depd	%r0, 31, 32, %r1		!\
	ble	4(%sr7, %r1)			!\
	ldi	__CONCAT(SYS_,x), %r1		!\
	.import	__cerror, code			!\
	sub,*=	%r0, %r1, %r0			!\
	b,l	__cerror, %rp			!\
	ldd	HPPA_FRAME_RP(%sr0,%sp), %rp

#define	PSEUDO(x,y)				!\
SYSENTRY(x)					!\
	SYSCALL(y)				!\
	bv	%r0(%rp)			!\
	nop					!\
SYSEXIT(x)
#define	PSEUDO_HIDDEN(x,y)			!\
SYSENTRY_HIDDEN(x)				!\
	SYSCALL(y)				!\
	bv	%r0(%rp)			!\
	nop					!\
SYSEXIT(x)

#define	PSEUDO_NOERROR(x,y)			!\
SYSENTRY(x)					!\
	std	%rp, HPPA_FRAME_RP(%sr0,%sp)	!\
	ldil	L%SYSCALLGATE, %r1		!\
	depd	%r0, 31, 32, %r1		!\
	ble	4(%sr7, %r1)			!\
	ldi	__CONCAT(SYS_,y), %r1		!\
	ldd	HPPA_FRAME_RP(%sr0,%sp), %rp	!\
	bv	%r0(%rp)			!\
	nop					!\
SYSEXIT(x)

#define	RSYSCALL(x)		PSEUDO(x,x)
#define	RSYSCALL_HIDDEN(x)	PSEUDO_HIDDEN(x,x)

