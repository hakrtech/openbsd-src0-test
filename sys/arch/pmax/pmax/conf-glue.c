/*
 * conf-glue.c:
 * A hand-edited ioconf.c, as generated by config.old program
 *
 * Seriously munged  to support old-stype pmax configurations
 * configured and compiled with new config.
 * Since new config doesn't yet support most pmax drivers,
 * this file contains an driver configuration table produced
 * by config.old run on a `generic' configuration.
 * This table uses device counts produced by the new config
 * ``needs-count'' keyword, that tells us at compile time
 * the maximum number of each device  that were configured.
 *
 * It also  provides a dummy set of cfdriver  structures,
 * for devices present in the configuration file, but
 * whose drivers don't (yet) have a struct cfdriver.
 * This avoids link-time errors with unresolved references
 * from the `parent table' produced by new-style config,
 * which assumes all device drivers have a struct cfdriver.
 *
 * THIS MAY NOT WORK FOR ALL MACHINES.
 */

#include <sys/param.h>
#include <sys/types.h>
#include <sys/device.h>
#include <sys/buf.h>
#include <sys/dkstat.h>


#include <machine/autoconf.h>
#include <pmax/dev/device.h>

#define C (char *)

/*
 * Get old-style count of how many devices were configured
 * for each driver.
 */
#include "asc.h"
#include "sii.h"
#include "rz.h"
#include "tz.h"


/* declarations for glue to 4.4bsd pmax port SCSI drivers and autoconfig */
#if NASC > 0
extern struct pmax_driver ascdriver;
#endif
#if NSII > 0
extern struct pmax_driver siidriver;
#endif
#if NRZ > 0
extern struct pmax_driver rzdriver;
#endif
#if NTZ > 0
extern struct pmax_driver tzdriver;
#endif


/*
 * A fixed-at-compile-time disk configuration for the pmax-specific
 * scsi code.
 *
 * If you want to change this, fix the pmax kernel to use new-style
 * config code and the machine-independent NetBSD scsi drivers.
 */

struct pmax_scsi_device scsi_dinit[] = {
/*driver,	cdriver,	unit,	ctlr,	drive,	slave,	dk,	flags*/
{ &rzdriver,	&siidriver,	0,	0,	0,	0,	1,	0x0 },
{ &rzdriver,	&siidriver,	1,	0,	1,	0,	1,	0x0 },
{ &rzdriver,	&siidriver,	2,	0,	2,	0,	1,	0x0 },
{ &rzdriver,	&siidriver,	3,	0,	3,	0,	1,	0x0 },
{ &rzdriver,	&siidriver,	4,	0,	4,	0,	1,	0x0 },
{ &tzdriver,	&siidriver,	0,	0,	5,	0,	0,	0x0 },
{ &tzdriver,	&siidriver,	1,	0,	6,	0,	0,	0x0 },
{ &rzdriver,	&ascdriver,	0,	0,	0,	0,	1,	0x0 },
{ &rzdriver,	&ascdriver,	1,	0,	1,	0,	1,	0x0 },
{ &rzdriver,	&ascdriver,	2,	0,	2,	0,	1,	0x0 },
{ &rzdriver,	&ascdriver,	3,	0,	3,	0,	1,	0x0 },
{ &rzdriver,	&ascdriver,	4,	0,	4,	0,	1,	0x0 },
{ &tzdriver,	&ascdriver,	0,	0,	5,	0,	0,	0x0 },
{ &tzdriver,	&ascdriver,	1,	0,	6,	0,	0,	0x0 },
0
};


int	nomatch  __P((struct device * parent, void *cfdata, void *aux));
void	noattach __P((struct device *parent, struct device *self, void *aux));


/* placeholder definitions for new-style scsi bus/disk/tape drivers */

struct cfdriver oldscsibuscd = {NULL, "", nomatch, noattach, DV_DULL, 0, 0};

struct cfdriver rzcd	= {NULL, "rz", nomatch, noattach, DV_DULL, 0, 0};
struct cfdriver tzcd	= {NULL, "tz", nomatch, noattach, DV_DULL, 0, 0};


#define MAX_SCSI 4
static int nscsi;
static struct pmax_ctlr pmax_scsi_table[MAX_SCSI+1] = {
/*	driver,		unit,	addr,		flags */

	{ NULL, },  { NULL, }, { NULL, }, { NULL, },
	{ NULL, } /* sentinel */
};

/*
 * Callback for scsi controllers to register themselves with this
 * config glue.  Construct an old-style pmax autoconfiguration
 * SCSI-driver table entry for a  DECstation SCSI controller, and add it
 * to the table of known SCSI drivers.  Needed for old-style pmax
 * SCSI-bus probing.  configure() will call us back to probe
 * each known controller for the statically-configured drives, above.
 */
void
pmax_add_scsi(dp, unit)
	struct pmax_driver *dp;
	int unit;
{
	struct pmax_ctlr *cp  = &pmax_scsi_table[nscsi++];
	if (nscsi > MAX_SCSI) {
		panic("Too many old-style SCSI adaptors\n");
	}
	cp->pmax_driver = dp;
	cp->pmax_unit = unit;
}

/*
 * Configure scsi devices on old-style pmax scsi drivers.
 * Interrupts must be enabled or this will hang.
 *
 * Called by configure() after all possible controllers have been
 * found.  The controllers really should invoke new-style
 * autoconfiguration on themselves, probing their SCSI buses,
 * but the pmax drivers don't yet have polled SCSI.
 */
void
configure_scsi()
{
	register struct pmax_ctlr *cp;
	register struct pmax_scsi_device *dp;
	register struct pmax_driver *drp;

	/* probe and initialize SCSI buses */
	for (cp = &pmax_scsi_table[0]; drp = cp->pmax_driver; cp++) {

		/* probe and initialize devices connected to controller */
		for (dp = scsi_dinit; drp = dp->sd_driver; dp++) {
			/* might want to get fancier later */
			if (dp->sd_cdriver != cp->pmax_driver ||
			    dp->sd_ctlr != cp->pmax_unit)
			    continue;	/* not connected */
			if (!(*drp->d_init)(dp))
			    continue;
			dp->sd_alive = 1;
			/* if device is a disk, assign number for statistics */
			if (dp->sd_dk && dkn < DK_NDRIVE)
			    dp->sd_dk = dkn++;
			else
			    dp->sd_dk = -1;
		}
	}
}


/*
 * Match function in struct cfdriver of old-conf drivers: never matches.
 */
int
nomatch(parent, cfdata, aux)
	struct device *parent;
	void *cfdata;
	void *aux;
{
	struct cfdata *cf = cfdata;
	struct confargs *ca = aux;
#if /*def DEBUG*/ 0
	printf("nomatch  %s: %s: %s offset 0x%lx not yet done: %x\n",
	        parent->dv_cfdata->cf_driver->cd_name,
	       parent->dv_xname,
	       ca->ca_name, ca->ca_offset);
#endif
	return 0;
}


/*
 * Attach function in struct cfdriver of old-conf drivers: never called.
 */
void
noattach(parent, self, aux)
	struct device *parent;
	struct device *self;
	void *aux;
{
	struct confargs *ca = aux;

	/*XXX*/
#ifdef DEBUG
	printf("new attach  %s%d from %s: not yet done\n",
	       ca->ca_name, self->dv_unit,
	       parent->dv_xname);
#else
	panic("Can't do ew-attach of old device %s\n",
	      ca->ca_name, self->dv_unit);
#endif
	return;
}

