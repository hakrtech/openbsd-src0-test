/*	$OpenBSD: wscons_machdep.c,v 1.3 2005/12/09 14:39:26 jsg Exp $ */

/*
 * Copyright (c) 2001 Aaron Campbell
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR OR HIS RELATIVES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF MIND, USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/conf.h>
#include <sys/device.h>
#include <sys/extent.h>

#include <machine/autoconf.h>
#include <machine/bus.h>

#include <dev/cons.h>
#include <dev/ic/mc6845reg.h>
#include <dev/ic/pcdisplayvar.h>
#include <dev/ic/vgareg.h>
#include <dev/ic/vgavar.h>

#include "wsdisplay.h"
#if NWSDISPLAY > 0
#include <dev/wscons/wsdisplayvar.h>
#endif
#include <dev/wscons/wskbdvar.h>
#include <dev/wscons/wsconsio.h>
#include <dev/usb/ukbdvar.h>

cons_decl(ws);

void
wscnprobe(cp)
	struct consdev *cp;
{
	int maj;

	/* locate the major number */
	for (maj = 0; maj < nchrdev; maj++) {
		if (cdevsw[maj].d_open == wsdisplayopen)
			break;
	}

	if (maj == nchrdev) {
		/* we are not in cdevsw[], give up */
		panic("wsdisplay is not in cdevsw[]");
	}

	cp->cn_dev = makedev(maj, 0);
	cp->cn_pri = CN_INTERNAL;
	cp->cn_pri = CN_REMOTE;
}

void
wscninit(cp)
	struct consdev *cp;
{
static int initted;

	if (initted)
		return;

	initted = 1;

	if (!vga_cnattach(&sys_config.pci_io[0], &sys_config.pci_mem[0],
	    WSDISPLAY_TYPE_PCIVGA, 1)) {
		ukbd_cnattach();
	}
	return;
}

void
wscnputc(dev, i)
	dev_t dev;
	int i;
{
	wsdisplay_cnputc(dev, i);
}

int
wscngetc(dev)
	dev_t dev;
{
	int c;

	wskbd_cnpollc(dev, 1);
	c = wskbd_cngetc(dev);
	wskbd_cnpollc(dev, 0);

	return c;
}

void
wscnpollc(dev, on)
	dev_t dev;
	int on;
{
	wskbd_cnpollc(dev, on);
}
