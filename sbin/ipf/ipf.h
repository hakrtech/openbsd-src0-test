/*
 * (C)opyright 1993-1996 by Darren Reed.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that this notice is preserved and due credit is given
 * to the original author and the contributors.
 *
 * @(#)ipf.h	1.12 6/5/96
 * $DRId: ipf.h,v 2.0.1.1 1997/01/09 15:14:43 darrenr Exp $
 */

#ifndef	SOLARIS
#define	SOLARIS	(defined(sun) && (defined(__svr4__) || defined(__SVR4)))
#endif
#define	OPT_REMOVE	0x00001
#define	OPT_DEBUG	0x00002
#define	OPT_OUTQUE	FR_OUTQUE	/* 0x0004 */
#define	OPT_INQUE	FR_INQUE	/* 0x0008 */
#define	OPT_LOG		FR_LOG		/* 0x0010 */
#define	OPT_SHOWLIST	0x00020
#define	OPT_VERBOSE	0x00040
#define	OPT_DONOTHING	0x00080
#define	OPT_HITS	0x00100
#define	OPT_BRIEF	0x00200
#define OPT_ACCNT	FR_ACCOUNT	/* 0x0400 */
#define	OPT_FRSTATES	FR_KEEPFRAG	/* 0x0800 */
#define	OPT_IPSTATES	FR_KEEPSTATE	/* 0x1000 */
#define	OPT_INACTIVE	FR_INACTIVE	/* 0x2000 */
#define	OPT_SHOWLINENO	0x04000
#define	OPT_PRINTFR	0x08000
#define	OPT_ZERORULEST	0x10000

extern	struct	frentry	*parse();

extern	void	printfr(), binprint(), initparse();

#if defined(__SVR4) || defined(__svr4__)
#define	index	strchr
#define	bzero(a,b)	memset(a, 0, b)
#define	bcopy(a,b,c)	memmove(b,a,c)
#endif

struct	ipopt_names	{
	int	on_value;
	int	on_bit;
	int	on_siz;
	char	*on_name;
};


extern	u_long	hostnum(), optname();
extern	void	printpacket();
#if SOLARIS
extern	int	inet_aton();
#endif

#ifdef	sun
#define	STRERROR(x)	sys_errlist[x]
extern	char	*sys_errlist[];
#else
#define	STRERROR(x)	strerror(x)
#endif

#ifndef	MIN
#define	MIN(a,b)	((a) > (b) ? (b) : (a))
#endif

