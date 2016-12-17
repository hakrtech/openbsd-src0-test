/* $OpenBSD: reg.h,v 1.1 2016/12/17 23:38:33 patrick Exp $ */
/*
 * Copyright (c) 2014 Patrick Wildt <patrick@blueri.se>
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

#ifndef _MACHINE_REG_H_
#define _MACHINE_REG_H_

struct reg {
	unsigned long x[30];
	unsigned long x_sp;
	unsigned long x_lr;
	unsigned long x_pc;
	unsigned long x_cpsr;
};

struct fpreg {
	uint64_t	fp_registers[64]; // really 32 128 bit registers.
	uint32_t	fp_sr;
	uint32_t	fp_cr;
};

#endif /* !_MACHINE_REG_H_ */
