/*	$OpenBSD: frnd.c,v 1.3 1998/07/02 19:05:29 mickey Exp $	*/

/*
 * Copyright 1996 1995 by Open Software Foundation, Inc.   
 *              All Rights Reserved 
 *  
 * Permission to use, copy, modify, and distribute this software and 
 * its documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appears in all copies and 
 * that both the copyright notice and this permission notice appear in 
 * supporting documentation. 
 *  
 * OSF DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE 
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
 * FOR A PARTICULAR PURPOSE. 
 *  
 * IN NO EVENT SHALL OSF BE LIABLE FOR ANY SPECIAL, INDIRECT, OR 
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN ACTION OF CONTRACT, 
 * NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION 
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. 
 * 
 */
/*
 * pmk1.1
 */
/*
 * (c) Copyright 1986 HEWLETT-PACKARD COMPANY
 *
 * To anyone who acknowledges that this file is provided "AS IS" 
 * without any express or implied warranty:
 *     permission to use, copy, modify, and distribute this file 
 * for any purpose is hereby granted without fee, provided that 
 * the above copyright notice and this notice appears in all 
 * copies, and that the name of Hewlett-Packard Company not be 
 * used in advertising or publicity pertaining to distribution 
 * of the software without specific, written prior permission.  
 * Hewlett-Packard Company makes no representations about the 
 * suitability of this software for any purpose.
 */

#include "../spmath/float.h"
#include "../spmath/sgl_float.h"
#include "../spmath/dbl_float.h"
#include "../spmath/quad_float.h"
#include "../spmath/cnv_float.h"

/*
 *  Single Floating-point Round to Integer
 */

/*ARGSUSED*/
int
sgl_frnd(srcptr,nullptr,dstptr,status)

sgl_floating_point *srcptr, *dstptr;
void *nullptr;
unsigned int *status;
{
	register unsigned int src, result;
	register int src_exponent;
	register boolean inexact = FALSE;

	src = *srcptr;
        /*
         * check source operand for NaN or infinity
         */
        if ((src_exponent = Sgl_exponent(src)) == SGL_INFINITY_EXPONENT) {
                /*
                 * is signaling NaN?
                 */
                if (Sgl_isone_signaling(src)) {
                        /* trap if INVALIDTRAP enabled */
                        if (Is_invalidtrap_enabled()) return(INVALIDEXCEPTION);
                        /* make NaN quiet */
                        Set_invalidflag();
                        Sgl_set_quiet(src);
                }
                /*
                 * return quiet NaN or infinity
                 */
                *dstptr = src;
                return(NOEXCEPTION);
        }
	/* 
	 * Need to round?
	 */
	if ((src_exponent -= SGL_BIAS) >= SGL_P - 1) {
		*dstptr = src;
		return(NOEXCEPTION);
	}
	/*
	 * Generate result
	 */
	if (src_exponent >= 0) {
		Sgl_clear_exponent_set_hidden(src);
		result = src;
		Sgl_rightshift(result,(SGL_P-1) - (src_exponent));
		/* check for inexact */
		if (Sgl_isinexact_to_fix(src,src_exponent)) {
			inexact = TRUE;
			/*  round result  */
			switch (Rounding_mode()) {
			case ROUNDPLUS:
			     if (Sgl_iszero_sign(src)) Sgl_increment(result);
			     break;
			case ROUNDMINUS:
			     if (Sgl_isone_sign(src)) Sgl_increment(result);
			     break;
			case ROUNDNEAREST:
			     if (Sgl_isone_roundbit(src,src_exponent))
			        if (Sgl_isone_stickybit(src,src_exponent) 
				|| (Sgl_isone_lowmantissa(result))) 
					Sgl_increment(result);
			} 
		}
		Sgl_leftshift(result,(SGL_P-1) - (src_exponent));
		if (Sgl_isone_hiddenoverflow(result)) 
			Sgl_set_exponent(result,src_exponent + (SGL_BIAS+1));
		else Sgl_set_exponent(result,src_exponent + SGL_BIAS);
	}
	else {
		result = src;  		/* set sign */
		Sgl_setzero_exponentmantissa(result);
		/* check for inexact */
		if (Sgl_isnotzero_exponentmantissa(src)) {
			inexact = TRUE;
			/*  round result  */
			switch (Rounding_mode()) {
			case ROUNDPLUS:
			     if (Sgl_iszero_sign(src)) 
				Sgl_set_exponent(result,SGL_BIAS);
			     break;
			case ROUNDMINUS:
			     if (Sgl_isone_sign(src)) 
				Sgl_set_exponent(result,SGL_BIAS);
			     break;
			case ROUNDNEAREST:
			     if (src_exponent == -1)
			        if (Sgl_isnotzero_mantissa(src))
				   Sgl_set_exponent(result,SGL_BIAS);
			} 
		}
	}
	*dstptr = result;
	if (inexact) {
		if (Is_inexacttrap_enabled()) return(INEXACTEXCEPTION);
		else Set_inexactflag();
	}
	return(NOEXCEPTION);
} 

/*
 *  Double Floating-point Round to Integer
 */

/*ARGSUSED*/
int
dbl_frnd(srcptr,nullptr,dstptr,status)

dbl_floating_point *srcptr, *dstptr;
void *nullptr;
unsigned int *status;
{
	register unsigned int srcp1, srcp2, resultp1, resultp2;
	register int src_exponent;
	register boolean inexact = FALSE;

	Dbl_copyfromptr(srcptr,srcp1,srcp2);
        /*
         * check source operand for NaN or infinity
         */
        if ((src_exponent = Dbl_exponent(srcp1)) == DBL_INFINITY_EXPONENT) {
                /*
                 * is signaling NaN?
                 */
                if (Dbl_isone_signaling(srcp1)) {
                        /* trap if INVALIDTRAP enabled */
                        if (Is_invalidtrap_enabled()) return(INVALIDEXCEPTION);
                        /* make NaN quiet */
                        Set_invalidflag();
                        Dbl_set_quiet(srcp1);
                }
                /*
                 * return quiet NaN or infinity
                 */
                Dbl_copytoptr(srcp1,srcp2,dstptr);
                return(NOEXCEPTION);
        }
	/* 
	 * Need to round?
	 */
	if ((src_exponent -= DBL_BIAS) >= DBL_P - 1) {
		Dbl_copytoptr(srcp1,srcp2,dstptr);
		return(NOEXCEPTION);
	}
	/*
	 * Generate result
	 */
	if (src_exponent >= 0) {
		Dbl_clear_exponent_set_hidden(srcp1);
		resultp1 = srcp1;
		resultp2 = srcp2;
		Dbl_rightshift(resultp1,resultp2,(DBL_P-1) - (src_exponent));
		/* check for inexact */
		if (Dbl_isinexact_to_fix(srcp1,srcp2,src_exponent)) {
			inexact = TRUE;
			/*  round result  */
			switch (Rounding_mode()) {
			case ROUNDPLUS:
			     if (Dbl_iszero_sign(srcp1)) 
				Dbl_increment(resultp1,resultp2);
			     break;
			case ROUNDMINUS:
			     if (Dbl_isone_sign(srcp1)) 
				Dbl_increment(resultp1,resultp2);
			     break;
			case ROUNDNEAREST:
			     if (Dbl_isone_roundbit(srcp1,srcp2,src_exponent))
			      if (Dbl_isone_stickybit(srcp1,srcp2,src_exponent) 
				  || (Dbl_isone_lowmantissap2(resultp2))) 
					Dbl_increment(resultp1,resultp2);
			} 
		}
		Dbl_leftshift(resultp1,resultp2,(DBL_P-1) - (src_exponent));
		if (Dbl_isone_hiddenoverflow(resultp1))
			Dbl_set_exponent(resultp1,src_exponent + (DBL_BIAS+1));
		else Dbl_set_exponent(resultp1,src_exponent + DBL_BIAS);
	}
	else {
		resultp1 = srcp1;  /* set sign */
		Dbl_setzero_exponentmantissa(resultp1,resultp2);
		/* check for inexact */
		if (Dbl_isnotzero_exponentmantissa(srcp1,srcp2)) {
			inexact = TRUE;
			/*  round result  */
			switch (Rounding_mode()) {
			case ROUNDPLUS:
			     if (Dbl_iszero_sign(srcp1)) 
				Dbl_set_exponent(resultp1,DBL_BIAS);
			     break;
			case ROUNDMINUS:
			     if (Dbl_isone_sign(srcp1)) 
				Dbl_set_exponent(resultp1,DBL_BIAS);
			     break;
			case ROUNDNEAREST:
			     if (src_exponent == -1)
			        if (Dbl_isnotzero_mantissa(srcp1,srcp2))
				   Dbl_set_exponent(resultp1,DBL_BIAS);
			} 
		}
	}
	Dbl_copytoptr(resultp1,resultp2,dstptr);
	if (inexact) {
		if (Is_inexacttrap_enabled()) return(INEXACTEXCEPTION);
		else Set_inexactflag();
	}
	return(NOEXCEPTION);
}

/*ARGSUSED*/
int
quad_frnd(srcptr,nullptr,dstptr,status)

quad_floating_point *srcptr, *dstptr;
void *nullptr;
unsigned int *status;
{
	return(UNIMPLEMENTEDEXCEPTION);
}

