/*	$OpenBSD: s_csinh.c,v 1.3 2012/12/05 23:20:04 deraadt Exp $	*/
/*
 * Copyright (c) 2008 Stephen L. Moshier <steve@moshier.net>
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

/* LINTLIBRARY */

/*							csinh
 *
 *	Complex hyperbolic sine
 *
 *
 *
 * SYNOPSIS:
 *
 * double complex csinh();
 * double complex z, w;
 *
 * w = csinh (z);
 *
 * DESCRIPTION:
 *
 * csinh z = (cexp(z) - cexp(-z))/2
 *         = sinh x * cos y  +  i cosh x * sin y .
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -10,+10     30000       3.1e-16     8.2e-17
 *
 */

#include <complex.h>
#include <float.h>
#include <math.h>

double complex
csinh(double complex z)
{
	double complex w;
	double x, y;

	x = creal(z);
	y = cimag(z);
	w = sinh (x) * cos (y)  +  (cosh (x) * sin (y)) * I;
	return (w);
}

#if	LDBL_MANT_DIG == 53
#ifdef	lint
/* PROTOLIB1 */
long double complex csinhl(long double complex);
#else	/* lint */
__weak_alias(csinhl, csinh);
#endif	/* lint */
#endif	/* LDBL_MANT_DIG == 53 */
