/* $OpenBSD: kbind.h,v 1.1 2016/03/20 02:32:39 guenther Exp $ */

/*
 * Copyright (c) 2016 Philip Guenther <guenther@openbsd.org>
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

#include <sys/syscall.h>
#include <machine/vmparam.h>	/* SYSCALLGATE */

#define	MD_DISABLE_KBIND						\
do {									\
	register long r1 __asm__("r1") = SYSCALLGATE;			\
	register void *arg0 __asm__("r26") = NULL;			\
	__asm__ __volatile__ ("ble 4(%%sr7, %%r1) ! ldi %0, %%r22"	\
	    :								\
	    : "i" (SYS_kbind), "r" (r1), "r"(arg0)			\
	    : "r22", "r28", "r29", "cc", "memory");			\
} while (0)
