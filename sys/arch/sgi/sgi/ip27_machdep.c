/*	$OpenBSD: ip27_machdep.c,v 1.77 2017/02/11 03:44:22 visa Exp $	*/

/*
 * Copyright (c) 2008, 2009 Miodrag Vallat.
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

/*
 * Origin 200 / Origin 2000 / Onyx 2 (IP27), as well as
 * Origin 300 / Onyx 300 / Origin 350 / Onyx 350 / Onyx 4 / Origin 3000 /
 * Fuel / Tezro (IP35) specific code.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/atomic.h>
#include <sys/device.h>
#include <sys/malloc.h>
#include <sys/proc.h>
#include <sys/reboot.h>
#include <sys/timetc.h>
#include <sys/tty.h>

#include <mips64/arcbios.h>
#include <mips64/archtype.h>
#include <mips64/cache.h>

#include <machine/autoconf.h>
#include <machine/bus.h>
#include <machine/cpu.h>
#include <mips64/mips_cpu.h>
#include <machine/memconf.h>
#include <machine/mnode.h>

#include <uvm/uvm_extern.h>

#include <sgi/sgi/ip27.h>
#include <sgi/sgi/l1.h>
#include <sgi/xbow/hub.h>
#include <sgi/xbow/widget.h>
#include <sgi/xbow/xbow.h>

#include <sgi/pci/iofreg.h>
#include <dev/ic/comvar.h>

#include <dev/cons.h>

extern char *hw_prod;

extern void (*md_halt)(int);

paddr_t	ip27_widget_short(int16_t, u_int);
paddr_t	ip27_widget_long(int16_t, u_int);
paddr_t	ip27_widget_map(int16_t, u_int,bus_addr_t *, bus_size_t *);
int	ip27_widget_id(int16_t, u_int, uint32_t *);
int	ip27_widget_id_early(int16_t, u_int, uint32_t *);

void	ip27_halt(int);

unsigned int xbow_long_shift = 29;

static paddr_t io_base;
static int ip35 = 0;
uint	 maxnodes;
gda_t	*gda;

int	ip27_hub_intr_register(int, int, int *);
int	ip27_hub_intr_establish(int (*)(void *), void *, int, int,
	    const char *, struct intrhand *);
void	ip27_hub_intr_disestablish(int);
void	ip27_hub_intr_clear(int);
void	ip27_hub_intr_set(int);
uint32_t hubpi_intr0(uint32_t, struct trapframe *);
uint32_t hubpi_intr1(uint32_t, struct trapframe *);
void	ip27_hub_intr_makemasks0(void);
void	ip27_hub_intr_makemasks1(void);
void	ip27_hub_setintrmask(int);
void	ip27_hub_splx(int);

void	ip27_attach_node(struct device *, int16_t);
int	ip27_print(void *, const char *);
void	ip27_nmi(void *);

#ifdef MULTIPROCESSOR

#define IP27_SLICE_IPI(slice) ((slice) + HUBPI_ISR0_IPI_A)

unsigned int	ip27_ncpus;

int	ip27_kl_launch_cpu(klinfo_t *, void *);
int	ip27_kl_launch_cpu_board(lboard_t *, void *);
int	ip27_kl_attach_cpu(klinfo_t *, void *);
int	ip27_kl_attach_cpu_board(lboard_t *, void *);

uint	ip27_hub_get_timecount(struct timecounter *);

struct timecounter ip27_hub_timecounter = {
	.tc_get_timecount = ip27_hub_get_timecount,
	.tc_poll_pps = NULL,
	.tc_counter_mask = 0xffffffff,	/* truncated to 32 bits. */
	.tc_frequency = 1250000,
	.tc_name = "hubrt",
	.tc_quality = 100
};

volatile uint64_t	ip27_spinup_a0;
volatile uint64_t	ip27_spinup_sp;
volatile uint32_t	ip27_spinup_turn = ~0;

#define SPINUP_TICKET(nasid, slice)	(((nasid) << 8) | (slice))

void ip27_cpu_spinup_trampoline(uint64_t);

#endif /* MULTIPROCESSOR */

/*
 * IP27 interrupt handling declarations: 128 hw sources, plus timers and
 * hub error sources; 5 levels.
 */

struct intrhand *hubpi_intrhand0[HUBPI_NINTS];
struct intrhand *hubpi_intrhand1[HUBPI_NINTS];

#ifdef notyet
#define	INTPRI_XBOW_HUB		(INTPRI_CLOCK + 1)	/* HUB errors */
#define	INTPRI_XBOW_TIMER	(INTPRI_XBOW_HUB + 1)	/* prof timer */
#define	INTPRI_XBOW_CLOCK	(INTPRI_XBOW_TIMER + 1)	/* RTC */
#define	INTPRI_XBOW_HW1		(INTPRI_XBOW_CLOCK + 1)	/* HW level 1 */
#else
#define	INTPRI_XBOW_HW1		(INTPRI_CLOCK + 1)	/* HW level 1 */
#endif
#define	INTPRI_XBOW_HW0		(INTPRI_XBOW_HW1 + 1)	/* HW level 0 */

struct {
	uint64_t hw[2];
} hubpi_intem[MAXCPUS], hubpi_imask[MAXCPUS][NIPLS];

void
ip27_setup()
{
	struct cpu_info *ci = curcpu();
	struct ip27_config *ip27_config;
	uint64_t synergy0_0;
	console_t *cons;
	nmi_t *nmi;
	static char unknown_model[20];

	ci->ci_nasid = masternasid;
	ci->ci_slice = IP27_LHUB_L(HUBPI_CPU_NUMBER);

	io_base = PHYS_TO_XKPHYS_UNCACHED(0, SP_IO);

	ip35 = sys_config.system_type == SGI_IP35;

	if (ip35) {
		/*
		 * Get brick model type.
		 * We need to access the Synergy registers through the remote
		 * HUB interface, local access is protected on some models.
		 * Synergy0 register #0 is a 16 bits identification register.
		 */
		synergy0_0 = IP27_RHSPEC_L(0, HSPEC_SYNERGY(0, 0));
		sys_config.system_subtype = (synergy0_0 & 0xf000) >> 12;
		switch (sys_config.system_subtype) {
		case IP35_O350:	/* Chimera */
			hw_prod = "Origin 350";
			break;
		case IP35_FUEL:	/* Asterix */
			hw_prod = "Fuel";
			break;
		case IP35_O300:	/* Speedo2 */
			hw_prod = "Origin 300";
			break;
		case IP35_CBRICK:
			/* regular C-Brick, must be an Origin 3000 system */
			hw_prod = "Origin 3000";
			break;
		default:
			snprintf(unknown_model, sizeof unknown_model,
			    "Unknown IP35 type %x", sys_config.system_subtype);
			hw_prod = unknown_model;
			break;
		}
	} else {
		ip27_config = (struct ip27_config *)
		    IP27_LHSPEC_ADDR(LBOOTBASE_IP27 + IP27_CONFIG_OFFSET);
		if (ip27_config->magic == IP27_CONFIG_MAGIC)
			sys_config.system_subtype = ip27_config->ip27_subtype;
		else
			sys_config.system_subtype = IP27_UNKNOWN;
		switch (sys_config.system_subtype) {
		case IP27_O2K:
			hw_prod = "Origin 2000";
			break;
		case IP27_O200:
			hw_prod = "Origin 200";
			break;
		default:
			snprintf(unknown_model, sizeof unknown_model,
			    "Unknown IP27 type %x", sys_config.system_subtype);
			hw_prod = unknown_model;
			break;
		}
	}

	/*
	 * Register DMA-reachable memory constraints.
	 * The xbridge(4) is limited to a 31-bit region (its IOMMU features
	 * are too restricted to be of use).
	 */
	dma_constraint.ucr_low = 0;
	dma_constraint.ucr_high = (1UL << 31) - 1;

	xbow_widget_base = ip27_widget_short;
	xbow_widget_map = ip27_widget_map;
	xbow_widget_id = ip27_widget_id_early;

	md_halt = ip27_halt;

	/*
	 * Figure out as early as possibly whether we are running in M
	 * or N mode.
	 */

	kl_init(ip35);
	if (kl_n_mode != 0)
		xbow_long_shift = 28;

#ifdef MULTIPROCESSOR
	/*
	 * Pre-launch secondary CPUs with the help of the PROM. This has to be
	 * done now, before tearing down the PROM TLB and disabling interrupts
	 * on the secondary CPUs. The CPUs will wait in the spinup trampoline
	 * until the system launches them for real.
	 */
	ip27_ncpus = 1;
	kl_scan_all_nodes(IP27_BC_NODE, ip27_kl_launch_cpu_board, NULL);
#endif

	/*
	 * Initialize the early console parameters.
	 * This assumes it is either on IOC3 or IOC4, accessible through
	 * a widget small window.
	 *
	 * Since IOC3 and IOC4 use different clocks, we need to tell them
	 * apart early. We rely on the serial port offset within the IOC
	 * space.
	 */

	cons = kl_get_console();
	xbow_build_bus_space(&sys_config.console_io, 0,
	    8 /* whatever nonzero */);
	/* point to devio base */
	sys_config.console_io.bus_base =
	    cons->uart_base & 0xfffffffffff00000UL;
	comconsaddr = cons->uart_base & 0x00000000000fffffUL;
	comconsrate = cons->baud;
	if (comconsrate < 50 || comconsrate > 115200)
		comconsrate = 9600;
	if ((comconsaddr & 0xfff) < 0x380) {
		/* IOC3 */
		comconsfreq = 22000000 / 3;
	} else {
		/* IOC4 */
		uint32_t ioc4_mcr;
		paddr_t ioc4_base;

		/*
		 * IOC4 clocks are derived from the PCI clock,
		 * so we need to figure out whether this is an 66MHz
		 * or a 33MHz bus.
		 */
		ioc4_base = sys_config.console_io.bus_base;
		ioc4_mcr = *(volatile uint32_t *)(ioc4_base + IOC4_MCR);
		if (ioc4_mcr & IOC4_MCR_PCI_66MHZ)
			comconsfreq = 66666667;
		else
			comconsfreq = 33333333;
	}
	comconsiot = &sys_config.console_io;

	/*
	 * Force widget interrupts to run through us, unless a
	 * better interrupt master widget is found.
	 */

	xbow_intr_widget_intr_register = ip27_hub_intr_register;
	xbow_intr_widget_intr_establish = ip27_hub_intr_establish;
	xbow_intr_widget_intr_disestablish = ip27_hub_intr_disestablish;
	xbow_intr_widget_intr_clear = ip27_hub_intr_clear;
	xbow_intr_widget_intr_set = ip27_hub_intr_set;

	set_intr(INTPRI_XBOW_HW1, CR_INT_1, hubpi_intr1);
	set_intr(INTPRI_XBOW_HW0, CR_INT_0, hubpi_intr0);
	register_splx_handler(ip27_hub_splx);

	/*
	 * Disable all hardware interrupts.
	 */

	IP27_LHUB_S(HUBPI_CPU0_IMR0, 0);
	IP27_LHUB_S(HUBPI_CPU0_IMR1, 0);
	IP27_LHUB_S(HUBPI_CPU1_IMR0, 0);
	IP27_LHUB_S(HUBPI_CPU1_IMR1, 0);
	(void)IP27_LHUB_L(HUBPI_IR0);
	(void)IP27_LHUB_L(HUBPI_IR1);
	if (ip35) {
		IP27_RHUB_PI_S(masternasid, 1, HUBPI_CPU0_IMR0, 0);
		IP27_RHUB_PI_S(masternasid, 1, HUBPI_CPU0_IMR1, 0);
		IP27_RHUB_PI_S(masternasid, 1, HUBPI_CPU1_IMR0, 0);
		IP27_RHUB_PI_S(masternasid, 1, HUBPI_CPU1_IMR1, 0);
		(void)IP27_RHUB_PI_L(masternasid, 1, HUBPI_IR0);
		(void)IP27_RHUB_PI_L(masternasid, 1, HUBPI_IR1);
	}

	/*
	 * Setup NMI handler.
	 */
	nmi = IP27_NMI(0);
	nmi->magic = NMI_MAGIC;
	nmi->cb = (vaddr_t)ip27_nmi;
	nmi->cb_complement = ~nmi->cb;
	nmi->cb_arg = 0;

	/*
	 * Set up Node 0's HUB.
	 */
	IP27_LHUB_S(HUBPI_REGION_PRESENT, 0xffffffffffffffff);
	IP27_LHUB_S(HUBPI_CALIAS_SIZE, PI_CALIAS_SIZE_0);
	if (ip35) {
		IP27_RHUB_PI_S(masternasid, 1,
		    HUBPI_REGION_PRESENT, 0xffffffffffffffff);
		IP27_RHUB_PI_S(masternasid, 1,
		    HUBPI_CALIAS_SIZE, PI_CALIAS_SIZE_0);
	}

	/*
	 * Compute interrupt register address.
	 */
	xbow_intr_address = (1UL << 47) /* XIO I/O space */ |
	    (masternasid << (ip35 ? 39 : 38)) |
	    ((uint64_t)IP27_RHUB_ADDR(0, HUBPI_IR_CHANGE) -
	     IP27_NODE_IO_BASE(0)) /* HUB register offset */;

	_device_register = dksc_device_register;

#ifdef MULTIPROCESSOR
	tc_init(&ip27_hub_timecounter);
#endif
}

/*
 * Autoconf enumeration
 */

void
ip27_autoconf(struct device *parent)
{
	union {
		struct mainbus_attach_args maa;
		struct cpu_attach_args caa;
	} u;
	uint node;

	xbow_widget_id = ip27_widget_id;

	/*
	 * Attach the CPU we are running on early; other processors,
	 * if any, will get attached as they are discovered.
	 */

	bzero(&u, sizeof u);
	u.maa.maa_name = "cpu";
	u.maa.maa_nasid = currentnasid = masternasid;
	u.maa.maa_physid = IP27_LHUB_L(HUBPI_CPU_NUMBER);
	u.caa.caa_hw = &bootcpu_hwinfo;
	config_found(parent, &u, ip27_print);
	u.maa.maa_name = "clock";
	config_found(parent, &u, ip27_print);

	/*
	 * Now attach all nodes' I/O devices.
	 */

#ifdef MULTIPROCESSOR
	ip27_ncpus = 1;
#endif
	ip27_attach_node(parent, masternasid);
	for (node = 0; node < maxnodes; node++) {
		if (gda->nasid[node] < 0)
			continue;
		if (gda->nasid[node] == masternasid)
			continue;
		ip27_attach_node(parent, gda->nasid[node]);
	}
}

void
ip27_attach_node(struct device *parent, int16_t nasid)
{
	union {
		struct mainbus_attach_args maa;
		struct spdmem_attach_args saa;
	} u;
	uint dimm;
	void *match;

	currentnasid = nasid;
	bzero(&u, sizeof u);

	/*
	 * IRIX performs this extra initialization on Origin 200 systems.
	 * This seems to properly initialize on-board devices on the
	 * second node.
	 */
	if (sys_config.system_type == SGI_IP27 &&
	    sys_config.system_subtype == IP27_O200) {
		IP27_RHUB_S(nasid, HUBMDBASE_IP27 | HUBMD_LED0,
		    nasid == masternasid ? 1 : 9);
		IP27_RHUB_S(nasid, HUBMDBASE_IP27 | HUBMD_LED0,
		    nasid == masternasid ? 1 : 9);
	}

#ifdef MULTIPROCESSOR
	kl_scan_node(nasid, IP27_BC_NODE, ip27_kl_attach_cpu_board, parent);
#endif

	if (ip35) {
		l1_display(nasid, TRUE, "OpenBSD/sgi");

		u.maa.maa_name = "spdmem";
		u.maa.maa_nasid = nasid;
		for (dimm = 0; dimm < L1_SPD_DIMM_MAX; dimm++) {
			u.saa.dimm = dimm;
			/*
			 * inline config_found_sm() without printing a message
			 * if match() fails, to avoid getting
			 * ``spdmem not configured'' for empty memory slots.
			 */
			if ((match = config_search(NULL, parent, &u)) != NULL)
				config_attach(parent, match, &u, ip27_print);
		}
	}
	u.maa.maa_name = "xbow";
	u.maa.maa_nasid = nasid;
	config_found(parent, &u, ip27_print);
}

int
ip27_print(void *aux, const char *pnp)
{
	struct mainbus_attach_args *maa = aux;

	if (pnp != NULL)
		printf("%s at %s", maa->maa_name, pnp);
	printf(" nasid %d", maa->maa_nasid);

	return UNCONF;
}

/*
 * Widget mapping.
 */

paddr_t
ip27_widget_short(int16_t nasid, u_int widget)
{
	/*
	 * A hardware bug on the PI side of the Hub chip (at least in
	 * earlier versions) causes accesses to the short window #0
	 * to be unreliable.
	 * The PROM implements a workaround by remapping it to
	 * big window #6 (the last programmable big window).
	 */
	if (widget == 0)
		return ip27_widget_long(nasid, IOTTE_SWIN0);

	return ((uint64_t)(widget) << 24) |
	    ((uint64_t)(nasid) << kl_n_shift) | io_base;
}

paddr_t
ip27_widget_long(int16_t nasid, u_int window)
{
	return ((uint64_t)(window + 1) << xbow_long_shift) |
	    ((uint64_t)(nasid) << kl_n_shift) | io_base;
}

paddr_t
ip27_widget_map(int16_t nasid, u_int widget, bus_addr_t *offs, bus_size_t *len)
{
	uint tte, avail_tte;
	uint64_t iotte;
	paddr_t delta, start, end;
	int s;

	/*
	 * On Origin systems, we can only have partial views of the widget
	 * address space, due to the addressing scheme limiting each node's
	 * address space to 31 to 33 bits.
	 *
	 * The largest window is 256MB or 512MB large, depending on the
	 * mode the system is in (M/N).
	 */

	/*
	 * Round the requested range to a large window boundary.
	 */

	start = *offs;
	end = start + *len;

	start = (start >> xbow_long_shift);
	end = (end + (1 << xbow_long_shift) - 1) >> xbow_long_shift;

	/*
	 * See if an existing IOTTE covers part of the mapping we are asking
	 * for.  If so, reuse it and truncate the caller's range.
	 */

	s = splhigh();	/* XXX or disable interrupts completely? */

	avail_tte = IOTTE_MAX;
	for (tte = 0; tte < IOTTE_MAX; tte++) {
		if (tte == IOTTE_SWIN0)
			continue;

		iotte = IP27_RHUB_L(nasid, HUBIOBASE + HUBIO_IOTTE(tte));
		if (IOTTE_WIDGET(iotte) == 0) {
			if (avail_tte == IOTTE_MAX)
				avail_tte = tte;
			continue;
		}
		if (IOTTE_WIDGET(iotte) != widget)
			continue;

		if (IOTTE_OFFSET(iotte) < start ||
		    (IOTTE_OFFSET(iotte) + 1) >= end)
			continue;

		/*
		 * We found a matching IOTTE (an exact match if we asked for
		 * less than the large window size, a partial match otherwise).
		 * Reuse it (since we never unmap IOTTE at this point, there
		 * is no need to maintain a reference count).
		 */
		break;
	}

	/*
	 * If we found an unused IOTTE while searching above, program it
	 * to map the beginning of the requested range.
	 */

	if (tte == IOTTE_MAX && avail_tte != IOTTE_MAX) {
		tte = avail_tte;

		/* XXX I don't understand why it's not device space. */
		iotte = IOTTE(IOTTE_SPACE_MEMORY, widget, start);
		IP27_RHUB_S(nasid, HUBIOBASE + HUBIO_IOTTE(tte), iotte);
		(void)IP27_RHUB_L(nasid, HUBIOBASE + HUBIO_IOTTE(tte));
	}

	splx(s);

	if (tte != IOTTE_MAX) {
		delta = *offs - (start << xbow_long_shift);
		/* *offs unmodified */
		*len = (1 << xbow_long_shift) - delta;

		return ip27_widget_long(nasid, tte) + delta;
	}

	return 0UL;
}

/*
 * Widget enumeration
 */

int
ip27_widget_id(int16_t nasid, u_int widget, uint32_t *wid)
{
	paddr_t wpa;
	uint32_t id;

	if (widget != 0)
	{
		if (widget < WIDGET_MIN || widget > WIDGET_MAX)
			return EINVAL;
	}

	wpa = ip27_widget_short(nasid, widget);
	if (guarded_read_4(wpa + (WIDGET_ID | 4), &id) != 0)
		return ENXIO;

	if (wid != NULL)
		*wid = id;

	return 0;
}

/*
 * Same as the above, but usable before we can handle faults.
 * Expects the caller to only use valid widget numbers...
 */
int
ip27_widget_id_early(int16_t nasid, u_int widget, uint32_t *wid)
{
	paddr_t wpa;

	if (widget != 0)
	{
		if (widget < WIDGET_MIN || widget > WIDGET_MAX)
			return EINVAL;
	}

	wpa = ip27_widget_short(nasid, widget);
	if (wid != NULL)
		*wid = *(uint32_t *)(wpa + (WIDGET_ID | 4));

	return 0;
}

/*
 * Reboot code
 */

void
ip27_halt(int howto)
{
	uint32_t promop;
	uint node;
	uint64_t nibase, action;

	/*
	 * Even if ARCBios TLB and exception vectors are restored,
	 * returning to ARCBios doesn't work.
	 *
	 * So, instead, send a reset through the network interface
	 * of the Hub space.  Although there seems to be a way to tell
	 * the PROM which action we want it to take afterwards, it
	 * always reboots for me...
	 */

	if (howto & RB_HALT) {
#if 0
		if (howto & RB_POWERDOWN)
			promop = GDA_PROMOP_HALT;
		else
			promop = GDA_PROMOP_EIM;
#else
		if (howto & RB_POWERDOWN) {
			if (ip35) {
				l1_exec_command(masternasid, "* pwr d");
				delay(1000000);
				printf("Powerdown failed, "
				    "please switch off power manually.\n");
			} else {
				printf("Software powerdown not supported, "
				    "please switch off power manually.\n");
			}
			for (;;)
				continue;
			/* NOTREACHED */
		} else {
			printf("System halted.\n"
			    "Press any key to restart\n");
			cngetc();
			promop = GDA_PROMOP_REBOOT;
		}
#endif
	} else
		promop = GDA_PROMOP_REBOOT;

	promop |= GDA_PROMOP_MAGIC | GDA_PROMOP_NO_DIAGS |
	    GDA_PROMOP_NO_MEMINIT;

#if 0
	/*
	 * That's what one would expect, based on the gda layout...
	 */
	gda->promop = promop;
#else
	/*
	 * ...but the magic location is in a different castle.
	 * And it's not even the same between IP27 and IP35.
	 * Laugh, everyone! It's what SGI wants us to.
	 */
	if (ip35)
		IP27_LHUB_S(HUBLBBASE_IP35 + 0x8010, promop);
	else
		IP27_LHUB_S(HUBPIBASE + 0x418, promop);
#endif

	if (ip35) {
		nibase = HUBNIBASE_IP35;
		action = NI_RESET_LOCAL_IP35 | NI_RESET_ACTION_IP35;
	} else {
		nibase = HUBNIBASE_IP27;
		action = NI_RESET_LOCAL_IP27 | NI_RESET_ACTION_IP27;
	}

	/*
	 * Reset all other nodes, if present.
	 */

	for (node = 0; node < maxnodes; node++) {
		if (gda->nasid[node] < 0)
			continue;
		if (gda->nasid[node] == masternasid)
			continue;
		IP27_RHUB_S(gda->nasid[node],
		    nibase + HUBNI_RESET_ENABLE, NI_RESET_ENABLE);
		IP27_RHUB_S(gda->nasid[node],
		    nibase + HUBNI_RESET, action);
	}
	IP27_LHUB_S(nibase + HUBNI_RESET_ENABLE, NI_RESET_ENABLE);
	IP27_LHUB_S(nibase + HUBNI_RESET, action);
}

/*
 * Local HUB interrupt handling routines
 */

/*
 * Find a suitable interrupt bit for the given interrupt.
 */
int
ip27_hub_intr_register(int widget, int level, int *intrbit)
{
	u_long cpuid = cpu_number();
	int bit;

	/*
	 * Try to allocate a bit on hardware level 0 first.
	 */
	for (bit = HUBPI_INTR0_WIDGET_MAX; bit >= HUBPI_INTR0_WIDGET_MIN; bit--)
		if ((hubpi_intem[cpuid].hw[0] & (1UL << bit)) == 0)
			goto found;

	/*
	 * If all level 0 sources are in use, try to allocate a bit on
	 * level 1.
	 */
	for (bit = HUBPI_INTR1_WIDGET_MAX; bit >= HUBPI_INTR1_WIDGET_MIN; bit--)
		if ((hubpi_intem[cpuid].hw[1] & (1UL << bit)) == 0) {
			bit += HUBPI_NINTS;
			goto found;
		}

	return EINVAL;

found:
	*intrbit = bit;
	return 0;
}

/*
 * Register an interrupt handler for a given source, and enable it.
 */
int
ip27_hub_intr_establish(int (*func)(void *), void *arg, int intrbit,
    int level, const char *name, struct intrhand *ihstore)
{
	struct intrhand *ih, **anchor;
	u_long cpuid = cpu_number();
	int flags;
	int s;

#ifdef DIAGNOSTIC
	if (intrbit < 0 || intrbit >= HUBPI_NINTS + HUBPI_NINTS)
		return EINVAL;
#endif

	flags = (level & IPL_MPSAFE) ? IH_MPSAFE : 0;
	level &= ~IPL_MPSAFE;

	/*
	 * Widget interrupts are not supposed to be shared - the interrupt
	 * mask is supposedly large enough for all interrupt sources.
	 *
	 * XXX On systems with many widgets and/or nodes, this assumption
	 * XXX will no longer stand; we'll need to implement interrupt
	 * XXX sharing at some point.
	 */
	if (intrbit >= HUBPI_NINTS)
		anchor = &hubpi_intrhand1[intrbit % HUBPI_NINTS];
	else
		anchor = &hubpi_intrhand0[intrbit];
	if (*anchor != NULL)
		return EEXIST;

	if (ihstore == NULL) {
		ih = malloc(sizeof(*ih), M_DEVBUF, M_NOWAIT);
		if (ih == NULL)
			return ENOMEM;
		flags |= IH_ALLOCATED;
	} else
		ih = ihstore;

	ih->ih_next = NULL;
	ih->ih_fun = func;
	ih->ih_arg = arg;
	ih->ih_level = level;
	ih->ih_irq = intrbit;
	ih->ih_flags = flags;
	if (name != NULL)
		evcount_attach(&ih->ih_count, name, &ih->ih_level);

	s = splhigh();

	*anchor = ih;

	hubpi_intem[cpuid].hw[intrbit / HUBPI_NINTS] |=
	    1UL << (intrbit % HUBPI_NINTS);
	if (intrbit / HUBPI_NINTS != 0)
		ip27_hub_intr_makemasks1();
	else
		ip27_hub_intr_makemasks0();

	splx(s);	/* causes hw mask update */

	return 0;
}

void
ip27_hub_intr_disestablish(int intrbit)
{
	struct intrhand *ih, **anchor;
	u_long cpuid = cpu_number();
	int s;

#ifdef DIAGNOSTIC
	if (intrbit < 0 || intrbit >= HUBPI_NINTS + HUBPI_NINTS)
		return;
#endif

	if (intrbit >= HUBPI_NINTS)
		anchor = &hubpi_intrhand1[intrbit % HUBPI_NINTS];
	else
		anchor = &hubpi_intrhand0[intrbit];

	s = splhigh();

	if ((ih = *anchor) == NULL) {
		splx(s);
		return;
	}

	*anchor = NULL;

	hubpi_intem[cpuid].hw[intrbit / HUBPI_NINTS] &=
	    ~(1UL << (intrbit % HUBPI_NINTS));
	if (intrbit / HUBPI_NINTS != 0)
		ip27_hub_intr_makemasks1();
	else
		ip27_hub_intr_makemasks0();

	splx(s);

	if (ISSET(ih->ih_flags, IH_ALLOCATED))
		free(ih, M_DEVBUF, sizeof *ih);
}

void
ip27_hub_intr_clear(int intrbit)
{
	struct cpu_info *ci = curcpu();

	IP27_RHUB_PI_S(ci->ci_nasid, IP27_SLICE_SUBNODE(ci->ci_slice),
	    HUBPI_IR_CHANGE, PI_IR_CLR | intrbit);
	(void)IP27_RHUB_PI_L(ci->ci_nasid, IP27_SLICE_SUBNODE(ci->ci_slice),
	    HUBPI_IR0);
}

void
ip27_hub_intr_set(int intrbit)
{
	struct cpu_info *ci = curcpu();

	IP27_RHUB_PI_S(ci->ci_nasid, IP27_SLICE_SUBNODE(ci->ci_slice),
	    HUBPI_IR_CHANGE, PI_IR_SET | intrbit);
}

void
ip27_hub_splx(int newipl)
{
	struct cpu_info *ci = curcpu();

	/* Update masks to new ipl. Order highly important! */
	__asm__ (".set noreorder\n");
	ci->ci_ipl = newipl;
	mips_sync();
	__asm__ (".set reorder\n");
	ip27_hub_setintrmask(newipl);

	/* If we still have softints pending trigger processing. */
	if (ci->ci_softpending && newipl < IPL_SOFTINT)
		setsoftintr0();
}

/*
 * Level 0 and level 1 interrupt dispatchers.
 */

#define	INTR_FUNCTIONNAME	hubpi_intr0
#define	MASK_FUNCTIONNAME	ip27_hub_intr_makemasks0
#define	INTR_LOCAL_DECLS
#define	MASK_LOCAL_DECLS \
	struct cpu_info *ci = curcpu();
#define	INTR_GETMASKS \
do { \
	isr = IP27_LHUB_L(HUBPI_IR0); \
	imr = IP27_LHUB_L(IP27_SLICE_LCPU(ci->ci_slice) == 0 ? \
	    HUBPI_CPU0_IMR0 : HUBPI_CPU1_IMR0); \
	bit = HUBPI_INTR0_WIDGET_MAX; \
} while (0)
#define	INTR_MASKPENDING \
do { \
	IP27_LHUB_S(IP27_SLICE_LCPU(ci->ci_slice) == 0 ? \
	    HUBPI_CPU0_IMR0 : HUBPI_CPU1_IMR0, imr & ~isr); \
	(void)IP27_LHUB_L(HUBPI_IR0); \
} while (0)
#define	INTR_IMASK(ipl)		hubpi_imask[ci->ci_cpuid][ipl].hw[0]
#ifdef MULTIPROCESSOR
#define	INTR_IPI_HOOK(ipl) \
do { \
	unsigned long ipibit = IP27_SLICE_IPI(ci->ci_slice); \
	unsigned long ipimask = 1 << ipibit; \
	if ((isr & ipimask) && \
	    !(hubpi_imask[ci->ci_cpuid][ipl].hw[0] & ipimask)) { \
		struct intrhand *ih = hubpi_intrhand0[ipibit]; \
		ih->ih_fun(ih->ih_arg); \
		isr &= ~ipimask; \
	} \
} while (0)
#endif /* MULTIPROCESSOR */
#define	INTR_HANDLER(bit)	hubpi_intrhand0[bit]
#define	INTR_SPURIOUS(bit) \
do { \
	printf("spurious interrupt, source %d\n", bit); \
} while (0)
#define	INTR_MASKRESTORE \
do { \
	IP27_LHUB_S(IP27_SLICE_LCPU(ci->ci_slice) == 0 ? \
	    HUBPI_CPU0_IMR0 : HUBPI_CPU1_IMR0, imr); \
	(void)IP27_LHUB_L(HUBPI_IR0); \
} while (0)
#define	INTR_MASKSIZE	HUBPI_NINTS

#include <sgi/sgi/intr_template.c>

#define	INTR_FUNCTIONNAME	hubpi_intr1
#define	MASK_FUNCTIONNAME	ip27_hub_intr_makemasks1
#define	INTR_LOCAL_DECLS
#define	MASK_LOCAL_DECLS \
	struct cpu_info *ci = curcpu();
#define	INTR_GETMASKS \
do { \
	isr = IP27_LHUB_L(HUBPI_IR1); \
	imr = IP27_LHUB_L(IP27_SLICE_LCPU(ci->ci_slice) == 0 ? \
	    HUBPI_CPU0_IMR1 : HUBPI_CPU1_IMR1); \
	bit = HUBPI_INTR1_WIDGET_MAX; \
} while (0)
#define	INTR_MASKPENDING \
do { \
	IP27_LHUB_S(IP27_SLICE_LCPU(ci->ci_slice) == 0 ? \
	    HUBPI_CPU0_IMR1 : HUBPI_CPU1_IMR1, imr & ~isr); \
	(void)IP27_LHUB_L(HUBPI_IR1); \
} while (0)
#define	INTR_IMASK(ipl)		hubpi_imask[ci->ci_cpuid][ipl].hw[1]
#define	INTR_HANDLER(bit)	hubpi_intrhand1[bit]
#define	INTR_SPURIOUS(bit) \
do { \
	printf("spurious interrupt, source %d\n", bit + HUBPI_NINTS); \
} while (0)
#define	INTR_MASKRESTORE \
do { \
	IP27_LHUB_S(IP27_SLICE_LCPU(ci->ci_slice) == 0 ? \
	    HUBPI_CPU0_IMR1 : HUBPI_CPU1_IMR1, imr); \
	(void)IP27_LHUB_L(HUBPI_IR1); \
} while (0)
#define	INTR_MASKSIZE	HUBPI_NINTS

#include <sgi/sgi/intr_template.c>

void
ip27_hub_setintrmask(int level)
{
	struct cpu_info *ci = curcpu();
	u_long imr0, imr1;

	if (IP27_SLICE_LCPU(ci->ci_slice) == 0) {
		imr0 = HUBPI_CPU0_IMR0;
		imr1 = HUBPI_CPU0_IMR1;
	} else {
		imr0 = HUBPI_CPU1_IMR0;
		imr1 = HUBPI_CPU1_IMR1;
	}

	IP27_LHUB_S(imr0, hubpi_intem[ci->ci_cpuid].hw[0] &
	    ~hubpi_imask[ci->ci_cpuid][level].hw[0]);
	(void)IP27_LHUB_L(HUBPI_IR0);
	IP27_LHUB_S(imr1, hubpi_intem[ci->ci_cpuid].hw[1] &
	    ~hubpi_imask[ci->ci_cpuid][level].hw[1]);
	(void)IP27_LHUB_L(HUBPI_IR1);
}

void
ip27_nmi(void *arg)
{
	vaddr_t regs_offs;
	register_t *regs, epc;
	struct trapframe nmi_frame;
	extern int db_ktrap(int, struct trapframe *);

	/*
	 * Build a ddb frame from the registers saved in the NMI KREGS
	 * area.
	 */

	if (ip35)
		regs_offs = IP35_NMI_KREGS_BASE;	/* XXX assumes cpu0 */
	else
		regs_offs = IP27_NMI_KREGS_BASE;	/* XXX assumes cpu0 */
	regs = IP27_UNCAC_ADDR(register_t *, 0, regs_offs);

	memset(&nmi_frame, 0xff, sizeof nmi_frame);
	
	/* general registers */
	memcpy(&nmi_frame.zero, regs, 32 * sizeof(register_t));
	regs += 32;
	nmi_frame.sr = *regs++;		/* COP_0_STATUS_REG */
	nmi_frame.cause = *regs++;	/* COP_0_CAUSE_REG */
	nmi_frame.pc = *regs++;
	nmi_frame.badvaddr = *regs++;	/* COP_0_BAD_VADDR */
	epc = *regs++;			/* COP_0_EXC_PC */
	regs++;				/* COP_0_CACHE_ERR */
	regs++;				/* NMI COP_0_STATUS_REG */

	setsr(getsr() & ~SR_BOOT_EXC_VEC);
	printf("NMI, PC = %p RA = %p SR = %08lx EPC = %p\n",
	    (void *)nmi_frame.pc, (void *)nmi_frame.ra, nmi_frame.sr,
	    (void *)epc);
#ifdef DDB
	(void)db_ktrap(-1, &nmi_frame);
#endif
	panic("NMI");
	/* NOTREACHED */
}

#ifdef MULTIPROCESSOR

int
ip27_kl_launch_cpu(klinfo_t *comp, void *arg)
{
	struct cpu_info *ci = curcpu();

	/* Skip the running CPU. */
	if (comp->nasid == ci->ci_nasid &&
	    comp->physid == ci->ci_slice)
		return 0;

	/* XXX Skip CPUs on other nodes. */
	if (comp->nasid != ci->ci_nasid)
		return 0;

	if (ip27_ncpus >= MAXCPUS)
		return 0;

	ip27_prom_launch_slave(comp->nasid, comp->physid,
	    ip27_cpu_spinup_trampoline,
	    SPINUP_TICKET(comp->nasid, comp->physid), 0, 0);
	ip27_ncpus++;

	return 0;
}

int
ip27_kl_launch_cpu_board(lboard_t *board, void *arg)
{
	kl_scan_board(board, KLSTRUCT_CPU, ip27_kl_launch_cpu, arg);
	return 0;
}

int
ip27_kl_attach_cpu(klinfo_t *comp, void *arg)
{
	struct cpu_attach_args caa;
	struct cpu_hwinfo hw;
	struct cpu_info *ci = curcpu();
	struct device *parent = arg;
	klcpu_t *cpucomp;

	/* Skip the running CPU. */
	if (comp->nasid == ci->ci_nasid &&
	    comp->physid == ci->ci_slice)
		return 0;

	/* XXX Skip CPUs on other nodes. */
	if (comp->nasid != ci->ci_nasid)
		return 0;

	if (ip27_ncpus >= MAXCPUS)
		return 0;

	cpucomp = (klcpu_t *)comp;

	hw.c0prid = cpucomp->cpu_prid;
	hw.c1prid = cpucomp->cpu_prid;
	hw.clock = cpucomp->cpu_speed * 1000000;
	hw.tlbsize = 64;
	hw.type = (cpucomp->cpu_prid >> 8) & 0xff;

	caa.caa_maa.maa_name = "cpu";
	caa.caa_maa.maa_nasid = comp->nasid;
	caa.caa_maa.maa_physid = comp->physid;
	caa.caa_hw = &hw;
	config_found(parent, &caa, ip27_print);
	ip27_ncpus++;

	return 0;
}

int
ip27_kl_attach_cpu_board(lboard_t *board, void *arg)
{
	kl_scan_board(board, KLSTRUCT_CPU, ip27_kl_attach_cpu, arg);
	return 0;
}

uint
ip27_hub_get_timecount(struct timecounter *tc)
{
	return IP27_RHUB_L(masternasid, HUBPI_RT_COUNT);
}

void
hw_ipi_intr_set(u_long cpuid)
{
	struct cpu_info *ci = get_cpu_info(cpuid);
	int intr;

	intr = IP27_SLICE_IPI(ci->ci_slice);
	IP27_RHUB_PI_S(ci->ci_nasid, IP27_SLICE_SUBNODE(ci->ci_slice),
	    HUBPI_IR_CHANGE, PI_IR_SET | intr);
}

void
hw_ipi_intr_clear(u_long cpuid)
{
	struct cpu_info *ci = get_cpu_info(cpuid);
	int intr;

	intr = IP27_SLICE_IPI(ci->ci_slice);
	IP27_RHUB_PI_S(ci->ci_nasid, IP27_SLICE_SUBNODE(ci->ci_slice),
	    HUBPI_IR_CHANGE, PI_IR_CLR | intr);
	(void)IP27_RHUB_PI_L(ci->ci_nasid, IP27_SLICE_SUBNODE(ci->ci_slice),
	    HUBPI_IR0);
}

int
hw_ipi_intr_establish(int (*func)(void *), u_long cpuid)
{
	struct cpu_info *ci = get_cpu_info(cpuid);
	int intr;

	intr = IP27_SLICE_IPI(ci->ci_slice);
	return ip27_hub_intr_establish(func, (void *)cpuid, intr, IPL_IPI,
	    NULL, &ci->ci_ipiih);
}

void
hw_cpu_hatch(struct cpu_info *ci)
{
	int s;

	setcurcpu(ci);

	/*
	 * Make sure we can access the extended address space.
	 * Note that r10k and later do not allow XUSEG accesses
	 * from kernel mode unless SR_UX is set.
	 */
	setsr(getsr() | SR_KX | SR_UX);

	tlb_init(64);
	tlb_set_pid(0);

	/*
	 * Turn off bootstrap exception vectors.
	 */
	setsr(getsr() & ~SR_BOOT_EXC_VEC);

	/*
	 * Clear out the I and D caches.
	 */
	Mips10k_ConfigCache(ci);
	Mips_SyncCache(ci);

	printf("cpu%lu launched\n", cpu_number());

	(*md_startclock)(ci);

	ncpus++;
	cpuset_add(&cpus_running, ci);

	mips64_ipi_init();
	ip27_hub_setintrmask(0);

	spl0();
	(void)updateimask(0);

	SCHED_LOCK(s);
	cpu_switchto(NULL, sched_chooseproc());
}

void
hw_cpu_boot_secondary(struct cpu_info *ci)
{
	vaddr_t kstack;

	kstack = alloc_contiguous_pages(USPACE);
	if (kstack == 0)
		panic("unable to allocate idle stack\n");

	__asm__ (".set noreorder\n");
	ci->ci_curprocpaddr = (void *)kstack;
	ip27_spinup_a0 = (uint64_t)ci;
	ip27_spinup_sp = (uint64_t)(kstack + USPACE);
	mips_sync();
	__asm__ (".set reorder\n");
	ip27_spinup_turn = SPINUP_TICKET(ci->ci_nasid, ci->ci_slice);

	while (!cpuset_isset(&cpus_running, ci))
		;
}

#endif /* MULTIPROCESSOR */
