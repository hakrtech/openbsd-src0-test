/*	$OpenBSD: conf.h,v 1.1 2008/11/26 14:47:50 drahn Exp $	*/
/*	$NetBSD: conf.h,v 1.8 2002/02/10 12:26:03 chris Exp $	*/

#ifndef _MOKO_CONF_H
#define	_MOKO_CONF_H

#include <sys/conf.h>

/*
 * MOKO specific device includes go in here
 */

//#define CONF_HAVE_APM
#define	CONF_HAVE_USB
#define	CONF_HAVE_WSCONS

#include <arm/conf.h>

#endif	/* _MOKO_CONF_H */
