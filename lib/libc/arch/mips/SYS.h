/*-
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Ralph Campbell.
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
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
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
 *	from: @(#)SYS.h	8.1 (Berkeley) 6/4/93
 *      $Id: SYS.h,v 1.2 1996/05/16 11:15:57 pefo Exp $ 
 */

#include <sys/syscall.h>
#if MACHINE==pica
#include <machine/asm.h>
#else
#include <machine/machAsmDefs.h>
#endif

#ifdef __STDC__
#ifdef ABICALLS
#define RSYSCALL(x)     .abicalls; \
			LEAF(x); .set noreorder; .cpload t9; .set reorder; \
			li v0,SYS_ ## x; syscall; \
			bne a3,zero,err; j ra; \
			err: la t9, _C_LABEL(cerror); jr t9; END(x);
#define PSEUDO(x,y)     .abicalls; \
			LEAF(x); .set noreorder; .cpload t9; .set reorder; \
			li v0,SYS_ ## y; syscall; \
			bne a3,zero,err; j ra; \
			err: la t9, _C_LABEL(cerror); jr t9; END(x);
#else
#define RSYSCALL(x)     LEAF(x); li v0,SYS_ ## x; syscall; \
			bne a3,zero,err; j ra; err: j _C_LABEL(cerror); END(x);
#define PSEUDO(x,y)     LEAF(x); li v0,SYS_ ## y; syscall; \
			bne a3,zero,err; j ra; err: j _C_LABEL(cerror); END(x);
#endif
#else
#ifdef ABICALLS
#define RSYSCALL(x)     .abicalls; \
			LEAF(x); .set noreorder; .cpload t9; .set reorder; \
			li v0,SYS_/**/x; syscall; \
			bne a3,zero,err; j ra; \
			err: la t9, _C_LABEL(cerror); jr t9; END(x);
#define PSEUDO(x,y)     .abicalls; \
			LEAF(x); .set noreorder; .cpload t9; .set reorder; \
			li v0,SYS_/**/y; syscall; \
			bne a3,zero,err; j ra; \
			err: la t9, _C_LABEL(cerror); jr t9; END(x);
#else
#define RSYSCALL(x)     LEAF(x); li v0,SYS_/**/x; syscall; \
			bne a3,zero,err; j ra; err: j _C_LABEL(cerror); END(x);
#define PSEUDO(x,y)     LEAF(x); li v0,SYS_/**/y; syscall; \
			bne a3,zero,err; j ra; err: j _C_LABEL(cerror); END(x);
#endif
#endif
