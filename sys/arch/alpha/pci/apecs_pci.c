/*	$NetBSD: apecs_pci.c,v 1.6 1996/04/12 06:08:09 cgd Exp $	*/

/*
 * Copyright (c) 1995, 1996 Carnegie-Mellon University.
 * All rights reserved.
 *
 * Author: Chris G. Demetriou
 * 
 * Permission to use, copy, modify and distribute this software and
 * its documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS" 
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND 
 * FOR ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 *
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 *
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/device.h>
#include <vm/vm.h>

#include <dev/pci/pcireg.h>
#include <dev/pci/pcivar.h>
#include <alpha/pci/apecsreg.h>
#include <alpha/pci/apecsvar.h>

void		apecs_attach_hook __P((struct device *, struct device *,
		    struct pcibus_attach_args *));
int		apecs_bus_maxdevs __P((void *, int));
pcitag_t	apecs_make_tag __P((void *, int, int, int));
void		apecs_decompose_tag __P((void *, pcitag_t, int *, int *,
		    int *));
pcireg_t	apecs_conf_read __P((void *, pcitag_t, int));
void		apecs_conf_write __P((void *, pcitag_t, int, pcireg_t));

void
apecs_pci_init(pc, v)
	pci_chipset_tag_t pc;
	void *v;
{

	pc->pc_conf_v = v;
	pc->pc_attach_hook = apecs_attach_hook;
	pc->pc_bus_maxdevs = apecs_bus_maxdevs;
	pc->pc_make_tag = apecs_make_tag;
	pc->pc_decompose_tag = apecs_decompose_tag;
	pc->pc_conf_read = apecs_conf_read;
	pc->pc_conf_write = apecs_conf_write;
}

void
apecs_attach_hook(parent, self, pba)
	struct device *parent, *self;
	struct pcibus_attach_args *pba;
{
}

int
apecs_bus_maxdevs(cpv, busno)
	void *cpv;
	int busno;
{

	return 32;
}

pcitag_t
apecs_make_tag(cpv, b, d, f)
	void *cpv;
	int b, d, f;
{

	return (b << 16) | (d << 11) | (f << 8);
}

void
apecs_decompose_tag(cpv, tag, bp, dp, fp)
	void *cpv;
	pcitag_t tag;
	int *bp, *dp, *fp;
{

	if (bp != NULL)
		*bp = (tag >> 16) & 0xff;
	if (dp != NULL)
		*dp = (tag >> 11) & 0x1f;
	if (fp != NULL)
		*fp = (tag >> 8) & 0x7;
}

pcireg_t
apecs_conf_read(cpv, tag, offset)
	void *cpv;
	pcitag_t tag;
	int offset;
{
	struct apecs_config *acp = cpv;
	pcireg_t *datap, data;
	int s, secondary, ba;
	int32_t old_haxr2;					/* XXX */

	/* secondary if bus # != 0 */
	pci_decompose_tag(&acp->ac_pc, tag, &secondary, 0, 0);
	if (secondary) {
		s = splhigh();
		old_haxr2 = REGVAL(EPIC_HAXR2);
		wbflush();
		REGVAL(EPIC_HAXR2) = old_haxr2 | 0x1;
		wbflush();
	}

	datap = (pcireg_t *)phystok0seg(APECS_PCI_CONF |
	    tag << 5UL |					/* XXX */
	    (offset & ~0x03) << 5 |				/* XXX */
	    0 << 5 |						/* XXX */
	    0x3 << 3);						/* XXX */
	data = (pcireg_t)-1;
	if (!(ba = badaddr(datap, sizeof *datap)))
		data = *datap;

	if (secondary) {
		wbflush();
		REGVAL(EPIC_HAXR2) = old_haxr2;
		wbflush();
		splx(s);
	}

#if 0
	printf("apecs_conf_read: tag 0x%lx, reg 0x%lx -> %x @ %p%s\n", tag, reg,
	    data, datap, ba ? " (badaddr)" : "");
#endif

	return data;
}

void
apecs_conf_write(cpv, tag, offset, data)
	void *cpv;
	pcitag_t tag;
	int offset;
	pcireg_t data;
{
	struct apecs_config *acp = cpv;
	pcireg_t *datap;
	int s, secondary;
	int32_t old_haxr2;					/* XXX */

	/* secondary if bus # != 0 */
	pci_decompose_tag(&acp->ac_pc, tag, &secondary, 0, 0);
	if (secondary) {
		s = splhigh();
		old_haxr2 = REGVAL(EPIC_HAXR2);
		wbflush();
		REGVAL(EPIC_HAXR2) = old_haxr2 | 0x1;
		wbflush();
	}

	datap = (pcireg_t *)phystok0seg(APECS_PCI_CONF |
	    tag << 5UL |					/* XXX */
	    (offset & ~0x03) << 5 |				/* XXX */
	    0 << 5 |						/* XXX */
	    0x3 << 3);						/* XXX */
	*datap = data;

	if (secondary) {
		wbflush();
		REGVAL(EPIC_HAXR2) = old_haxr2;	
		wbflush();
		splx(s);
	}

#if 0
	printf("apecs_conf_write: tag 0x%lx, reg 0x%lx -> 0x%x @ %p\n", tag,
	    reg, data, datap);
#endif
}
