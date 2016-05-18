/*	$OpenBSD: tcb.h,v 1.2 2016/05/18 20:21:13 guenther Exp $	*/

/*
 * Copyright (c) 2011 Philip Guenther <guenther@openbsd.org>
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

#ifndef _MACHINE_TCB_H_
#define _MACHINE_TCB_H_

#ifdef _KERNEL

/*
 * In userspace, register gbr contains the address of the thread's TCB
 */
#define TCB_GET(p)		((void *)(p)->p_md.md_regs->tf_gbr)
#define TCB_SET(p, addr)	((p)->p_md.md_regs->tf_gbr = (int)(addr))

#else /* _KERNEL */

/* ELF TLS ABI calls for small TCB, with static TLS data after it */
#define TLS_VARIANT	1

/* Get a pointer to the TCB itself */
static inline void *
__sh_get_tcb(void)
{
	void *__val;
	__asm__ ("stc gbr, %0" : "=r" (__val));
	return __val;
}
#define TCB_GET()	__sh_get_tcb()
#define TCB_SET(tcb)	__asm__ ("ldc %0, gbr" : : "r" (tcb));

#endif /* _KERNEL */

#endif /* _MACHINE_TCB_H_ */
