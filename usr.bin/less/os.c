/*
 * Copyright (C) 1984-2011  Mark Nudelman
 *
 * You may distribute under the terms of either the GNU General Public
 * License or the Less License, as specified in the README file.
 *
 * For more information about less, or for information on how to 
 * contact the author, see the README file.
 */


/*
 * Operating system dependent routines.
 *
 * Most of the stuff in here is based on Unix, but an attempt
 * has been made to make things work on other operating systems.
 * This will sometimes result in a loss of functionality, unless
 * someone rewrites code specifically for the new operating system.
 *
 * The makefile provides defines to decide whether various
 * Unix features are present.
 */

#include "less.h"
#include <signal.h>
#if HAVE_TIME_H
#include <time.h>
#endif
#if HAVE_ERRNO_H
#include <errno.h>
#endif
#if HAVE_VALUES_H
#include <values.h>
#endif

#if HAVE_TIME_T
#define time_type	time_t
#else
#define	time_type	long
#endif

extern int sigs;

/*
 * Like read() system call, but is deliberately interruptible.
 */
	public int
iread(fd, buf, len)
	int fd;
	char *buf;
	unsigned int len;
{
	register int n;

start:
#if MSDOS_COMPILER==WIN32C
	if (ABORT_SIGS())
		return (READ_INTR);
#else
#if MSDOS_COMPILER && MSDOS_COMPILER != DJGPPC
	if (kbhit())
	{
		int c;
		
		c = getch();
		if (c == '\003')
			return (READ_INTR);
		ungetch(c);
	}
#endif
#endif

	flush();
#if MSDOS_COMPILER==DJGPPC
	if (isatty(fd))
	{
		/*
		 * Don't try reading from a TTY until a character is
		 * available, because that makes some background programs
		 * believe DOS is busy in a way that prevents those
		 * programs from working while "less" waits.
		 */
		fd_set readfds;

		FD_ZERO(&readfds);
		FD_SET(fd, &readfds);
		if (select(fd+1, &readfds, 0, 0, 0) == -1)
			return (-1);
	}
#endif
	n = read(fd, buf, len);
#if 1
	/*
	 * This is a kludge to workaround a problem on some systems
	 * where terminating a remote tty connection causes read() to
	 * start returning 0 forever, instead of -1.
	 */
	{
		extern int ignore_eoi;
		if (!ignore_eoi)
		{
			static int consecutive_nulls = 0;
			if (n == 0)
				consecutive_nulls++;
			else
				consecutive_nulls = 0;
			if (consecutive_nulls > 20)
				quit(QUIT_ERROR);
		}
	}
#endif
	if (n < 0)
	{
#if HAVE_ERRNO
		/*
		 * Certain values of errno indicate we should just retry the read.
		 */
#if MUST_DEFINE_ERRNO
		extern int errno;
#endif
#ifdef EINTR
		if (errno == EINTR)
			return (READ_INTR);
#endif
#ifdef EAGAIN
		if (errno == EAGAIN)
			goto start;
#endif
#endif
		return (-1);
	}
	return (n);
}

/*
 * Return the current time.
 */
#if HAVE_TIME
	public long
get_time()
{
	time_type t;

	time(&t);
	return (t);
}
#endif


#if !HAVE_STRERROR
/*
 * Local version of strerror, if not available from the system.
 */
	static char *
strerror(err)
	int err;
{
#if HAVE_SYS_ERRLIST
	static char buf[16];
	extern char *sys_errlist[];
	extern int sys_nerr;
  
	if (err < sys_nerr)
		return sys_errlist[err];
	snprintf(buf, sizeof(buf), "Error %d", err);
	return buf;
#else
	return ("cannot open");
#endif
}
#endif

/*
 * errno_message: Return an error message based on the value of "errno".
 */
	public char *
errno_message(filename)
	char *filename;
{
	register char *p;
	register char *m;
	size_t len;
#if HAVE_ERRNO
#if MUST_DEFINE_ERRNO
	extern int errno;
#endif
	p = strerror(errno);
#else
	p = "cannot open";
#endif
	len = strlen(filename) + strlen(p) + 3;
	m = (char *) ecalloc(len, sizeof(char));
	SNPRINTF2(m, len, "%s: %s", filename, p);
	return (m);
}

/* #define HAVE_FLOAT 0 */

	static POSITION
muldiv(val, num, den)
	POSITION val, num, den;
{
#if HAVE_FLOAT
	double v = (((double) val) * num) / den;
	return ((POSITION) (v + 0.5));
#else
	POSITION v = ((POSITION) val) * num;

	if (v / num == val)
		/* No overflow */
		return (POSITION) (v / den);
	else
		/* Above calculation overflows; 
		 * use a method that is less precise but won't overflow. */
		return (POSITION) (val / (den / num));
#endif
}

/*
 * Return the ratio of two POSITIONS, as a percentage.
 * {{ Assumes a POSITION is a long int. }}
 */
	public int
percentage(num, den)
	POSITION num, den;
{
	return (int) muldiv(num,  (POSITION) 100, den);
}

/*
 * Return the specified percentage of a POSITION.
 */
	public POSITION
percent_pos(pos, percent, fraction)
	POSITION pos;
	int percent;
	long fraction;
{
	/* Change percent (parts per 100) to perden (parts per NUM_FRAC_DENOM). */
	POSITION perden = (percent * (NUM_FRAC_DENOM / 100)) + (fraction / 100);

	if (perden == 0)
		return (0);
	return (POSITION) muldiv(pos, perden, (POSITION) NUM_FRAC_DENOM);
}

#if !HAVE_STRCHR
/*
 * strchr is used by regexp.c.
 */
	char *
strchr(s, c)
	char *s;
	int c;
{
	for ( ;  *s != '\0';  s++)
		if (*s == c)
			return (s);
	if (c == '\0')
		return (s);
	return (NULL);
}
#endif

#if !HAVE_MEMCPY
	VOID_POINTER
memcpy(dst, src, len)
	VOID_POINTER dst;
	VOID_POINTER src;
	int len;
{
	char *dstp = (char *) dst;
	char *srcp = (char *) src;
	int i;

	for (i = 0;  i < len;  i++)
		dstp[i] = srcp[i];
	return (dst);
}
#endif

#ifdef _OSK_MWC32

/*
 * This implements an ANSI-style intercept setup for Microware C 3.2
 */
	public int 
os9_signal(type, handler)
	int type;
	RETSIGTYPE (*handler)();
{
	intercept(handler);
}

#include <sgstat.h>

	int 
isatty(f)
	int f;
{
	struct sgbuf sgbuf;

	if (_gs_opt(f, &sgbuf) < 0)
		return -1;
	return (sgbuf.sg_class == 0);
}
	
#endif
