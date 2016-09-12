
#ifndef _LIBM_MATH_H_
#define _LIBM_MATH_H_

#include_next <math.h>

#if 0
extern PROTO_NORMAL(signgam);
#endif

PROTO_NORMAL(acos);
PROTO_STD_DEPRECATED(acosf);
PROTO_NORMAL(acosh);
PROTO_STD_DEPRECATED(acoshf);
LDBL_PROTO_STD_DEPRECATED(acoshl);
LDBL_PROTO_STD_DEPRECATED(acosl);
PROTO_NORMAL(asin);
PROTO_NORMAL(asinf);
PROTO_NORMAL(asinh);
PROTO_STD_DEPRECATED(asinhf);
LDBL_PROTO_STD_DEPRECATED(asinhl);
LDBL_PROTO_NORMAL(asinl);
PROTO_NORMAL(atan);
PROTO_NORMAL(atan2);
PROTO_NORMAL(atan2f);
LDBL_PROTO_NORMAL(atan2l);
PROTO_NORMAL(atanf);
PROTO_NORMAL(atanh);
PROTO_STD_DEPRECATED(atanhf);
LDBL_PROTO_STD_DEPRECATED(atanhl);
LDBL_PROTO_NORMAL(atanl);
PROTO_NORMAL(cbrt);
PROTO_STD_DEPRECATED(cbrtf);
LDBL_PROTO_STD_DEPRECATED(cbrtl);
PROTO_NORMAL(ceil);
PROTO_NORMAL(ceilf);
PROTO_STD_DEPRECATED(ceill);
PROTO_NORMAL(copysign);
PROTO_NORMAL(copysignf);
LDBL_PROTO_NORMAL(copysignl);
PROTO_NORMAL(cos);
PROTO_NORMAL(cosf);
PROTO_NORMAL(cosh);
PROTO_NORMAL(coshf);
LDBL_PROTO_NORMAL(coshl);
LDBL_PROTO_NORMAL(cosl);
PROTO_DEPRECATED(drem);
PROTO_DEPRECATED(dremf);
PROTO_NORMAL(erf);
PROTO_NORMAL(erfc);
PROTO_STD_DEPRECATED(erfcf);
PROTO_NORMAL(erfcl);
PROTO_STD_DEPRECATED(erff);
LDBL_PROTO_NORMAL(erfl);
PROTO_NORMAL(exp);
PROTO_NORMAL(exp2);
PROTO_STD_DEPRECATED(exp2f);
LDBL_PROTO_STD_DEPRECATED(exp2l);
PROTO_NORMAL(expf);
PROTO_NORMAL(expl);
PROTO_NORMAL(expm1);
PROTO_NORMAL(expm1f);
LDBL_PROTO_NORMAL(expm1l);
PROTO_NORMAL(fabsf);
PROTO_STD_DEPRECATED(fdim);
PROTO_STD_DEPRECATED(fdimf);
PROTO_STD_DEPRECATED(fdiml);
PROTO_NORMAL(floor);
PROTO_NORMAL(floorf);
PROTO_NORMAL(floorl);
PROTO_NORMAL(fma);
PROTO_STD_DEPRECATED(fmaf);
LDBL_PROTO_STD_DEPRECATED(fmal);
PROTO_NORMAL(fmax);
PROTO_STD_DEPRECATED(fmaxf);
LDBL_PROTO_STD_DEPRECATED(fmaxl);
PROTO_NORMAL(fmin);
PROTO_STD_DEPRECATED(fminf);
LDBL_PROTO_STD_DEPRECATED(fminl);
PROTO_NORMAL(fmod);
PROTO_NORMAL(fmodf);
LDBL_PROTO_STD_DEPRECATED(fmodl);
PROTO_STD_DEPRECATED(frexpf);
LDBL_PROTO_NORMAL(frexpl);
PROTO_DEPRECATED(gamma);
PROTO_DEPRECATED(gamma_r);
PROTO_DEPRECATED(gammaf);
PROTO_DEPRECATED(gammaf_r);
PROTO_NORMAL(hypot);
PROTO_NORMAL(hypotf);
LDBL_PROTO_NORMAL(hypotl);
PROTO_NORMAL(ilogb);
PROTO_NORMAL(ilogbf);
LDBL_PROTO_NORMAL(ilogbl);
PROTO_NORMAL(j0);
PROTO_NORMAL(j0f);
PROTO_NORMAL(j1);
PROTO_NORMAL(j1f);
PROTO_DEPRECATED(jn);
PROTO_DEPRECATED(jnf);
PROTO_STD_DEPRECATED(ldexpf);
LDBL_PROTO_NORMAL(ldexpl);
PROTO_NORMAL(lgamma);
PROTO_NORMAL(lgamma_r);
PROTO_STD_DEPRECATED(lgammaf);
PROTO_NORMAL(lgammaf_r);
LDBL_PROTO_NORMAL(lgammal);
PROTO_NORMAL(llrint);
PROTO_NORMAL(llrintf);
LDBL_PROTO_STD_DEPRECATED(llrintl);
PROTO_NORMAL(llround);
PROTO_STD_DEPRECATED(llroundf);
PROTO_STD_DEPRECATED(llroundl);
PROTO_NORMAL(log);
PROTO_NORMAL(log10);
PROTO_STD_DEPRECATED(log10f);
LDBL_PROTO_STD_DEPRECATED(log10l);
PROTO_NORMAL(log1p);
PROTO_NORMAL(log1pf);
PROTO_NORMAL(log1pl);
PROTO_NORMAL(log2);
PROTO_STD_DEPRECATED(log2f);
LDBL_PROTO_STD_DEPRECATED(log2l);
PROTO_NORMAL(logb);
PROTO_NORMAL(logbf);
LDBL_PROTO_STD_DEPRECATED(logbl);
PROTO_NORMAL(logf);
LDBL_PROTO_NORMAL(logl);
PROTO_NORMAL(lrint);
PROTO_NORMAL(lrintf);
LDBL_PROTO_STD_DEPRECATED(lrintl);
PROTO_NORMAL(lround);
PROTO_STD_DEPRECATED(lroundf);
PROTO_STD_DEPRECATED(lroundl);
PROTO_STD_DEPRECATED(modff);
LDBL_PROTO_STD_DEPRECATED(modfl);
PROTO_NORMAL(nan);
PROTO_STD_DEPRECATED(nanf);
LDBL_PROTO_STD_DEPRECATED(nanl);
PROTO_STD_DEPRECATED(nearbyint);
PROTO_STD_DEPRECATED(nearbyintf);
PROTO_STD_DEPRECATED(nearbyintl);
PROTO_NORMAL(nextafter);
PROTO_STD_DEPRECATED(nextafterf);
PROTO_NORMAL(nextafterl);
PROTO_STD_DEPRECATED(nexttoward);
PROTO_STD_DEPRECATED(nexttowardf);
PROTO_STD_DEPRECATED(nexttowardl);
PROTO_NORMAL(pow);
PROTO_NORMAL(powf);
LDBL_PROTO_NORMAL(powl);
PROTO_NORMAL(remainder);
PROTO_NORMAL(remainderf);
LDBL_PROTO_STD_DEPRECATED(remainderl);
PROTO_NORMAL(remquo);
PROTO_STD_DEPRECATED(remquof);
LDBL_PROTO_NORMAL(remquol);
PROTO_NORMAL(rint);
PROTO_NORMAL(rintf);
PROTO_NORMAL(rintl);
PROTO_NORMAL(round);
PROTO_STD_DEPRECATED(roundf);
LDBL_PROTO_NORMAL(roundl);
PROTO_NORMAL(scalb);
PROTO_NORMAL(scalbf);
PROTO_STD_DEPRECATED(scalbln);
PROTO_STD_DEPRECATED(scalblnf);
PROTO_STD_DEPRECATED(scalblnl);
PROTO_NORMAL(scalbn);
PROTO_NORMAL(scalbnf);
PROTO_NORMAL(scalbnl);
PROTO_DEPRECATED(significand);
PROTO_DEPRECATED(significandf);
PROTO_NORMAL(sin);
PROTO_NORMAL(sinf);
PROTO_NORMAL(sinh);
PROTO_NORMAL(sinhf);
LDBL_PROTO_NORMAL(sinhl);
LDBL_PROTO_NORMAL(sinl);
PROTO_NORMAL(sqrt);
PROTO_NORMAL(sqrtf);
LDBL_PROTO_NORMAL(sqrtl);
PROTO_NORMAL(tan);
PROTO_STD_DEPRECATED(tanf);
PROTO_NORMAL(tanh);
PROTO_STD_DEPRECATED(tanhf);
LDBL_PROTO_STD_DEPRECATED(tanhl);
LDBL_PROTO_STD_DEPRECATED(tanl);
PROTO_NORMAL(tgamma);
PROTO_STD_DEPRECATED(tgammaf);
LDBL_PROTO_STD_DEPRECATED(tgammal);
PROTO_NORMAL(trunc);
PROTO_STD_DEPRECATED(truncf);
LDBL_PROTO_STD_DEPRECATED(truncl);
PROTO_NORMAL(y0);
PROTO_NORMAL(y0f);
PROTO_NORMAL(y1);
PROTO_NORMAL(y1f);
PROTO_DEPRECATED(yn);
PROTO_DEPRECATED(ynf);

#endif /* !_LIBM_MATH_H_ */
