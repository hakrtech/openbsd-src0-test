/*	$OpenBSD: hifn7751.c,v 1.75 2001/06/23 20:59:41 angelos Exp $	*/

/*
 * Invertex AEON / Hi/fn 7751 driver
 * Copyright (c) 1999 Invertex Inc. All rights reserved.
 * Copyright (c) 1999 Theo de Raadt
 * Copyright (c) 2000-2001 Network Security Technologies, Inc.
 *			http://www.netsec.net
 *
 * This driver is based on a previous driver by Invertex, for which they
 * requested:  Please send any comments, feedback, bug-fixes, or feature
 * requests to software@invertex.com.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Driver for the Hi/Fn 7751 encryption processor.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <sys/errno.h>
#include <sys/malloc.h>
#include <sys/kernel.h>
#include <sys/mbuf.h>
#include <vm/vm.h>
#include <vm/vm_extern.h>
#include <vm/pmap.h>
#include <machine/pmap.h>
#include <sys/device.h>

#include <crypto/cryptodev.h>
#include <dev/rndvar.h>

#include <dev/pci/pcireg.h>
#include <dev/pci/pcivar.h>
#include <dev/pci/pcidevs.h>

#include <dev/pci/hifn7751reg.h>
#include <dev/pci/hifn7751var.h>

#undef HIFN_DEBUG

/*
 * Prototypes and count for the pci_device structure
 */
int hifn_probe		__P((struct device *, void *, void *));
void hifn_attach	__P((struct device *, struct device *, void *));

struct cfattach hifn_ca = {
	sizeof(struct hifn_softc), hifn_probe, hifn_attach,
};

struct cfdriver hifn_cd = {
	0, "hifn", DV_DULL
};

void	hifn_reset_board __P((struct hifn_softc *));
int	hifn_enable_crypto __P((struct hifn_softc *, pcireg_t));
void	hifn_init_dma __P((struct hifn_softc *));
void	hifn_init_pci_registers __P((struct hifn_softc *));
int	hifn_sramsize __P((struct hifn_softc *));
int	hifn_dramsize __P((struct hifn_softc *));
void	hifn_ramtype __P((struct hifn_softc *));
void	hifn_sessions __P((struct hifn_softc *));
int	hifn_intr __P((void *));
u_int	hifn_write_command __P((struct hifn_command *, u_int8_t *));
u_int32_t hifn_next_signature __P((u_int32_t a, u_int cnt));
int	hifn_newsession __P((u_int32_t *, struct cryptoini *));
int	hifn_freesession __P((u_int64_t));
int	hifn_process __P((struct cryptop *));
void	hifn_callback __P((struct hifn_softc *, struct hifn_command *, u_int8_t *));
int	hifn_crypto __P((struct hifn_softc *, hifn_command_t *, struct cryptop *));
int	hifn_readramaddr __P((struct hifn_softc *, int, u_int8_t *, int));
int	hifn_writeramaddr __P((struct hifn_softc *, int, u_int8_t *, int));
int	hifn_dmamap_aligned __P((bus_dmamap_t));
void	hifn_dmamap_load __P((bus_dmamap_t, int *, struct hifn_desc *, int,
    volatile int *));
int	hifn_init_pubrng __P((struct hifn_softc *));
void	hifn_rng __P((void *));

struct hifn_stats {
	u_int64_t hst_ibytes;
	u_int64_t hst_obytes;
	u_int32_t hst_ipackets;
	u_int32_t hst_opackets;
	u_int32_t hst_invalid;
	u_int32_t hst_nomem;
} hifnstats;

int
hifn_probe(parent, match, aux)
	struct device *parent;
	void *match;
	void *aux;
{
	struct pci_attach_args *pa = (struct pci_attach_args *) aux;

	if (PCI_VENDOR(pa->pa_id) == PCI_VENDOR_INVERTEX &&
	    PCI_PRODUCT(pa->pa_id) == PCI_PRODUCT_INVERTEX_AEON)
		return (1);
	if (PCI_VENDOR(pa->pa_id) == PCI_VENDOR_HIFN &&
	    PCI_PRODUCT(pa->pa_id) == PCI_PRODUCT_HIFN_7751)
		return (1);
	if (PCI_VENDOR(pa->pa_id) == PCI_VENDOR_HIFN &&
	    PCI_PRODUCT(pa->pa_id) == PCI_PRODUCT_HIFN_7951)
		return (1);
	if (PCI_VENDOR(pa->pa_id) == PCI_VENDOR_NETSEC &&
	    PCI_PRODUCT(pa->pa_id) == PCI_PRODUCT_NETSEC_7751)
		return (1);
	return (0);
}

void 
hifn_attach(parent, self, aux)
	struct device *parent, *self;
	void *aux;
{
	struct hifn_softc *sc = (struct hifn_softc *)self;
	struct pci_attach_args *pa = aux;
	pci_chipset_tag_t pc = pa->pa_pc;
	pci_intr_handle_t ih;
	const char *intrstr = NULL;
	char rbase;
	bus_size_t iosize0, iosize1;
	u_int32_t cmd;
	u_int16_t ena;
	int rseg;
	caddr_t kva;

	cmd = pci_conf_read(pc, pa->pa_tag, PCI_COMMAND_STATUS_REG);
	cmd |= PCI_COMMAND_MEM_ENABLE | PCI_COMMAND_MASTER_ENABLE;
	pci_conf_write(pc, pa->pa_tag, PCI_COMMAND_STATUS_REG, cmd);
	cmd = pci_conf_read(pc, pa->pa_tag, PCI_COMMAND_STATUS_REG);

	if (!(cmd & PCI_COMMAND_MEM_ENABLE)) {
		printf(": failed to enable memory mapping\n");
		return;
	}

	if (!(cmd & PCI_COMMAND_MASTER_ENABLE)) {
		printf(": failed to enable bus mastering\n");
		return;
	}

	if (pci_mapreg_map(pa, HIFN_BAR0, PCI_MAPREG_TYPE_MEM, 0,
	    &sc->sc_st0, &sc->sc_sh0, NULL, &iosize0, 0)) {
		printf(": can't find mem space %d\n", 0);
		return;
	}

	if (pci_mapreg_map(pa, HIFN_BAR1, PCI_MAPREG_TYPE_MEM, 0,
	    &sc->sc_st1, &sc->sc_sh1, NULL, &iosize1, 0)) {
		printf(": can't find mem space %d\n", 1);
		goto fail_io0;
	}

	sc->sc_dmat = pa->pa_dmat;
	if (bus_dmamap_create(sc->sc_dmat, sizeof(*sc->sc_dma), 1,
	    sizeof(*sc->sc_dma), 0, BUS_DMA_NOWAIT, &sc->sc_dmamap)) {
		printf(": can't create dma map\n");
		goto fail_io1;
	}
	if (bus_dmamem_alloc(sc->sc_dmat, sizeof(*sc->sc_dma), PAGE_SIZE, 0,
	    sc->sc_dmamap->dm_segs, 1, &sc->sc_dmamap->dm_nsegs,
	    BUS_DMA_NOWAIT)) {
		printf(": can't alloc dma buffer\n");
		bus_dmamap_destroy(sc->sc_dmat, sc->sc_dmamap);
		goto fail_io1;
	}
	if (bus_dmamem_map(sc->sc_dmat, sc->sc_dmamap->dm_segs,
	    sc->sc_dmamap->dm_nsegs, sizeof(*sc->sc_dma), &kva,
	    BUS_DMA_NOWAIT)) {
		printf(": can't map dma buffers (%lu bytes)\n",
		    (u_long)sizeof(*sc->sc_dma));
		bus_dmamem_free(sc->sc_dmat, sc->sc_dmamap->dm_segs,
		    sc->sc_dmamap->dm_nsegs);
		bus_dmamap_destroy(sc->sc_dmat, sc->sc_dmamap);
		goto fail_io1;
	}
	if (bus_dmamap_load(sc->sc_dmat, sc->sc_dmamap, kva,
	    sizeof(*sc->sc_dma), NULL, BUS_DMA_NOWAIT)) {
		printf(": can't load dma map\n");
		bus_dmamem_unmap(sc->sc_dmat, kva, sizeof(*sc->sc_dma));
		bus_dmamem_free(sc->sc_dmat, sc->sc_dmamap->dm_segs,
		    sc->sc_dmamap->dm_nsegs);
		bus_dmamap_destroy(sc->sc_dmat, sc->sc_dmamap);
		goto fail_io1;
	}
	sc->sc_dma = (struct hifn_dma *)kva;
	bzero(sc->sc_dma, sizeof(*sc->sc_dma));

	hifn_reset_board(sc);

	if (hifn_enable_crypto(sc, pa->pa_id) != 0) {
		printf("%s: crypto enabling failed\n", sc->sc_dv.dv_xname);
		goto fail_mem;
	}

	if (PCI_VENDOR(pa->pa_id) == PCI_VENDOR_HIFN &&
	    PCI_PRODUCT(pa->pa_id) == PCI_PRODUCT_HIFN_7951)
		sc->sc_flags = HIFN_HAS_RNG | HIFN_HAS_PUBLIC;

	hifn_init_dma(sc);
	hifn_init_pci_registers(sc);

	hifn_ramtype(sc);

	if (sc->sc_drammodel == 0)
		hifn_sramsize(sc);
	else
		hifn_dramsize(sc);

	/*
	 * Workaround for NetSec 7751 rev A: half ram size because two
	 * of the address lines were left floating
	 */
	if (PCI_VENDOR(pa->pa_id) == PCI_VENDOR_NETSEC &&
	    PCI_PRODUCT(pa->pa_id) == PCI_PRODUCT_NETSEC_7751 &&
	    PCI_REVISION(pa->pa_class) == 0x61)
		sc->sc_ramsize >>= 1;

	/*
	 * Reinitialize again, since the DRAM/SRAM detection shifted our ring
	 * pointers and may have changed the value we send to the RAM Config
	 * Register.
	 */
	hifn_reset_board(sc);
	hifn_init_dma(sc);
	hifn_init_pci_registers(sc);

	if (pci_intr_map(pc, pa->pa_intrtag, pa->pa_intrpin,
	    pa->pa_intrline, &ih)) {
		printf(": couldn't map interrupt\n");
		goto fail_mem;
	}
	intrstr = pci_intr_string(pc, ih);
	sc->sc_ih = pci_intr_establish(pc, ih, IPL_NET, hifn_intr, sc,
	    self->dv_xname);
	if (sc->sc_ih == NULL) {
		printf(": couldn't establish interrupt");
		if (intrstr != NULL)
			printf(" at %s", intrstr);
		printf("\n");
		goto fail_mem;
	}

	hifn_sessions(sc);

	rseg = sc->sc_ramsize / 1024;
	rbase = 'K';
	if (sc->sc_ramsize >= (1024 * 1024)) {
		rbase = 'M';
		rseg /= 1024;
	}
	printf(", %d%cB %cram, %s\n", rseg, rbase,
	    sc->sc_drammodel ? 'd' : 's', intrstr);

	sc->sc_cid = crypto_get_driverid();
	if (sc->sc_cid < 0)
		goto fail_intr;

	WRITE_REG_0(sc, HIFN_0_PUCNFG,
	    READ_REG_0(sc, HIFN_0_PUCNFG) | HIFN_PUCNFG_CHIPID);
	ena = READ_REG_0(sc, HIFN_0_PUSTAT) & HIFN_PUSTAT_CHIPENA;

	switch (ena) {
	case HIFN_PUSTAT_ENA_2:
		crypto_register(sc->sc_cid, CRYPTO_3DES_CBC, 0, 0,
		    hifn_newsession, hifn_freesession, hifn_process);
		/*FALLTHROUGH*/
	case HIFN_PUSTAT_ENA_1:
		crypto_register(sc->sc_cid, CRYPTO_MD5_HMAC, 0, 0,
		    hifn_newsession, hifn_freesession, hifn_process);
		crypto_register(sc->sc_cid, CRYPTO_SHA1_HMAC, 0, 0,
		    NULL, NULL, NULL);
		crypto_register(sc->sc_cid, CRYPTO_DES_CBC, 0, 0,
		    NULL, NULL, NULL);
	}

	bus_dmamap_sync(sc->sc_dmat, sc->sc_dmamap,
	    BUS_DMASYNC_PREREAD | BUS_DMASYNC_PREWRITE);

	if (sc->sc_flags & (HIFN_HAS_PUBLIC | HIFN_HAS_RNG))
		hifn_init_pubrng(sc);

	return;

fail_intr:
	pci_intr_disestablish(pc, sc->sc_ih);
fail_mem:
	bus_dmamap_unload(sc->sc_dmat, sc->sc_dmamap);
	bus_dmamem_unmap(sc->sc_dmat, kva, sizeof(*sc->sc_dma));
	bus_dmamem_free(sc->sc_dmat, sc->sc_dmamap->dm_segs,
	    sc->sc_dmamap->dm_nsegs);
	bus_dmamap_destroy(sc->sc_dmat, sc->sc_dmamap);
fail_io1:
	bus_space_unmap(sc->sc_st1, sc->sc_sh1, iosize1);
fail_io0:
	bus_space_unmap(sc->sc_st0, sc->sc_sh0, iosize0);
}

int
hifn_init_pubrng(sc)
	struct hifn_softc *sc;
{
	int i;

	WRITE_REG_1(sc, HIFN_1_PUB_RESET,
	    READ_REG_1(sc, HIFN_1_PUB_RESET) | HIFN_PUBRST_RESET);

	for (i = 0; i < 100; i++) {
		DELAY(1000);
		if ((READ_REG_1(sc, HIFN_1_PUB_RESET) & HIFN_PUBRST_RESET)
		    == 0)
			break;
	}
	if (i == 100) {
		printf("%s: public key init failed\n", sc->sc_dv.dv_xname);
		return (1);
	}

	/* Enable the rng, if available */
	if (sc->sc_flags & HIFN_HAS_RNG) {
		WRITE_REG_1(sc, HIFN_1_RNG_CONFIG,
		    READ_REG_1(sc, HIFN_1_RNG_CONFIG) | HIFN_RNGCFG_ENA);
		sc->sc_rngfirst = 1;
		if (hz >= 100)
			sc->sc_rnghz = hz / 100;
		else
			sc->sc_rnghz = 1;
		timeout_set(&sc->sc_rngto, hifn_rng, sc);
		timeout_add(&sc->sc_rngto, sc->sc_rnghz);
	}

	/* Enable public key engine, if available */
	if (sc->sc_flags & HIFN_HAS_PUBLIC) {
		WRITE_REG_1(sc, HIFN_1_PUB_STATUS,
		    READ_REG_1(sc, HIFN_1_PUB_STATUS) | HIFN_PUBSTS_DONE);

		sc->sc_dmaier |= HIFN_DMAIER_PUBDONE;
		WRITE_REG_1(sc, HIFN_1_DMA_IER, sc->sc_dmaier);
	}

	return (0);
}

void
hifn_rng(vsc)
	void *vsc;
{
	struct hifn_softc *sc = vsc;
	u_int32_t num;

	num = READ_REG_1(sc, HIFN_1_RNG_DATA);

	if (sc->sc_rngfirst)
		sc->sc_rngfirst = 0;
	else
		add_true_randomness(num);

	timeout_add(&sc->sc_rngto, sc->sc_rnghz);
}

/*
 * Resets the board.  Values in the regesters are left as is
 * from the reset (i.e. initial values are assigned elsewhere).
 */
void
hifn_reset_board(sc)
	struct hifn_softc *sc;
{
	/*
	 * Set polling in the DMA configuration register to zero.  0x7 avoids
	 * resetting the board and zeros out the other fields.
	 */
	WRITE_REG_1(sc, HIFN_1_DMA_CNFG, HIFN_DMACNFG_MSTRESET |
	    HIFN_DMACNFG_DMARESET | HIFN_DMACNFG_MODE);

	/*
	 * Now that polling has been disabled, we have to wait 1 ms
	 * before resetting the board.
	 */
	DELAY(1000);

	/* Reset the board.  We do this by writing zeros to the DMA reset
	 * field, the BRD reset field, and the manditory 1 at position 2.
	 * Every other field is set to zero.
	 */
	WRITE_REG_1(sc, HIFN_1_DMA_CNFG, HIFN_DMACNFG_MODE);

	/*
	 * Wait another millisecond for the board to reset.
	 */
	DELAY(1000);

	/*
	 * Turn off the reset!  (No joke.)
	 */
	WRITE_REG_1(sc, HIFN_1_DMA_CNFG, HIFN_DMACNFG_MSTRESET |
	    HIFN_DMACNFG_DMARESET | HIFN_DMACNFG_MODE);

	/*
	 * Wait another millisecond for the board to un-reset.
	 */
	DELAY(1000);
}

u_int32_t
hifn_next_signature(a, cnt)
	u_int32_t a;
	u_int cnt;
{
	int i;
	u_int32_t v;

	for (i = 0; i < cnt; i++) {

		/* get the parity */
		v = a & 0x80080125;
		v ^= v >> 16;
		v ^= v >> 8;
		v ^= v >> 4;
		v ^= v >> 2;
		v ^= v >> 1;

		a = (v & 1) ^ (a << 1);
	}

	return a;
}

struct pci2id {
	u_short		pci_vendor;
	u_short		pci_prod;
	char		card_id[13];
} pci2id[] = {
	{
		PCI_VENDOR_HIFN,
		PCI_PRODUCT_HIFN_7951,
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		  0x00, 0x00, 0x00, 0x00, 0x00 }
	}, {
		PCI_VENDOR_NETSEC,
		PCI_PRODUCT_NETSEC_7751,
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		  0x00, 0x00, 0x00, 0x00, 0x00 }
	}, {
		PCI_VENDOR_INVERTEX,
		PCI_PRODUCT_INVERTEX_AEON,
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		  0x00, 0x00, 0x00, 0x00, 0x00 }
	}, {
		/*
		 * Other vendors share this PCI ID as well, such as
		 * http://www.powercrypt.com, and obviously they also
		 * use the same key.
		 */
		PCI_VENDOR_HIFN,
		PCI_PRODUCT_HIFN_7751,
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		  0x00, 0x00, 0x00, 0x00, 0x00 }
	},
};

/*
 * Checks to see if crypto is already enabled.  If crypto isn't enable,
 * "hifn_enable_crypto" is called to enable it.  The check is important,
 * as enabling crypto twice will lock the board.
 */
int 
hifn_enable_crypto(sc, pciid)
	struct hifn_softc *sc;
	pcireg_t pciid;
{
	u_int32_t dmacfg, ramcfg, encl, addr, i;
	char *offtbl = NULL;

	for (i = 0; i < sizeof(pci2id)/sizeof(pci2id[0]); i++) {
		if (pci2id[i].pci_vendor == PCI_VENDOR(pciid) &&
		    pci2id[i].pci_prod == PCI_PRODUCT(pciid)) {
			offtbl = pci2id[i].card_id;
			break;
		}
	}

	if (offtbl == NULL) {
#ifdef HIFN_DEBUG
		printf(": Unknown card!\n");
#endif
		return (1);
	}

	ramcfg = READ_REG_0(sc, HIFN_0_PUCNFG);
	dmacfg = READ_REG_1(sc, HIFN_1_DMA_CNFG);

	/*
	 * The RAM config register's encrypt level bit needs to be set before
	 * every read performed on the encryption level register.
	 */
	WRITE_REG_0(sc, HIFN_0_PUCNFG, ramcfg | HIFN_PUCNFG_CHIPID);

	encl = READ_REG_0(sc, HIFN_0_PUSTAT) & HIFN_PUSTAT_CHIPENA;

	/*
	 * Make sure we don't re-unlock.  Two unlocks kills chip until the
	 * next reboot.
	 */
	if (encl == HIFN_PUSTAT_ENA_1 || encl == HIFN_PUSTAT_ENA_2) {
#ifdef HIFN_DEBUG
		printf(": Strong Crypto already enabled!\n");
#endif
		goto report;
	}

	if (encl != 0 && encl != HIFN_PUSTAT_ENA_0) {
#ifdef HIFN_DEBUG
		printf(": Unknown encryption level\n");
#endif
		return 1;
	}

	WRITE_REG_1(sc, HIFN_1_DMA_CNFG, HIFN_DMACNFG_UNLOCK |
	    HIFN_DMACNFG_MSTRESET | HIFN_DMACNFG_DMARESET | HIFN_DMACNFG_MODE);
	DELAY(1000);
	addr = READ_REG_1(sc, HIFN_UNLOCK_SECRET1);
	DELAY(1000);
	WRITE_REG_1(sc, HIFN_UNLOCK_SECRET2, 0);
	DELAY(1000);

	for (i = 0; i <= 12; i++) {
		addr = hifn_next_signature(addr, offtbl[i] + 0x101);
		WRITE_REG_1(sc, HIFN_UNLOCK_SECRET2, addr);

		DELAY(1000);
	}

	WRITE_REG_0(sc, HIFN_0_PUCNFG, ramcfg | HIFN_PUCNFG_CHIPID);
	encl = READ_REG_0(sc, HIFN_0_PUSTAT) & HIFN_PUSTAT_CHIPENA;

#ifdef HIFN_DEBUG
	if (encl != HIFN_PUSTAT_ENA_1 && encl != HIFN_PUSTAT_ENA_2)
		printf(": engine is permanently locked until next system reset");
	else
		printf(": engine enabled successfully!");
#endif

report:
	WRITE_REG_0(sc, HIFN_0_PUCNFG, ramcfg);
	WRITE_REG_1(sc, HIFN_1_DMA_CNFG, dmacfg);

	switch (encl) {
	case HIFN_PUSTAT_ENA_0:
		printf(": no encr/auth");
		break;
	case HIFN_PUSTAT_ENA_1:
		printf(": DES");
		break;
	case HIFN_PUSTAT_ENA_2:
		printf(": 3DES");
		break;
	default:
		printf(": disabled");
		break;
	}

	return 0;
}

/*
 * Give initial values to the registers listed in the "Register Space"
 * section of the HIFN Software Development reference manual.
 */
void 
hifn_init_pci_registers(sc)
	struct hifn_softc *sc;
{
	/* write fixed values needed by the Initialization registers */
	WRITE_REG_0(sc, HIFN_0_PUCTRL, HIFN_PUCTRL_DMAENA);
	WRITE_REG_0(sc, HIFN_0_FIFOCNFG, HIFN_FIFOCNFG_THRESHOLD);
	WRITE_REG_0(sc, HIFN_0_PUIER, HIFN_PUIER_DSTOVER);

	/* write all 4 ring address registers */
	WRITE_REG_1(sc, HIFN_1_DMA_CRAR, sc->sc_dmamap->dm_segs[0].ds_addr +
	    offsetof(struct hifn_dma, cmdr[0]));
	WRITE_REG_1(sc, HIFN_1_DMA_SRAR, sc->sc_dmamap->dm_segs[0].ds_addr +
	    offsetof(struct hifn_dma, srcr[0]));
	WRITE_REG_1(sc, HIFN_1_DMA_DRAR, sc->sc_dmamap->dm_segs[0].ds_addr +
	    offsetof(struct hifn_dma, dstr[0]));
	WRITE_REG_1(sc, HIFN_1_DMA_RRAR, sc->sc_dmamap->dm_segs[0].ds_addr +
	    offsetof(struct hifn_dma, resr[0]));

	/* write status register */
	WRITE_REG_1(sc, HIFN_1_DMA_CSR, HIFN_DMACSR_D_CTRL_ENA |
	    HIFN_DMACSR_R_CTRL_ENA | HIFN_DMACSR_S_CTRL_ENA |
	    HIFN_DMACSR_C_CTRL_ENA);
	sc->sc_dmaier |= HIFN_DMAIER_R_DONE;
	WRITE_REG_1(sc, HIFN_1_DMA_IER, sc->sc_dmaier);

	WRITE_REG_0(sc, HIFN_0_PUCNFG, HIFN_PUCNFG_COMPSING |
	    HIFN_PUCNFG_DRFR_128 | HIFN_PUCNFG_TCALLPHASES |
	    HIFN_PUCNFG_TCDRVTOTEM | HIFN_PUCNFG_BUS32 |
	    (sc->sc_drammodel ? HIFN_PUCNFG_DRAM : HIFN_PUCNFG_SRAM));

	WRITE_REG_0(sc, HIFN_0_PUISR, HIFN_PUISR_DSTOVER);
	WRITE_REG_1(sc, HIFN_1_DMA_CNFG, HIFN_DMACNFG_MSTRESET |
	    HIFN_DMACNFG_DMARESET | HIFN_DMACNFG_MODE | HIFN_DMACNFG_LAST |
	    ((HIFN_POLL_FREQUENCY << 16 ) & HIFN_DMACNFG_POLLFREQ) |
	    ((HIFN_POLL_SCALAR << 8) & HIFN_DMACNFG_POLLINVAL));
}

/*
 * The maximum number of sessions supported by the card
 * is dependent on the amount of context ram, which
 * encryption algorithms are enabled, and how compression
 * is configured.  This should be configured before this
 * routine is called.
 */
void
hifn_sessions(sc)
	struct hifn_softc *sc;
{
	u_int32_t pucnfg;
	int ctxsize;

	pucnfg = READ_REG_0(sc, HIFN_0_PUCNFG);

	if (pucnfg & HIFN_PUCNFG_COMPSING) {
		if (pucnfg & HIFN_PUCNFG_ENCCNFG)
			ctxsize = 128;
		else
			ctxsize = 512;
		sc->sc_maxses = 1 +
		    ((sc->sc_ramsize - 32768) / ctxsize);
	}
	else
		sc->sc_maxses = sc->sc_ramsize / 16384;

	if (sc->sc_maxses > 2048)
		sc->sc_maxses = 2048;
}

void
hifn_ramtype(sc)
	struct hifn_softc *sc;
{
	u_int8_t data[8], dataexpect[8];
	int i;

	hifn_reset_board(sc);
	hifn_init_dma(sc);
	hifn_init_pci_registers(sc);

	for (i = 0; i < sizeof(data); i++)
		data[i] = dataexpect[i] = 0x55;
	if (hifn_writeramaddr(sc, 0, data, 0) < 0)
		return;
	if (hifn_readramaddr(sc, 0, data, 1) < 0)
		return;
	if (bcmp(data, dataexpect, sizeof(data)) != 0) {
		sc->sc_drammodel = 1;
		return;
	}

	hifn_reset_board(sc);
	hifn_init_dma(sc);
	hifn_init_pci_registers(sc);

	for (i = 0; i < sizeof(data); i++)
		data[i] = dataexpect[i] = 0xaa;
	if (hifn_writeramaddr(sc, 0, data, 0) < 0)
		return;
	if (hifn_readramaddr(sc, 0, data, 1) < 0)
		return;
	if (bcmp(data, dataexpect, sizeof(data)) != 0)
		sc->sc_drammodel = 1;
}

/*
 * For sram boards, just write/read memory until it fails, also check for
 * banking.
 */
int
hifn_sramsize(sc)
	struct hifn_softc *sc;
{
	u_int32_t a = 0, end;
	u_int8_t data[8], dataexpect[8];

	for (a = 0; a < sizeof(data); a++)
		data[a] = dataexpect[a] = 0x5a;

	hifn_reset_board(sc);
	hifn_init_dma(sc);
	hifn_init_pci_registers(sc);
	end = 1 << 20;	/* 1MB */
	for (a = 0; a < end; a += 16384) {
		if (hifn_writeramaddr(sc, a, data, 0) < 0)
			return (0);
		if (hifn_readramaddr(sc, a, data, 1) < 0)
			return (0);
		if (bcmp(data, dataexpect, sizeof(data)) != 0)
			return (0);
		hifn_reset_board(sc);
		hifn_init_dma(sc);
		hifn_init_pci_registers(sc);
		sc->sc_ramsize = a + 16384;
	}

	for (a = 0; a < sizeof(data); a++)
		data[a] = dataexpect[a] = 0xa5;
	if (hifn_writeramaddr(sc, 0, data, 0) < 0)
		return (0);

	end = sc->sc_ramsize;
	for (a = 0; a < end; a += 16384) {
		hifn_reset_board(sc);
		hifn_init_dma(sc);
		hifn_init_pci_registers(sc);
		if (hifn_readramaddr(sc, a, data, 0) < 0)
			return (0);
		if (a != 0 && bcmp(data, dataexpect, sizeof(data)) == 0)
			return (0);
		sc->sc_ramsize = a + 16384;
	}

	hifn_reset_board(sc);
	hifn_init_dma(sc);
	hifn_init_pci_registers(sc);

	return (0);
}

/*
 * XXX For dram boards, one should really try all of the
 * HIFN_PUCNFG_DSZ_*'s.  This just assumes that PUCNFG
 * is already set up correctly.
 */
int
hifn_dramsize(sc)
	struct hifn_softc *sc;
{
	u_int32_t cnfg;

	cnfg = READ_REG_0(sc, HIFN_0_PUCNFG) &
	    HIFN_PUCNFG_DRAMMASK;
	sc->sc_ramsize = 1 << ((cnfg >> 13) + 18);
	return (0);
}

int
hifn_writeramaddr(sc, addr, data, slot)
	struct hifn_softc *sc;
	int addr, slot;
	u_int8_t *data;
{
	struct hifn_dma *dma = sc->sc_dma;
	hifn_base_command_t wc;
	const u_int32_t masks = HIFN_D_VALID | HIFN_D_LAST | HIFN_D_MASKDONEIRQ;

	wc.masks = 3 << 13;
	wc.session_num = addr >> 14;
	wc.total_source_count = 8;
	wc.total_dest_count = addr & 0x3fff;;

	/* build write command */
	*(hifn_base_command_t *)dma->command_bufs[slot] = wc;
	bcopy(data, &dma->test_src, sizeof(dma->test_src));

	dma->srcr[slot].p = sc->sc_dmamap->dm_segs[0].ds_addr
	    + offsetof(struct hifn_dma, test_src);
	dma->dstr[slot].p = sc->sc_dmamap->dm_segs[0].ds_addr
	    + offsetof(struct hifn_dma, test_dst);

	dma->cmdr[slot].l = 16 | masks;
	dma->srcr[slot].l = 8 | masks;
	dma->dstr[slot].l = 8 | masks;
	dma->resr[slot].l = HIFN_MAX_RESULT | masks;

	DELAY(3000);	/* let write command execute */
	if (dma->resr[slot].l & HIFN_D_VALID) {
		printf("%s: SRAM/DRAM detection error -- "
		    "result[%d] valid still set\n", sc->sc_dv.dv_xname, slot);
		return (-1);
	}
	return (0);
}

int
hifn_readramaddr(sc, addr, data, slot)
	struct hifn_softc *sc;
	int addr, slot;
	u_int8_t *data;
{
	struct hifn_dma *dma = sc->sc_dma;
	hifn_base_command_t rc;
	const u_int32_t masks = HIFN_D_VALID | HIFN_D_LAST | HIFN_D_MASKDONEIRQ;

	rc.masks = 2 << 13;
	rc.session_num = addr >> 14;
	rc.total_source_count = addr & 0x3fff;
	rc.total_dest_count = 8;

	*(hifn_base_command_t *)dma->command_bufs[slot] = rc;

	dma->srcr[slot].p = sc->sc_dmamap->dm_segs[0].ds_addr +
	    offsetof(struct hifn_dma, test_src);
	dma->dstr[slot].p =  sc->sc_dmamap->dm_segs[0].ds_addr +
	    offsetof(struct hifn_dma, test_dst);
	dma->cmdr[slot].l = 16 | masks;
	dma->srcr[slot].l = 8 | masks;
	dma->dstr[slot].l = 8 | masks;
	dma->resr[slot].l = HIFN_MAX_RESULT | masks;

	DELAY(1000);	/* let read command execute */
	if (dma->resr[slot].l & HIFN_D_VALID) {
		printf("%s: SRAM/DRAM detection error -- "
		    "result[%d] valid still set\n", sc->sc_dv.dv_xname, slot);
		return (-1);
	}
	bcopy(&dma->test_dst, data, sizeof(dma->test_dst));
	return (0);
}

/*
 * Initialize the descriptor rings.
 */
void 
hifn_init_dma(sc)
	struct hifn_softc *sc;
{
	struct hifn_dma *dma = sc->sc_dma;
	int i;

	/* initialize static pointer values */
	for (i = 0; i < HIFN_D_CMD_RSIZE; i++)
		dma->cmdr[i].p = sc->sc_dmamap->dm_segs[0].ds_addr +
		    offsetof(struct hifn_dma, command_bufs[i][0]);
	for (i = 0; i < HIFN_D_RES_RSIZE; i++)
		dma->resr[i].p = sc->sc_dmamap->dm_segs[0].ds_addr +
		    offsetof(struct hifn_dma, result_bufs[i][0]);

	dma->cmdr[HIFN_D_CMD_RSIZE].p = sc->sc_dmamap->dm_segs[0].ds_addr
	    + offsetof(struct hifn_dma, cmdr[0]);
	dma->srcr[HIFN_D_SRC_RSIZE].p = sc->sc_dmamap->dm_segs[0].ds_addr
	    + offsetof(struct hifn_dma, srcr[0]);
	dma->dstr[HIFN_D_DST_RSIZE].p = sc->sc_dmamap->dm_segs[0].ds_addr
	    + offsetof(struct hifn_dma, dstr[0]);
	dma->resr[HIFN_D_RES_RSIZE].p = sc->sc_dmamap->dm_segs[0].ds_addr
	    + offsetof(struct hifn_dma, resr[0]);
	dma->cmdu = dma->srcu = dma->dstu = dma->resu = 0;
	dma->cmdi = dma->srci = dma->dsti = dma->resi = 0;
	dma->cmdk = dma->srck = dma->dstk = dma->resk = 0;
}

/*
 * Writes out the raw command buffer space.  Returns the
 * command buffer size.
 */
u_int
hifn_write_command(cmd, buf)
	struct hifn_command *cmd;
	u_int8_t *buf;
{
	u_int8_t *buf_pos;
	hifn_base_command_t *base_cmd;
	hifn_mac_command_t *mac_cmd;
	hifn_crypt_command_t *cry_cmd;
	int using_mac, using_crypt, len;
	u_int32_t dlen;

	buf_pos = buf;
	using_mac = cmd->base_masks & HIFN_BASE_CMD_MAC;
	using_crypt = cmd->base_masks & HIFN_BASE_CMD_CRYPT;

	base_cmd = (hifn_base_command_t *)buf_pos;
	base_cmd->masks = cmd->base_masks;
	dlen = cmd->src_map->dm_mapsize;
	base_cmd->total_source_count = dlen & HIFN_BASE_CMD_LENMASK_LO;
	base_cmd->total_dest_count = dlen & HIFN_BASE_CMD_LENMASK_HI;
	dlen >>= 16;
	base_cmd->session_num = cmd->session_num |
	    ((dlen << HIFN_BASE_CMD_SRCLEN_S) & HIFN_BASE_CMD_SRCLEN_M) |
	    ((dlen << HIFN_BASE_CMD_DSTLEN_S) & HIFN_BASE_CMD_DSTLEN_M);
	buf_pos += sizeof(hifn_base_command_t);

	if (using_mac) {
		mac_cmd = (hifn_mac_command_t *)buf_pos;
		dlen = cmd->mac_process_len;
		mac_cmd->source_count = dlen & 0xffff;
		dlen >>= 16;
		mac_cmd->masks = cmd->mac_masks |
		    ((dlen << HIFN_MAC_CMD_SRCLEN_S) & HIFN_MAC_CMD_SRCLEN_M);
		mac_cmd->header_skip = cmd->mac_header_skip;
		buf_pos += sizeof(hifn_mac_command_t);
	}

	if (using_crypt) {
		cry_cmd = (hifn_crypt_command_t *)buf_pos;
		dlen = cmd->crypt_process_len;
		cry_cmd->source_count = dlen & 0xffff;
		dlen >>= 16;
		cry_cmd->masks = cmd->cry_masks |
		    ((dlen << HIFN_CRYPT_CMD_SRCLEN_S) & HIFN_CRYPT_CMD_SRCLEN_M);
		cry_cmd->header_skip = cmd->crypt_header_skip;
		buf_pos += sizeof(hifn_crypt_command_t);
	}

	if (using_mac && mac_cmd->masks & HIFN_MAC_CMD_NEW_KEY) {
		bcopy(cmd->mac, buf_pos, HIFN_MAC_KEY_LENGTH);
		buf_pos += HIFN_MAC_KEY_LENGTH;
	}

	if (using_crypt && cry_cmd->masks & HIFN_CRYPT_CMD_NEW_KEY) {
		len = (cry_cmd->masks & HIFN_CRYPT_CMD_ALG_3DES) ?
		    HIFN_3DES_KEY_LENGTH : HIFN_DES_KEY_LENGTH;
		bcopy(cmd->ck, buf_pos, len);
		buf_pos += len;
	}

	if (using_crypt && cry_cmd->masks & HIFN_CRYPT_CMD_NEW_IV) {
		bcopy(cmd->iv, buf_pos, HIFN_IV_LENGTH);
		buf_pos += HIFN_IV_LENGTH;
	}

	if ((base_cmd->masks & (HIFN_BASE_CMD_MAC | HIFN_BASE_CMD_CRYPT)) == 0) {
		bzero(buf_pos, 8);
		buf_pos += 8;
	}

	return (buf_pos - buf);
}

int
hifn_dmamap_aligned(bus_dmamap_t map)
{
	int i;

	for (i = 0; i < map->dm_nsegs; i++) {
		if ((map->dm_segs[i].ds_addr & 3) ||
		    (map->dm_segs[i].ds_len & 3))
			return (0);
	}
	return (1);
}

void
hifn_dmamap_load(map, idxp, desc, ndesc, usedp)
	bus_dmamap_t map;
	int *idxp, ndesc;
	struct hifn_desc *desc;
	volatile int *usedp;
{
	int i, idx, last = 0;

	for (i = 0; i < map->dm_nsegs; i++) {
		if (i == map->dm_nsegs-1)
			last = HIFN_D_LAST;

		if ((*idxp) == ndesc) {
			idx = 0;
			(*idxp) = 1;
			desc[ndesc].l = HIFN_D_VALID | HIFN_D_JUMP |
			    HIFN_D_MASKDONEIRQ | HIFN_D_LAST;
		} else
			idx = (*idxp)++;
		desc[idx].p = map->dm_segs[i].ds_addr;
		desc[idx].l = map->dm_segs[i].ds_len | HIFN_D_VALID |
		    HIFN_D_MASKDONEIRQ | last;
	}
	(*usedp) += map->dm_nsegs;
}

int 
hifn_crypto(sc, cmd, crp)
	struct hifn_softc *sc;
	struct hifn_command *cmd;
	struct cryptop *crp;
{
	struct	hifn_dma *dma = sc->sc_dma;
	u_int32_t cmdlen;
	int cmdi, resi, s;

	if (bus_dmamap_create(sc->sc_dmat, HIFN_MAX_DMALEN, MAX_SCATTER,
	    HIFN_MAX_SEGLEN, 0, BUS_DMA_NOWAIT, &cmd->src_map))
		return (-1);

	if (crp->crp_flags & CRYPTO_F_IMBUF) {
		if (bus_dmamap_load_mbuf(sc->sc_dmat, cmd->src_map,
		    cmd->src_m, BUS_DMA_NOWAIT))
			goto err_srcmap1;
	} else if (crp->crp_flags & CRYPTO_F_IOV) {
		if (bus_dmamap_load_uio(sc->sc_dmat, cmd->src_map,
		    cmd->src_io, BUS_DMA_NOWAIT))
			goto err_srcmap1;
	} else
		goto err_srcmap1;

	if (hifn_dmamap_aligned(cmd->src_map)) {
		if (crp->crp_flags & CRYPTO_F_IOV)
			cmd->dst_io = cmd->src_io;
		else if (crp->crp_flags & CRYPTO_F_IMBUF)
			cmd->dst_m = cmd->src_m;
		cmd->dst_map = cmd->src_map;
	} else {
		if (crp->crp_flags & CRYPTO_F_IOV)
			goto err_srcmap;
		if (crp->crp_flags & CRYPTO_F_IMBUF) {
			int totlen, len;
			struct mbuf *m, *top, **mp;

			totlen = cmd->src_map->dm_mapsize;
			if (cmd->src_m->m_flags & M_PKTHDR) {
				len = MHLEN;
				MGETHDR(m, M_DONTWAIT, MT_DATA);
			} else {
				len = MLEN;
				MGET(m, M_DONTWAIT, MT_DATA);
			}
			if (m == NULL)
				goto err_srcmap;
			if (len == MHLEN)
				M_DUP_PKTHDR(m, cmd->src_m);
			if (totlen >= MINCLSIZE) {
				MCLGET(m, M_DONTWAIT);
				if (m->m_flags & M_EXT)
					len = MCLBYTES;
			}
			m->m_len = len;
			top = NULL;
			mp = &top;

			while (totlen > 0) {
				if (top) {
					MGET(m, M_DONTWAIT, MT_DATA);
					if (m == NULL) {
						m_freem(top);
						goto err_srcmap;
					}
					len = MLEN;
				}
				if (top && totlen >= MINCLSIZE) {
					MCLGET(m, M_DONTWAIT);
					if (m->m_flags & M_EXT)
						len = MCLBYTES;
				}
				m->m_len = len;
				totlen -= len;
				*mp = m;
				mp = &m->m_next;
			}
			cmd->dst_m = top;
		}
	}

	if (cmd->dst_map == NULL) {
		if (bus_dmamap_create(sc->sc_dmat,
		    HIFN_MAX_SEGLEN * MAX_SCATTER, MAX_SCATTER,
		    HIFN_MAX_SEGLEN, 0, BUS_DMA_NOWAIT, &cmd->dst_map))
			goto err_srcmap;
		if (crp->crp_flags & CRYPTO_F_IMBUF) {
			if (bus_dmamap_load_mbuf(sc->sc_dmat, cmd->dst_map,
			    cmd->dst_m, BUS_DMA_NOWAIT))
				goto err_dstmap1;
		} else if (crp->crp_flags & CRYPTO_F_IOV) {
			if (bus_dmamap_load_uio(sc->sc_dmat, cmd->dst_map,
			    cmd->dst_io, BUS_DMA_NOWAIT))
				goto err_dstmap1;
		}
	}

#ifdef HIFN_DEBUG
	printf("%s: Entering cmd: stat %8x ien %8x u %d/%d/%d/%d n %d/%d\n",
	    sc->sc_dv.dv_xname,
	    READ_REG_1(sc, HIFN_1_DMA_CSR), READ_REG_1(sc, HIFN_1_DMA_IER),
	    dma->cmdu, dma->srcu, dma->dstu, dma->resu,
	    cmd->src_map->dm_nsegs, cmd->dst_map->dm_nsegs);
#endif

	bus_dmamap_sync(sc->sc_dmat, cmd->src_map, BUS_DMASYNC_PREREAD);
	bus_dmamap_sync(sc->sc_dmat, cmd->dst_map, BUS_DMASYNC_PREWRITE);

	s = splnet();

	bus_dmamap_sync(sc->sc_dmat, sc->sc_dmamap,
	    BUS_DMASYNC_POSTREAD | BUS_DMASYNC_POSTWRITE);

	/*
	 * need 1 cmd, and 1 res
	 * need N src, and N dst
	 */
	if (dma->cmdu+1 > HIFN_D_CMD_RSIZE ||
	    dma->srcu+cmd->src_map->dm_nsegs > HIFN_D_SRC_RSIZE ||
	    dma->dstu+cmd->dst_map->dm_nsegs > HIFN_D_DST_RSIZE ||
	    dma->resu+1 > HIFN_D_RES_RSIZE) {
		bus_dmamap_sync(sc->sc_dmat, sc->sc_dmamap,
		    BUS_DMASYNC_PREREAD | BUS_DMASYNC_PREWRITE);
		splx(s);
		goto err_dstmap;
	}

	if (dma->cmdi == HIFN_D_CMD_RSIZE) {
		dma->cmdi = 0;
		dma->cmdr[HIFN_D_CMD_RSIZE].l = HIFN_D_VALID | HIFN_D_LAST |
		    HIFN_D_MASKDONEIRQ | HIFN_D_JUMP;
	}
	cmdi = dma->cmdi++;

	if (dma->resi == HIFN_D_RES_RSIZE) {
		dma->resi = 0;
		dma->resr[HIFN_D_RES_RSIZE].l = HIFN_D_VALID | HIFN_D_LAST |
		    HIFN_D_MASKDONEIRQ | HIFN_D_JUMP;
	}
	resi = dma->resi++;

	cmdlen = hifn_write_command(cmd, dma->command_bufs[cmdi]);
#ifdef HIFN_DEBUG
	printf("write_command %d (nice %d)\n", cmdlen,
	    hifn_dmamap_aligned(cmd->src_map));
#endif
	/* .p for command/result already set */
	dma->cmdr[cmdi].l = cmdlen | HIFN_D_VALID | HIFN_D_LAST |
	    HIFN_D_MASKDONEIRQ;
	dma->cmdu++;

	/*
	 * We don't worry about missing an interrupt (which a "command wait"
	 * interrupt salvages us from), unless there is more than one command
	 * in the queue.
	 */
	if (dma->cmdu > 1) {
		sc->sc_dmaier |= HIFN_DMAIER_C_WAIT;
		WRITE_REG_1(sc, HIFN_1_DMA_IER, sc->sc_dmaier);
	}

	hifnstats.hst_ipackets++;
	hifnstats.hst_ibytes += cmd->src_map->dm_mapsize;

	hifn_dmamap_load(cmd->src_map, &dma->srci, dma->srcr,
	    HIFN_D_SRC_RSIZE, &dma->srcu);

	hifn_dmamap_load(cmd->dst_map, &dma->dsti, dma->dstr,
	    HIFN_D_DST_RSIZE, &dma->dstu);

	/*
	 * Unlike other descriptors, we don't mask done interrupt from
	 * result descriptor.
	 */
#ifdef HIFN_DEBUG
	printf("load res\n");
#endif
	dma->hifn_commands[resi] = cmd;
	dma->resr[resi].l = HIFN_MAX_RESULT | HIFN_D_VALID | HIFN_D_LAST;
	dma->resu++;

#ifdef HIFN_DEBUG
	printf("%s: command: stat %8x ier %8x\n",
	    sc->sc_dv.dv_xname,
	    READ_REG_1(sc, HIFN_1_DMA_CSR), READ_REG_1(sc, HIFN_1_DMA_IER));
#endif

	bus_dmamap_sync(sc->sc_dmat, sc->sc_dmamap,
	    BUS_DMASYNC_PREREAD | BUS_DMASYNC_PREWRITE);

	splx(s);
	return 0;		/* success */

err_dstmap:
	if (cmd->src_map != cmd->dst_map)
		bus_dmamap_unload(sc->sc_dmat, cmd->dst_map);
err_dstmap1:
	if (cmd->src_map != cmd->dst_map)
		bus_dmamap_destroy(sc->sc_dmat, cmd->dst_map);
err_srcmap:
	bus_dmamap_unload(sc->sc_dmat, cmd->src_map);
err_srcmap1:
	bus_dmamap_destroy(sc->sc_dmat, cmd->src_map);
	return (-1);
}

int 
hifn_intr(arg)
	void *arg;
{
	struct hifn_softc *sc = arg;
	struct hifn_dma *dma = sc->sc_dma;
	u_int32_t dmacsr;
	int i, r = 0, u;

	dmacsr = READ_REG_1(sc, HIFN_1_DMA_CSR);

#ifdef HIFN_DEBUG
	printf("%s: irq: stat %08x ien %08x u %d/%d/%d/%d\n",
	    sc->sc_dv.dv_xname,
	    dmacsr, READ_REG_1(sc, HIFN_1_DMA_IER),
	    dma->cmdu, dma->srcu, dma->dstu, dma->resu);
#endif

	/* Nothing in the DMA unit interrupted */
	if ((dmacsr & sc->sc_dmaier) == 0)
		return (r);

	if ((sc->sc_flags & HIFN_HAS_PUBLIC) &&
	    (dmacsr & HIFN_DMACSR_PUBDONE)) {
		r = 1;
		dmacsr &= ~HIFN_DMACSR_PUBDONE;
	}

	if ((dmacsr & sc->sc_dmaier) == 0)
		return (r);

	r = 1;

	bus_dmamap_sync(sc->sc_dmat, sc->sc_dmamap, 
	    BUS_DMASYNC_POSTWRITE | BUS_DMASYNC_POSTREAD); 

	if (dma->resu > HIFN_D_RES_RSIZE)
		printf("%s: Internal Error -- ring overflow\n",
		    sc->sc_dv.dv_xname);

	if ((dmacsr & HIFN_DMACSR_C_WAIT) && (dma->cmdu == 0)) {
		/*
		 * If no slots to process and we receive a "waiting on
		 * command" interrupt, we disable the "waiting on command"
		 * (by clearing it).
		 */
		sc->sc_dmaier &= ~HIFN_DMAIER_C_WAIT;
		WRITE_REG_1(sc, HIFN_1_DMA_IER, sc->sc_dmaier);
	}

	while (dma->resu > 0) {
		struct hifn_command *cmd;
		u_int8_t *macbuf = NULL;

		cmd = dma->hifn_commands[dma->resk];

		/* if still valid, stop processing */
		if (dma->resr[dma->resk].l & HIFN_D_VALID)
			break;

		if (cmd->base_masks & HIFN_BASE_CMD_MAC) {
			macbuf = dma->result_bufs[dma->resk];
			macbuf += 12;
		}

		hifn_callback(sc, cmd, macbuf);
	
		if (++dma->resk == HIFN_D_RES_RSIZE)
			dma->resk = 0;
		dma->resu--;
		hifnstats.hst_opackets++;
	}

	/* clear the rings */

	i = dma->srck; u = dma->srcu;
	while (u != 0 && (dma->srcr[i].l & HIFN_D_VALID) == 0) {
		if (++i == HIFN_D_SRC_RSIZE)
			i = 0;
		u--;
	}
	dma->srck = i; dma->srcu = u;

	i = dma->cmdk; u = dma->cmdu;
	while (u != 0 && (dma->cmdr[i].l & HIFN_D_VALID) == 0) {
		if (++i == HIFN_D_CMD_RSIZE)
			i = 0;
		u--;
	}
	dma->cmdk = i; dma->cmdu = u;

	/*
	 * Clear "result done" and "command wait" flags in status register.
	 * If we still have slots to process and we received a "command wait"
	 * interrupt, this will interupt us again.
	 */
	WRITE_REG_1(sc, HIFN_1_DMA_CSR, HIFN_DMACSR_R_DONE|HIFN_DMACSR_C_WAIT);
	bus_dmamap_sync(sc->sc_dmat, sc->sc_dmamap, 
	    BUS_DMASYNC_PREWRITE | BUS_DMASYNC_PREREAD); 
	return (r);
}

/*
 * Allocate a new 'session' and return an encoded session id.  'sidp'
 * contains our registration id, and should contain an encoded session
 * id on successful allocation.
 */
int
hifn_newsession(sidp, cri)
	u_int32_t *sidp;
	struct cryptoini *cri;
{
	struct cryptoini *c;
	struct hifn_softc *sc = NULL;
	int i, mac = 0, cry = 0;

	if (sidp == NULL || cri == NULL)
		return (EINVAL);

	for (i = 0; i < hifn_cd.cd_ndevs; i++) {
		sc = hifn_cd.cd_devs[i];
		if (sc == NULL)
			break;
		if (sc->sc_cid == (*sidp))
			break;
	}
	if (sc == NULL)
		return (EINVAL);

	for (i = 0; i < sc->sc_maxses; i++)
		if (sc->sc_sessions[i].hs_flags == 0)
			break;
	if (i == sc->sc_maxses)
		return (ENOMEM);

	for (c = cri; c != NULL; c = c->cri_next) {
		if (c->cri_alg == CRYPTO_MD5_HMAC ||
		    c->cri_alg == CRYPTO_SHA1_HMAC) {
			if (mac)
				return (EINVAL);
			mac = 1;
		} else if (c->cri_alg == CRYPTO_DES_CBC ||
		    c->cri_alg == CRYPTO_3DES_CBC) {
			if (cry)
				return (EINVAL);
			cry = 1;
		} else
			return (EINVAL);

	}
	if (mac == 0 && cry == 0)
		return (EINVAL);

	*sidp = HIFN_SID(sc->sc_dv.dv_unit, i);
	sc->sc_sessions[i].hs_flags = 1;
	get_random_bytes(sc->sc_sessions[i].hs_iv, HIFN_IV_LENGTH);

	return (0);
}

/*
 * Deallocate a session.
 * XXX this routine should run a zero'd mac/encrypt key into context ram.
 * XXX to blow away any keys already stored there.
 */
int
hifn_freesession(tid)
	u_int64_t tid;
{
	struct hifn_softc *sc;
	int card, session;
	u_int32_t sid = ((u_int32_t) tid) & 0xffffffff;

	card = HIFN_CARD(sid);
	if (card >= hifn_cd.cd_ndevs || hifn_cd.cd_devs[card] == NULL)
		return (EINVAL);

	sc = hifn_cd.cd_devs[card];
	session = HIFN_SESSION(sid);
	if (session >= sc->sc_maxses)
		return (EINVAL);

	bzero(&sc->sc_sessions[session], sizeof(sc->sc_sessions[session]));
	return (0);
}

int
hifn_process(crp)
	struct cryptop *crp;
{
	struct hifn_command *cmd = NULL;
	int card, session, err;
	struct hifn_softc *sc;
	struct cryptodesc *crd1, *crd2, *maccrd, *enccrd;

	if (crp == NULL || crp->crp_callback == NULL) {
		hifnstats.hst_invalid++;
		return (EINVAL);
	}

	card = HIFN_CARD(crp->crp_sid);
	if (card >= hifn_cd.cd_ndevs || hifn_cd.cd_devs[card] == NULL) {
		err = EINVAL;
		goto errout;
	}

	sc = hifn_cd.cd_devs[card];
	session = HIFN_SESSION(crp->crp_sid);
	if (session >= sc->sc_maxses) {
		err = EINVAL;
		goto errout;
	}

	cmd = (struct hifn_command *)malloc(sizeof(struct hifn_command),
	    M_DEVBUF, M_NOWAIT);
	if (cmd == NULL) {
		err = ENOMEM;
		goto errout;
	}
	bzero(cmd, sizeof(struct hifn_command));

	if (crp->crp_flags & CRYPTO_F_IMBUF) {
		cmd->src_m = (struct mbuf *)crp->crp_buf;
		cmd->dst_m = (struct mbuf *)crp->crp_buf;
	} else if (crp->crp_flags & CRYPTO_F_IOV) {
		cmd->src_io = (struct uio *)crp->crp_buf;
		cmd->dst_io = (struct uio *)crp->crp_buf;
	} else {
		err = EINVAL;
		goto errout;	/* XXX only handle mbufs right now */
	}

	crd1 = crp->crp_desc;
	if (crd1 == NULL) {
		err = EINVAL;
		goto errout;
	}
	crd2 = crd1->crd_next;

	if (crd2 == NULL) {
		if (crd1->crd_alg == CRYPTO_MD5_HMAC ||
		    crd1->crd_alg == CRYPTO_SHA1_HMAC) {
			maccrd = crd1;
			enccrd = NULL;
		} else if (crd1->crd_alg == CRYPTO_DES_CBC ||
			 crd1->crd_alg == CRYPTO_3DES_CBC) {
			if ((crd1->crd_flags & CRD_F_ENCRYPT) == 0)
				cmd->base_masks |= HIFN_BASE_CMD_DECODE;
			maccrd = NULL;
			enccrd = crd1;
		} else {
			err = EINVAL;
			goto errout;
		}
	} else {
		if ((crd1->crd_alg == CRYPTO_MD5_HMAC ||
		    crd1->crd_alg == CRYPTO_SHA1_HMAC) &&
		    (crd2->crd_alg == CRYPTO_DES_CBC ||
			crd2->crd_alg == CRYPTO_3DES_CBC) &&
		    ((crd2->crd_flags & CRD_F_ENCRYPT) == 0)) {
			cmd->base_masks = HIFN_BASE_CMD_DECODE;
			maccrd = crd1;
			enccrd = crd2;
		} else if ((crd1->crd_alg == CRYPTO_DES_CBC ||
		    crd1->crd_alg == CRYPTO_3DES_CBC) &&
		    (crd2->crd_alg == CRYPTO_MD5_HMAC ||
			crd2->crd_alg == CRYPTO_SHA1_HMAC) &&
		    (crd1->crd_flags & CRD_F_ENCRYPT)) {
			enccrd = crd1;
			maccrd = crd2;
		} else {
			/*
			 * We cannot order the 7751 as requested
			 */
			err = EINVAL;
			goto errout;
		}
	}

	if (enccrd) {
		cmd->base_masks |= HIFN_BASE_CMD_CRYPT;
		cmd->cry_masks |= HIFN_CRYPT_CMD_MODE_CBC |
		    HIFN_CRYPT_CMD_NEW_IV;
		if (enccrd->crd_flags & CRD_F_ENCRYPT) {
			if (enccrd->crd_flags & CRD_F_IV_EXPLICIT)
				bcopy(enccrd->crd_iv, cmd->iv, HIFN_IV_LENGTH);
			else
				bcopy(sc->sc_sessions[session].hs_iv,
				    cmd->iv, HIFN_IV_LENGTH);

			if ((enccrd->crd_flags & CRD_F_IV_PRESENT) == 0) {
				if (crp->crp_flags & CRYPTO_F_IMBUF)
					m_copyback(cmd->src_m, enccrd->crd_inject,
					    HIFN_IV_LENGTH, cmd->iv);
				else if (crp->crp_flags & CRYPTO_F_IOV) {
					if (crp->crp_iv == NULL)
						bzero(cmd->iv, 8);
					else
						bcopy(crp->crp_iv, cmd->iv, 8);
				}
			}
		} else {
			if (enccrd->crd_flags & CRD_F_IV_EXPLICIT)
				bcopy(enccrd->crd_iv, cmd->iv, HIFN_IV_LENGTH);
			else if (crp->crp_flags & CRYPTO_F_IMBUF)
				m_copydata(cmd->src_m, enccrd->crd_inject,
				    HIFN_IV_LENGTH, cmd->iv);
			else if (crp->crp_flags & CRYPTO_F_IOV) {
				if (crp->crp_iv == NULL)
					bzero(cmd->iv, 8);
				else
					bcopy(crp->crp_iv, cmd->iv, 8);
			}
		}

		if (enccrd->crd_alg == CRYPTO_DES_CBC)
			cmd->cry_masks |= HIFN_CRYPT_CMD_ALG_DES;
		else
			cmd->cry_masks |= HIFN_CRYPT_CMD_ALG_3DES;

		cmd->crypt_header_skip = enccrd->crd_skip;
		cmd->crypt_process_len = enccrd->crd_len;
		cmd->ck = enccrd->crd_key;

		if (sc->sc_sessions[session].hs_flags == 1)
			cmd->cry_masks |= HIFN_CRYPT_CMD_NEW_KEY;
	}

	if (maccrd) {
		cmd->base_masks |= HIFN_BASE_CMD_MAC;
		cmd->mac_masks |= HIFN_MAC_CMD_RESULT |
		    HIFN_MAC_CMD_MODE_HMAC | HIFN_MAC_CMD_RESULT |
		    HIFN_MAC_CMD_POS_IPSEC | HIFN_MAC_CMD_TRUNC;

		if (maccrd->crd_alg == CRYPTO_MD5_HMAC)
			cmd->mac_masks |= HIFN_MAC_CMD_ALG_MD5;
		else
			cmd->mac_masks |= HIFN_MAC_CMD_ALG_SHA1;

		if (sc->sc_sessions[session].hs_flags == 1) {
			cmd->mac_masks |= HIFN_MAC_CMD_NEW_KEY;
			bcopy(maccrd->crd_key, cmd->mac, maccrd->crd_klen >> 3);
			bzero(cmd->mac + (maccrd->crd_klen >> 3),
			    HIFN_MAC_KEY_LENGTH - (maccrd->crd_klen >> 3));
		}

		cmd->mac_header_skip = maccrd->crd_skip;
		cmd->mac_process_len = maccrd->crd_len;
	}

	if (sc->sc_sessions[session].hs_flags == 1)
		sc->sc_sessions[session].hs_flags = 2;

	cmd->private_data = (u_long)crp;
	cmd->session_num = session;
	cmd->softc = sc;

	if (hifn_crypto(sc, cmd, crp) == 0)
		return (0);

	err = ENOMEM;

errout:
	if (cmd != NULL)
		free(cmd, M_DEVBUF);
	if (err == EINVAL)
		hifnstats.hst_invalid++;
	else
		hifnstats.hst_nomem++;
	crp->crp_etype = err;
	crp->crp_callback(crp);
	return (0);
}

void
hifn_callback(sc, cmd, macbuf)
	struct hifn_softc *sc;
	struct hifn_command *cmd;
	u_int8_t *macbuf;
{
	struct hifn_dma *dma = sc->sc_dma;
	struct cryptop *crp = (struct cryptop *)cmd->private_data;
	struct cryptodesc *crd;
	struct mbuf *m;
	int totlen;

	bus_dmamap_sync(sc->sc_dmat, cmd->src_map, BUS_DMASYNC_POSTREAD);
	bus_dmamap_sync(sc->sc_dmat, cmd->dst_map, BUS_DMASYNC_POSTWRITE);

	if (crp->crp_flags & CRYPTO_F_IMBUF) {
		if (cmd->src_m != cmd->dst_m) {
			m_freem(cmd->src_m);
			crp->crp_buf = (caddr_t)cmd->dst_m;
		}
		if ((m = cmd->dst_m) != NULL) {
			totlen = cmd->src_map->dm_mapsize;
			hifnstats.hst_obytes += totlen;
			while (m) {
				if (totlen < m->m_len) {
					m->m_len = totlen;
					totlen = 0;
				} else
					totlen -= m->m_len;
				m = m->m_next;
				if (++dma->dstk == HIFN_D_DST_RSIZE)
					dma->dstk = 0;
				dma->dstu--;
			}
		} else {
			hifnstats.hst_obytes += dma->dstr[dma->dstk].l & HIFN_D_LENGTH;
			if (++dma->dstk == HIFN_D_DST_RSIZE)
				dma->dstk = 0;
			dma->dstu--;
		}
	} else if (crp->crp_flags & CRYPTO_F_IOV) {
		hifnstats.hst_obytes += cmd->dst_map->dm_mapsize;
		dma->dstk = (dma->dstk + cmd->dst_map->dm_nsegs) % HIFN_D_DST_RSIZE;
		dma->dstu -= cmd->dst_map->dm_nsegs;
	}

	if ((cmd->base_masks & (HIFN_BASE_CMD_CRYPT | HIFN_BASE_CMD_DECODE)) ==
	    HIFN_BASE_CMD_CRYPT) {
		for (crd = crp->crp_desc; crd; crd = crd->crd_next) {
			if (crd->crd_alg != CRYPTO_DES_CBC &&
			    crd->crd_alg != CRYPTO_3DES_CBC)
				continue;
			if (crp->crp_flags & CRYPTO_F_IMBUF)
				m_copydata((struct mbuf *)crp->crp_buf,
				    crd->crd_skip + crd->crd_len - HIFN_IV_LENGTH,
				    HIFN_IV_LENGTH,
				    cmd->softc->sc_sessions[cmd->session_num].hs_iv);
			else if (crp->crp_flags & CRYPTO_F_IOV) {
				cuio_copydata((struct uio *)crp->crp_buf,
				    crd->crd_skip + crd->crd_len - HIFN_IV_LENGTH,
				    HIFN_IV_LENGTH,
				    cmd->softc->sc_sessions[cmd->session_num].hs_iv);
			}
			break;
		}
	}

	if (macbuf != NULL) {
		for (crd = crp->crp_desc; crd; crd = crd->crd_next) {
			if (crd->crd_alg != CRYPTO_MD5_HMAC &&
			    crd->crd_alg != CRYPTO_SHA1_HMAC)
				continue;
			if (crp->crp_flags & CRYPTO_F_IMBUF)
				m_copyback((struct mbuf *)crp->crp_buf,
				    crd->crd_inject, 12, macbuf);
			else if ((crp->crp_flags & CRYPTO_F_IOV) && crp->crp_mac)
				bcopy((caddr_t)macbuf, crp->crp_mac, 12);
			break;
		}
	}

	if (cmd->src_map != cmd->dst_map) {
		bus_dmamap_unload(sc->sc_dmat, cmd->dst_map);
		bus_dmamap_destroy(sc->sc_dmat, cmd->dst_map);
	}
	bus_dmamap_unload(sc->sc_dmat, cmd->src_map);
	bus_dmamap_destroy(sc->sc_dmat, cmd->src_map);
	free(cmd, M_DEVBUF);
	crypto_done(crp);
}
