/*	$NetBSD: clock.c,v 1.4 1995/08/03 00:53:34 cgd Exp $	*/

/*
 * Copyright (c) 1988 University of Utah.
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * the Systems Programming Group of the University of Utah Computer
 * Science Department and Ralph Campbell.
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
 * from: Utah Hdr: clock.c 1.18 91/01/21
 *
 *	@(#)clock.c	8.1 (Berkeley) 6/10/93
 */

#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/device.h>

#include <machine/autoconf.h>
#include <machine/rpb.h>

#include <alpha/alpha/clockvar.h>
#include <alpha/tc/asic.h>			/* XXX */
#include <dev/isa/isavar.h>			/* XXX */

/* Definition of the driver for autoconfig. */
static int	clockmatch __P((struct device *, void *, void *));
static void	clockattach __P((struct device *, struct device *, void *));
struct cfdriver clockcd =
    { NULL, "clock", clockmatch, clockattach, DV_DULL,
	sizeof(struct clock_softc) };

#if defined(DEC_3000_500) || defined(DEC_3000_300) || \
    defined(DEC_2000_300) || defined(DEC_2100_A50)
void	mcclock_attach __P((struct device *parent,
	    struct device *self, void *aux));
#endif

#define	SECMIN	((unsigned)60)			/* seconds per minute */
#define	SECHOUR	((unsigned)(60*SECMIN))		/* seconds per hour */
#define	SECDAY	((unsigned)(24*SECHOUR))	/* seconds per day */
#define	SECYR	((unsigned)(365*SECDAY))	/* seconds per common year */

#define	LEAPYEAR(year)	(((year) % 4) == 0)

static int
clockmatch(parent, cfdata, aux)
	struct device *parent;
	void *cfdata;
	void *aux;
{
	struct cfdata *cf = cfdata;
	struct confargs *ca = aux;

	/* See how many clocks this system has */	
	switch (hwrpb->rpb_type) {
#if defined(DEC_3000_500) || defined(DEC_3000_300)

#if defined(DEC_3000_500)
	case ST_DEC_3000_500:
#endif
#if defined(DEC_3000_300)
	case ST_DEC_3000_300:
#endif
		/* make sure that we're looking for this type of device. */
		if (!BUS_MATCHNAME(ca, "dallas_rtc"))
			return (0);

		if (cf->cf_unit >= 1)
			return (0);

		break;
#endif

#if defined(DEC_2100_A50)
	case ST_DEC_2100_A50:
		/* Just say yes.  XXX */

		if (cf->cf_unit >= 1)
			return 0;

		/* XXX XXX XXX */
		{
			struct isa_attach_args *ia = aux;

			if (ia->ia_iobase != 0x70 &&		/* XXX */
			    ia->ia_iobase != -1)		/* XXX */
				return (0);

			ia->ia_iobase = 0x70;			/* XXX */
			ia->ia_iosize = 2;			/* XXX */
			ia->ia_msize = 0;
		}

		break;
#endif

#if defined(DEC_2000_300)
	case ST_DEC_2000_300:
		panic("clockmatch on jensen");
#endif

	default:
		panic("unknown CPU");
	}

	return (1);
}

static void
clockattach(parent, self, aux)
	struct device *parent;
	struct device *self;
	void *aux;
{

	switch (hwrpb->rpb_type) {
#if defined(DEC_3000_500) || defined(DEC_3000_300) || \
    defined(DEC_2000_300) || defined(DEC_2100_A50)
#if defined(DEC_3000_500)
	case ST_DEC_3000_500:
#endif
#if defined(DEC_2000_300)
	case ST_DEC_2000_300:
#endif
#if defined(DEC_3000_300)
	case ST_DEC_3000_300:
#endif
#if defined(DEC_2100_A50)
	case ST_DEC_2100_A50:
#endif
		mcclock_attach(parent, self, aux);
		break;
#endif /* defined(at least one of lots of system types) */

	default:
		panic("clockattach: it didn't get here.  really.");
	}


	/*
	 * establish the clock interrupt; it's a special case
	 */
	set_clockintr(hardclock);

	printf("\n");
}

/*
 * Machine-dependent clock routines.
 *
 * Startrtclock restarts the real-time clock, which provides
 * hardclock interrupts to kern_clock.c.
 *
 * Inittodr initializes the time of day hardware which provides
 * date functions.  Its primary function is to use some file
 * system information in case the hardare clock lost state.
 *
 * Resettodr restores the time of day hardware after a time change.
 */

/*
 * Start the real-time and statistics clocks. Leave stathz 0 since there
 * are no other timers available.
 */
cpu_initclocks()
{
	extern int tickadj;
	struct clock_softc *csc = (struct clock_softc *)clockcd.cd_devs[0];
	int fractick;

	hz = 1024;		/* 1024 Hz clock */
	tick = 1000000 / hz;	/* number of microseconds between interrupts */
	tickfix = 1000000 - (hz * tick);
	if (tickfix) {
		int ftp;

		ftp = min(ffs(tickfix), ffs(hz));
		tickfix >>= (ftp - 1);
		tickfixinterval = hz >> (ftp - 1);
        }

	/*
	 * Get the clock started.
	 */
	(*csc->sc_init)(csc);
}

/*
 * We assume newhz is either stathz or profhz, and that neither will
 * change after being set up above.  Could recalculate intervals here
 * but that would be a drag.
 */
void
setstatclockrate(newhz)
	int newhz;
{

	/* nothing we can do */
}

/*
 * This code is defunct after 2099.
 * Will Unix still be here then??
 */
static short dayyr[12] = {
	0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
};

/*
 * Initialze the time of day register, based on the time base which is, e.g.
 * from a filesystem.  Base provides the time to within six months,
 * and the time of year clock (if any) provides the rest.
 */
void
inittodr(base)
	time_t base;
{
	struct clock_softc *csc = (struct clock_softc *)clockcd.cd_devs[0];
	register int days, yr;
	struct clocktime ct;
	long deltat;
	int badbase, s;

	if (base < 5*SECYR) {
		printf("WARNING: preposterous time in file system");
		/* read the system clock anyway */
		base = 6*SECYR + 186*SECDAY + SECDAY/2;
		badbase = 1;
	} else
		badbase = 0;

	(*csc->sc_get)(csc, base, &ct);

	csc->sc_initted = 1;

	/* simple sanity checks */
	if (ct.year < 70 || ct.mon < 1 || ct.mon > 12 || ct.day < 1 ||
	    ct.day > 31 || ct.hour > 23 || ct.min > 59 || ct.sec > 59) {
		/*
		 * Believe the time in the file system for lack of
		 * anything better, resetting the TODR.
		 */
		time.tv_sec = base;
		if (!badbase) {
			printf("WARNING: preposterous clock chip time\n");
			resettodr();
		}
		goto bad;
	}
	days = 0;
	for (yr = 70; yr < ct.year; yr++)
		days += LEAPYEAR(yr) ? 366 : 365;
	days += dayyr[ct.mon - 1] + ct.day - 1;
	if (LEAPYEAR(yr) && ct.mon > 2)
		days++;
	/* now have days since Jan 1, 1970; the rest is easy... */
	time.tv_sec =
	    days * SECDAY + ct.hour * SECHOUR + ct.min * SECMIN + ct.sec;

	if (!badbase) {
		/*
		 * See if we gained/lost two or more days;
		 * if so, assume something is amiss.
		 */
		deltat = time.tv_sec - base;
		if (deltat < 0)
			deltat = -deltat;
		if (deltat < 2 * SECDAY)
			return;
		printf("WARNING: clock %s %d days",
		    time.tv_sec < base ? "lost" : "gained", deltat / SECDAY);
	}
bad:
	printf(" -- CHECK AND RESET THE DATE!\n");
}

/*
 * Reset the TODR based on the time value; used when the TODR
 * has a preposterous value and also when the time is reset
 * by the stime system call.  Also called when the TODR goes past
 * TODRZERO + 100*(SECYEAR+2*SECDAY) (e.g. on Jan 2 just after midnight)
 * to wrap the TODR around.
 */
void
resettodr()
{
	struct clock_softc *csc = (struct clock_softc *)clockcd.cd_devs[0];
	register int t, t2;
	struct clocktime ct;
	int s;

	if (!csc->sc_initted)
		return;

	/* compute the day of week. */
	t2 = time.tv_sec / SECDAY;
	ct.dow = (t2 + 4) % 7;	/* 1/1/1970 was thursday */

	/* compute the year */
	ct.year = 69;
	while (t2 >= 0) {	/* whittle off years */
		t = t2;
		ct.year++;
		t2 -= LEAPYEAR(ct.year) ? 366 : 365;
	}

	/* t = month + day; separate */
	t2 = LEAPYEAR(ct.year);
	for (ct.mon = 1; ct.mon < 12; ct.mon++)
		if (t < dayyr[ct.mon] + (t2 && ct.mon > 1))
			break;

	ct.day = t - dayyr[ct.mon - 1] + 1;
	if (t2 && ct.mon > 2)
		ct.day--;

	/* the rest is easy */
	t = time.tv_sec % SECDAY;
	ct.hour = t / SECHOUR;
	t %= 3600;
	ct.min = t / SECMIN;
	ct.sec = t % SECMIN;

	(*csc->sc_set)(csc, &ct);
}

/*
 * Wait "n" microseconds.  This doesn't belong here.  XXX.
 */
void
delay(n)
	int n;
{
	DELAY(n);
}
