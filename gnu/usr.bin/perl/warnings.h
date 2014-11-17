/* -*- buffer-read-only: t -*-
   !!!!!!!   DO NOT EDIT THIS FILE   !!!!!!!
   This file is built by regen/warnings.pl.
   Any changes made here will be lost!
 */


#define Off(x)			((x) / 8)
#define Bit(x)			(1 << ((x) % 8))
#define IsSet(a, x)		((a)[Off(x)] & Bit(x))


#define G_WARN_OFF		0 	/* $^W == 0 */
#define G_WARN_ON		1	/* -w flag and $^W != 0 */
#define G_WARN_ALL_ON		2	/* -W flag */
#define G_WARN_ALL_OFF		4	/* -X flag */
#define G_WARN_ONCE		8	/* set if 'once' ever enabled */
#define G_WARN_ALL_MASK		(G_WARN_ALL_ON|G_WARN_ALL_OFF)

#define pWARN_STD		NULL
#define pWARN_ALL		(((STRLEN*)0)+1)    /* use warnings 'all' */
#define pWARN_NONE		(((STRLEN*)0)+2)    /* no  warnings 'all' */

#define specialWARN(x)		((x) == pWARN_STD || (x) == pWARN_ALL ||	\
				 (x) == pWARN_NONE)

/* if PL_warnhook is set to this value, then warnings die */
#define PERL_WARNHOOK_FATAL	(&PL_sv_placeholder)

/* Warnings Categories added in Perl 5.008 */

#define WARN_ALL		 0
#define WARN_CLOSURE		 1
#define WARN_DEPRECATED		 2
#define WARN_EXITING		 3
#define WARN_GLOB		 4
#define WARN_IO			 5
#define WARN_CLOSED		 6
#define WARN_EXEC		 7
#define WARN_LAYER		 8
#define WARN_NEWLINE		 9
#define WARN_PIPE		 10
#define WARN_UNOPENED		 11
#define WARN_MISC		 12
#define WARN_NUMERIC		 13
#define WARN_ONCE		 14
#define WARN_OVERFLOW		 15
#define WARN_PACK		 16
#define WARN_PORTABLE		 17
#define WARN_RECURSION		 18
#define WARN_REDEFINE		 19
#define WARN_REGEXP		 20
#define WARN_SEVERE		 21
#define WARN_DEBUGGING		 22
#define WARN_INPLACE		 23
#define WARN_INTERNAL		 24
#define WARN_MALLOC		 25
#define WARN_SIGNAL		 26
#define WARN_SUBSTR		 27
#define WARN_SYNTAX		 28
#define WARN_AMBIGUOUS		 29
#define WARN_BAREWORD		 30
#define WARN_DIGIT		 31
#define WARN_PARENTHESIS	 32
#define WARN_PRECEDENCE		 33
#define WARN_PRINTF		 34
#define WARN_PROTOTYPE		 35
#define WARN_QW			 36
#define WARN_RESERVED		 37
#define WARN_SEMICOLON		 38
#define WARN_TAINT		 39
#define WARN_THREADS		 40
#define WARN_UNINITIALIZED	 41
#define WARN_UNPACK		 42
#define WARN_UNTIE		 43
#define WARN_UTF8		 44
#define WARN_VOID		 45

/* Warnings Categories added in Perl 5.011 */

#define WARN_IMPRECISION	 46
#define WARN_ILLEGALPROTO	 47

/* Warnings Categories added in Perl 5.013 */

#define WARN_NON_UNICODE	 48
#define WARN_NONCHAR		 49
#define WARN_SURROGATE		 50

/* Warnings Categories added in Perl 5.017 */

#define WARN_EXPERIMENTAL	 51
#define WARN_EXPERIMENTAL__LEXICAL_SUBS 52
#define WARN_EXPERIMENTAL__LEXICAL_TOPIC 53
#define WARN_EXPERIMENTAL__REGEX_SETS 54
#define WARN_EXPERIMENTAL__SMARTMATCH 55

/* Warnings Categories added in Perl 5.019 */

#define WARN_EXPERIMENTAL__AUTODEREF 56
#define WARN_EXPERIMENTAL__POSTDEREF 57
#define WARN_EXPERIMENTAL__SIGNATURES 58
#define WARN_SYSCALLS		 59

#define WARNsize		15
#define WARN_ALLstring		"\125\125\125\125\125\125\125\125\125\125\125\125\125\125\125"
#define WARN_NONEstring		"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

#define isLEXWARN_on 	(PL_curcop->cop_warnings != pWARN_STD)
#define isLEXWARN_off	(PL_curcop->cop_warnings == pWARN_STD)
#define isWARN_ONCE	(PL_dowarn & (G_WARN_ON|G_WARN_ONCE))
#define isWARN_on(c,x)	(IsSet((U8 *)(c + 1), 2*(x)))
#define isWARNf_on(c,x)	(IsSet((U8 *)(c + 1), 2*(x)+1))

#define DUP_WARNINGS(p)		\
    (specialWARN(p) ? (STRLEN*)(p)	\
    : (STRLEN*)CopyD(p, PerlMemShared_malloc(sizeof(*p)+*p), sizeof(*p)+*p, \
		     			     char))

#define ckWARN(w)		Perl_ckwarn(aTHX_ packWARN(w))

/* The w1, w2 ... should be independent warnings categories; one shouldn't be
 * a subcategory of any other */

#define ckWARN2(w1,w2)		Perl_ckwarn(aTHX_ packWARN2(w1,w2))
#define ckWARN3(w1,w2,w3)	Perl_ckwarn(aTHX_ packWARN3(w1,w2,w3))
#define ckWARN4(w1,w2,w3,w4)	Perl_ckwarn(aTHX_ packWARN4(w1,w2,w3,w4))

#define ckWARN_d(w)		Perl_ckwarn_d(aTHX_ packWARN(w))
#define ckWARN2_d(w1,w2)	Perl_ckwarn_d(aTHX_ packWARN2(w1,w2))
#define ckWARN3_d(w1,w2,w3)	Perl_ckwarn_d(aTHX_ packWARN3(w1,w2,w3))
#define ckWARN4_d(w1,w2,w3,w4)	Perl_ckwarn_d(aTHX_ packWARN4(w1,w2,w3,w4))

#define WARNshift		8

#define packWARN(a)		(a                                      )

/* The a, b, ... should be independent warnings categories; one shouldn't be
 * a subcategory of any other */

#define packWARN2(a,b)		((a) | ((b)<<8)                         )
#define packWARN3(a,b,c)	((a) | ((b)<<8) | ((c)<<16)             )
#define packWARN4(a,b,c,d)	((a) | ((b)<<8) | ((c)<<16) | ((d) <<24))

#define unpackWARN1(x)		((x)        & 0xFF)
#define unpackWARN2(x)		(((x) >>8)  & 0xFF)
#define unpackWARN3(x)		(((x) >>16) & 0xFF)
#define unpackWARN4(x)		(((x) >>24) & 0xFF)

#define ckDEAD(x)							\
	   ( ! specialWARN(PL_curcop->cop_warnings) &&			\
	    ( isWARNf_on(PL_curcop->cop_warnings, WARN_ALL) || 		\
	      isWARNf_on(PL_curcop->cop_warnings, unpackWARN1(x)) ||	\
	      isWARNf_on(PL_curcop->cop_warnings, unpackWARN2(x)) ||	\
	      isWARNf_on(PL_curcop->cop_warnings, unpackWARN3(x)) ||	\
	      isWARNf_on(PL_curcop->cop_warnings, unpackWARN4(x))))

/* end of file warnings.h */

/* ex: set ro: */
