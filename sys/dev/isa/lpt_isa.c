/*	$OpenBSD: lpt_isa.c,v 1.2 1996/11/30 00:53:38 niklas Exp $	*/

/*
 * Copyright (c) 1993, 1994 Charles Hannum.
 * Copyright (c) 1990 William F. Jolitz, TeleMuse
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
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This software is a component of "386BSD" developed by 
 *	William F. Jolitz, TeleMuse.
 * 4. Neither the name of the developer nor the name "386BSD"
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS A COMPONENT OF 386BSD DEVELOPED BY WILLIAM F. JOLITZ 
 * AND IS INTENDED FOR RESEARCH AND EDUCATIONAL PURPOSES ONLY. THIS 
 * SOFTWARE SHOULD NOT BE CONSIDERED TO BE A COMMERCIAL PRODUCT. 
 * THE DEVELOPER URGES THAT USERS WHO REQUIRE A COMMERCIAL PRODUCT 
 * NOT MAKE USE OF THIS WORK.
 *
 * FOR USERS WHO WISH TO UNDERSTAND THE 386BSD SYSTEM DEVELOPED
 * BY WILLIAM F. JOLITZ, WE RECOMMEND THE USER STUDY WRITTEN 
 * REFERENCES SUCH AS THE  "PORTING UNIX TO THE 386" SERIES 
 * (BEGINNING JANUARY 1991 "DR. DOBBS JOURNAL", USA AND BEGINNING 
 * JUNE 1991 "UNIX MAGAZIN", GERMANY) BY WILLIAM F. JOLITZ AND 
 * LYNNE GREER JOLITZ, AS WELL AS OTHER BOOKS ON UNIX AND THE 
 * ON-LINE 386BSD USER MANUAL BEFORE USE. A BOOK DISCUSSING THE INTERNALS 
 * OF 386BSD ENTITLED "386BSD FROM THE INSIDE OUT" WILL BE AVAILABLE LATE 1992.
 *
 * THIS SOFTWARE IS PROVIDED BY THE DEVELOPER ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE DEVELOPER BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Device Driver for AT parallel printer port
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/device.h>

#include <machine/bus.h>
#include <machine/intr.h>

#include <dev/isa/isavar.h>
#include <dev/ic/lptreg.h>
#include <dev/ic/lptvar.h>

int	lpt_isa_probe __P((struct device *, void *, void *));
void	lpt_isa_attach __P((struct device *, struct device *, void *));

struct cfattach lpt_isa_ca = {
	sizeof(struct lpt_softc), lpt_isa_probe, lpt_isa_attach
};

/*
 * Logic:
 *	1) You should be able to write to and read back the same value
 *	   to the data port.  Do an alternating zeros, alternating ones,
 *	   walking zero, and walking one test to check for stuck bits.
 *
 *	2) You should be able to write to and read back the same value
 *	   to the control port lower 5 bits, the upper 3 bits are reserved
 *	   per the IBM PC technical reference manauls and different boards
 *	   do different things with them.  Do an alternating zeros, alternating
 *	   ones, walking zero, and walking one test to check for stuck bits.
 *
 *	   Some printers drag the strobe line down when the are powered off
 * 	   so this bit has been masked out of the control port test.
 *
 *	   XXX Some printers may not like a fast pulse on init or strobe, I
 *	   don't know at this point, if that becomes a problem these bits
 *	   should be turned off in the mask byte for the control port test.
 *
 *	3) Set the data and control ports to a value of 0
 */
int
lpt_isa_probe(parent, match, aux)
	struct device *parent;
	void *match, *aux;
{
	struct isa_attach_args *ia = aux;
	bus_space_tag_t iot;
	bus_space_handle_t ioh;
	bus_addr_t base;
	u_int8_t mask, data;
	int i, rv;

#ifdef DEBUG
#define	ABORT								     \
	do {								     \
		printf("lpt_isa_probe: mask %x data %x failed\n", mask,	     \
		    data);						     \
		goto out;						     \
	} while (0)
#else
#define	ABORT	goto out
#endif

	iot = ia->ia_iot;
	base = ia->ia_iobase;
	if (bus_space_map(iot, base, LPT_NPORTS, 0, &ioh))
		return 0;

	rv = 0;
	mask = 0xff;

	data = 0x55;				/* Alternating zeros */
	if (!lpt_port_test(iot, ioh, base, lpt_data, data, mask))
		ABORT;

	data = 0xaa;				/* Alternating ones */
	if (!lpt_port_test(iot, ioh, base, lpt_data, data, mask))
		ABORT;

	for (i = 0; i < CHAR_BIT; i++) {	/* Walking zero */
		data = ~(1 << i);
		if (!lpt_port_test(iot, ioh, base, lpt_data, data, mask))
			ABORT;
	}

	for (i = 0; i < CHAR_BIT; i++) {	/* Walking one */
		data = (1 << i);
		if (!lpt_port_test(iot, ioh, base, lpt_data, data, mask))
			ABORT;
	}

	bus_space_write_1(iot, ioh, lpt_data, 0);
	bus_space_write_1(iot, ioh, lpt_control, 0);

	ia->ia_iosize = LPT_NPORTS;
	ia->ia_msize = 0;

	rv = 1;

out:
	bus_space_unmap(iot, ioh, LPT_NPORTS);
	return rv;
}

void
lpt_isa_attach(parent, self, aux)
	struct device *parent, *self;
	void *aux;
{
	struct lpt_softc *sc = (void *)self;
	struct isa_attach_args *ia = aux;
	bus_space_tag_t iot;
	bus_space_handle_t ioh;

	if (ia->ia_irq != IRQUNK)
		printf("\n");
	else {
		sc->sc_flags |= LPT_POLLED;
		printf(": polled\n");
	}

	sc->sc_state = 0;

	iot = sc->sc_iot = ia->ia_iot;
	if (bus_space_map(iot, ia->ia_iobase, LPT_NPORTS, 0, &ioh))
		panic("lpt_isa_attach: couldn't map I/O ports");
	sc->sc_ioh = ioh;

	bus_space_write_1(iot, ioh, lpt_control, LPC_NINIT);

	if (ia->ia_irq != IRQUNK)
		sc->sc_ih = isa_intr_establish(ia->ia_ic, ia->ia_irq, IST_EDGE,
		    IPL_TTY, lptintr, sc, sc->sc_dev.dv_xname);
}
