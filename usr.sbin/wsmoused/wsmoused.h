/* $OpenBSD: wsmoused.h,v 1.1 2001/04/14 04:47:41 aaron Exp $ */

/*
 * Copyright (c) 2001 Jean-Baptiste Marchand, Julien Montagne and Jerome Verdon
 * 
 * Copyright (c) 1998 by Kazutaka Yokota
 *
 * Copyright (c) 1995 Michael Smith
 * 
 * Copyright (c) 1993 by David Dawes <dawes@xfree86.org>
 *
 * Copyright (c) 1990,91 by Thomas Roell, Dinkelscherben, Germany.
 *
 * All rights reserved.
 *
 * Most of this code was taken from the FreeBSD moused daemon, written by
 * Michael Smith. The FreeBSD moused daemon already contained code from the 
 * Xfree Project, written by David Dawes and Thomas Roell and Kazutaka Yokota.
 *
 * Adaptation to OpenBSD was done by Jean-Baptiste Marchand, Julien Montagne
 * and Jerome Verdon.
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
 *	This product includes software developed by
 *      David Dawes, Jean-Baptiste Marchand, Julien Montagne, Thomas Roell,
 *      Michael Smith, Jerome Verdon and Kazutaka Yokota.
 * 4. The name authors may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 */

#define FALSE 0
#define TRUE 1

/* Logging macros */

#define debug(fmt,args...) \
	if (debug&&nodaemon) printf(fmt, ##args)

#define logerr(e, fmt, args...) {				\
	if (background) {					\
	    syslog(LOG_DAEMON | LOG_ERR, fmt, ##args);		\
	    exit(e);						\
	} else							\
	    errx(e, fmt, ##args);				\
}

#define logwarn(fmt, args...) {				\
	if (background)						\
	    syslog(LOG_DAEMON | LOG_WARNING, fmt, ##args);	\
	else							\
	    warnx(fmt, ##args);					\
}

/* Daemon flags */

#define	ChordMiddle	0x0001 /* avoid bug reporting middle button as down 
				  when left and right are pressed */
#define Emulate3Button	0x0002 /* option to emulate a third button */
#define ClearDTR	0x0004 /* for mousesystems protocol (3 button mouse) */
#define ClearRTS	0x0008 /* idem as above */
#define NoPnP		0x0010 /* disable PnP for PnP mice */

/* Devices corresponding to physical interfaces */

#define WSMOUSE_DEV "/dev/wsmouse" /* can be /dev/wsmouse, /dev/wsmouse0, ...*/
#define SERIAL_DEV "/dev/cua0" /* can be /dev/cua00, /dev/cua01, ... */

#define IS_WSMOUSE_DEV(dev) (!(strncmp((dev), WSMOUSE_DEV,12)))
#define IS_SERIAL_DEV(dev) (!(strncmp((dev), SERIAL_DEV, 9)))
 
/* mouse structure : main structure */
typedef struct mouse_s {
    int flags;
    char *portname;		/* /dev/XXX */
    int proto;			/* MOUSE_PROTO_XXX */
    int baudrate;
    int old_baudrate;
    unsigned char rate;			/* report rate */
    unsigned char resolution;		/* MOUSE_RES_XXX or a positive number */
    int zmap;			/* MOUSE_{X|Y}AXIS or a button number */
    int wmode;			/* wheel mode button number */
    int mfd;			/* mouse file descriptor */
    int cfd;			/* console file descriptor */
    long clickthreshold;	/* double click speed in msec */
} mouse_t ;

/* Mouse buttons */

#define MOUSE_BUTTON1	0	/* left */
#define MOUSE_BUTTON2	1	/* middle */
#define MOUSE_BUTTON3	2	/* right */
#define MOUSE_BUTTON4	3
#define MOUSE_BUTTON5	4
#define MOUSE_BUTTON6	5
#define MOUSE_BUTTON7	6
#define MOUSE_BUTTON8	7
#define MOUSE_MAXBUTTON	8	

