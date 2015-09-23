/*	$OpenBSD: drm_fixed.h,v 1.2 2015/09/23 23:12:11 kettenis Exp $	*/
/*
 * Copyright 2009 Red Hat Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors: Dave Airlie
 *          Christian König
 */
#ifndef DRM_FIXED_H
#define DRM_FIXED_H

typedef union dfixed {
	u32 full;
} fixed20_12;


#define dfixed_const(A) (u32)(((A) << 12))/*  + ((B + 0.000122)*4096)) */
#define dfixed_const_half(A) (u32)(((A) << 12) + 2048)
#define dfixed_const_666(A) (u32)(((A) << 12) + 2731)
#define dfixed_const_8(A) (u32)(((A) << 12) + 3277)
#define dfixed_mul(A, B) ((u64)((u64)(A).full * (B).full + 2048) >> 12)
#define dfixed_init(A) { .full = dfixed_const((A)) }
#define dfixed_init_half(A) { .full = dfixed_const_half((A)) }
#define dfixed_trunc(A) ((A).full >> 12)
#define dfixed_frac(A) ((A).full & ((1 << 12) - 1))

static inline u32 dfixed_floor(fixed20_12 A)
{
	u32 non_frac = dfixed_trunc(A);

	return dfixed_const(non_frac);
}

static inline u32 dfixed_ceil(fixed20_12 A)
{
	u32 non_frac = dfixed_trunc(A);

	if (A.full > dfixed_const(non_frac))
		return dfixed_const(non_frac + 1);
	else
		return dfixed_const(non_frac);
}

static inline u32 dfixed_div(fixed20_12 A, fixed20_12 B)
{
	u64 tmp = ((u64)A.full << 13);

	tmp /= B.full;
	tmp += 1;
	tmp /= 2;

	return ((u32)tmp);
}

#define DRM_FIXED_POINT		32
#define DRM_FIXED_ONE		(1ULL << DRM_FIXED_POINT)
#define DRM_FIXED_DECIMAL_MASK	(DRM_FIXED_ONE - 1)
#define DRM_FIXED_DIGITS_MASK	(~DRM_FIXED_DECIMAL_MASK)

static inline s64 drm_int2fixp(int a)
{
	return ((s64)a) << DRM_FIXED_POINT;
}

static inline int drm_fixp2int(int64_t a)
{
	return ((s64)a) >> DRM_FIXED_POINT;
}

static inline unsigned drm_fixp_msbset(int64_t a)
{
	unsigned shift, sign = (a >> 63) & 1;

	for (shift = 62; shift > 0; --shift)
		if (((a >> shift) & 1) != sign)
			return shift;

	return 0;
}

static inline s64 drm_fixp_mul(s64 a, s64 b)
{
	unsigned shift = drm_fixp_msbset(a) + drm_fixp_msbset(b);
	s64 result;

	if (shift > 61) {
		shift = shift - 61;
		a >>= (shift >> 1) + (shift & 1);
		b >>= shift >> 1;
	} else
		shift = 0;

	result = a * b;

	if (shift > DRM_FIXED_POINT)
		return result << (shift - DRM_FIXED_POINT);

	if (shift < DRM_FIXED_POINT)
		return result >> (DRM_FIXED_POINT - shift);

	return result;
}

static inline s64 drm_fixp_div(s64 a, s64 b)
{
	unsigned shift = 62 - drm_fixp_msbset(a);
	s64 result;

	a <<= shift;

	if (shift < DRM_FIXED_POINT)
		b >>= (DRM_FIXED_POINT - shift);

	result = div64_s64(a, b);

	if (shift > DRM_FIXED_POINT)
		return result >> (shift - DRM_FIXED_POINT);

	return result;
}

static inline s64 drm_fixp_exp(s64 x)
{
	s64 tolerance = div64_s64(DRM_FIXED_ONE, 1000000);
	s64 sum = DRM_FIXED_ONE, term, y = x;
	u64 count = 1;

	if (x < 0)
		y = -1 * x;

	term = y;

	while (term >= tolerance) {
		sum = sum + term;
		count = count + 1;
		term = drm_fixp_mul(term, div64_s64(y, count));
	}

	if (x < 0)
		sum = drm_fixp_div(DRM_FIXED_ONE, sum);

	return sum;
}

#endif
