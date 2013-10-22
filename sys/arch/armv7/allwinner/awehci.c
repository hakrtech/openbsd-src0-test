/*	$OpenBSD: awehci.c,v 1.1 2013/10/22 13:22:19 jasper Exp $ */

/*
 * Copyright (c) 2005 David Gwynne <dlg@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*-
 * Copyright (c) 2011
 *	Ben Gray <ben.r.gray@gmail.com>.
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
 * THIS SOFTWARE IS PROVIDED BY AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/device.h>
#include <sys/kernel.h>
#include <sys/rwlock.h>
#include <sys/timeout.h>

#include <machine/intr.h>
#include <machine/bus.h>

#include <dev/usb/usb.h>
#include <dev/usb/usbdi.h>
#include <dev/usb/usbdivar.h>
#include <dev/usb/usb_mem.h>

#include <armv7/allwinner/allwinnervar.h>
#include <armv7/allwinner/awccmuvar.h>
#include <armv7/allwinner/awpiovar.h>

#include <dev/usb/ehcireg.h>
#include <dev/usb/ehcivar.h>

#define EHCI_HC_DEVSTR		"Allwinner Integrated USB 2.0 controller"

#define USB_PMU_IRQ_ENABLE	0x800

#define SDRAM_REG_HPCR_USB1	(0x250 + ((1 << 2) * 4))
#define SDRAM_REG_HPCR_USB2	(0x250 + ((1 << 2) * 5))
#define SDRAM_BP_HPCR_ACCESS	(1 << 0)

#define ULPI_BYPASS		(1 << 0)
#define AHB_INCRX_ALIGN		(1 << 8)
#define AHB_INCR4		(1 << 9)
#define AHB_INCR8		(1 << 10)

void	awehci_attach(struct device *, struct device *, void *);
int	awehci_detach(struct device *, int);
int	awehci_activate(struct device *, int);

struct awehci_softc {
	struct ehci_softc	 sc;
	void			*sc_ih;
};

int awehci_init(struct awehci_softc *, int);

struct cfattach awehci_ca = {
	sizeof (struct awehci_softc), NULL, awehci_attach,
	awehci_detach, awehci_activate
};

/* XXX
 * given the nature of SoCs, i think this should just panic on failure,
 * instead of disestablishing interrupt and unmapping space etc..
 */
void
awehci_attach(struct device *parent, struct device *self, void *aux)
{
	struct awehci_softc	*sc = (struct awehci_softc *)self;
	struct aw_attach_args	*aw = aux;
	usbd_status		 r;
	char			*devname = sc->sc.sc_bus.bdev.dv_xname;

	sc->sc.iot = aw->aw_iot;
	sc->sc.sc_bus.dmatag = aw->aw_dmat;
	sc->sc.sc_size = aw->aw_dev->mem[0].size;

	if (bus_space_map(sc->sc.iot, aw->aw_dev->mem[0].addr,
		aw->aw_dev->mem[0].size, 0, &sc->sc.ioh)) {
		printf(": cannot map mem space\n");
		goto out;
	}

	printf("\n");

	if (awehci_init(sc, self->dv_unit))
		return;

	/* Disable interrupts, so we don't get any spurious ones. */
	sc->sc.sc_offs = EREAD1(&sc->sc, EHCI_CAPLENGTH);
	EOWRITE2(&sc->sc, EHCI_USBINTR, 0);

	sc->sc_ih = arm_intr_establish(aw->aw_dev->irq[0], IPL_USB,
	    ehci_intr, &sc->sc, devname);
	if (sc->sc_ih == NULL) {
		printf(": unable to establish interrupt\n");
		printf("XXX - disable ehci");
#if 0
		awccmu_disable(CCMU_EHCI+unit?);
#endif
		goto mem0;
	}

	strlcpy(sc->sc.sc_vendor, "Allwinner", sizeof(sc->sc.sc_vendor));
	r = ehci_init(&sc->sc);
	if (r != USBD_NORMAL_COMPLETION) {
		printf("%s: init failed, error=%d\n", devname, r);
		printf("XXX - disable ehci");
#if 0
		awccmu_disable(CCMU_EHCI+unit?);
#endif
		goto intr;
	}

	sc->sc.sc_child = config_found((void *)sc, &sc->sc.sc_bus,
	    usbctlprint);

	goto out;

intr:
	arm_intr_disestablish(sc->sc_ih);
	sc->sc_ih = NULL;
mem0:
	bus_space_unmap(sc->sc.iot, sc->sc.ioh, sc->sc.sc_size);
	sc->sc.sc_size = 0;
out:
	return;
}

int
awehci_init(struct awehci_softc *sc, int unit)
{
	uint32_t r, val;
	int pin, mod;

	if (unit > 1)
		panic("awehci_init: unit >1 %d", unit);
	else if (unit == 0) {
		pin = AWPIO_USB1_PWR;
		r = SDRAM_REG_HPCR_USB1;
		mod = CCMU_EHCI0;
	} else {
		pin = AWPIO_USB2_PWR;
		r = SDRAM_REG_HPCR_USB2;
		mod = CCMU_EHCI1;
	}

	awccmu_enablemodule(mod);

	/* power up */
	awpio_setcfg(pin, AWPIO_OUTPUT);
	awpio_setpin(pin);

	val = bus_space_read_4(sc->sc.iot, sc->sc.ioh, USB_PMU_IRQ_ENABLE);
	val |= AHB_INCR8;	/* AHB INCR8 enable */
	val |= AHB_INCR4;	/* AHB burst type INCR4 enable */
	val |= AHB_INCRX_ALIGN;	/* AHB INCRX align enable */
	val |= ULPI_BYPASS;	/* ULPI bypass enable */
	bus_space_write_4(sc->sc.iot, sc->sc.ioh, USB_PMU_IRQ_ENABLE, val);

	val = bus_space_read_4(sc->sc.iot, sc->sc.ioh, r);
	val |= SDRAM_BP_HPCR_ACCESS;
	bus_space_write_4(sc->sc.iot, sc->sc.ioh, r, val);

	return (0);
}

int
awehci_detach(struct device *self, int flags)
{
	struct awehci_softc *sc = (struct awehci_softc *)self;
	int rv;

	rv = ehci_detach(&sc->sc, flags);
	if (rv)
		return (rv);

	if (sc->sc_ih != NULL) {
		arm_intr_disestablish(sc->sc_ih);
		sc->sc_ih = NULL;
	}

	if (sc->sc.sc_size) {
		bus_space_unmap(sc->sc.iot, sc->sc.ioh, sc->sc.sc_size);
		sc->sc.sc_size = 0;
	}
	/* XXX */
#if 0
	awccmu_disable(CCMU_EHCI+unit?);
	awpio_clrpin(pin);
#endif
	return (0);
}

int
awehci_activate(struct device *self, int act)
{
	struct awehci_softc *sc = (struct awehci_softc *)self;

	switch (act) {
	case DVACT_SUSPEND:
		sc->sc.sc_bus.use_polling++;
		/* FIXME */
		sc->sc.sc_bus.use_polling--;
		break;
	case DVACT_RESUME:
		sc->sc.sc_bus.use_polling++;
		/* FIXME */
		sc->sc.sc_bus.use_polling--;
		break;
	case DVACT_POWERDOWN:
		ehci_reset(&sc->sc);
		break;
	}
	return 0;
}
