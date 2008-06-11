/*	$OpenBSD: disksubr.c,v 1.74 2008/06/11 12:35:43 deraadt Exp $	*/
/*	$NetBSD: disksubr.c,v 1.16 1996/04/28 20:25:59 thorpej Exp $ */

/*
 * Copyright (c) 1994, 1995 Gordon W. Ross
 * Copyright (c) 1994 Theo de Raadt
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
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/buf.h>
#include <sys/ioccom.h>
#include <sys/device.h>
#include <sys/disklabel.h>
#include <sys/disk.h>
#include <sys/dkbad.h>

#include <machine/cpu.h>
#include <machine/autoconf.h>
#include <dev/sun/disklabel.h>

#if defined(SUN4)
#include <machine/oldmon.h>
#endif

#include "cd.h"

static	char *disklabel_sun_to_bsd(struct sun_disklabel *, struct disklabel *);
static	int disklabel_bsd_to_sun(struct disklabel *, struct sun_disklabel *);
static __inline u_int sun_extended_sum(struct sun_disklabel *, void *);

#if NCD > 0
extern void cdstrategy(struct buf *);
#endif

/*
 * Attempt to read a disk label from a device
 * using the indicated strategy routine.
 * The label must be partly set up before this:
 * secpercyl, secsize and anything required for a block i/o read
 * operation in the driver's strategy/start routines
 * must be filled in before calling us.
 *
 * Return buffer for use in signalling errors if requested.
 *
 * Returns null on success and an error string on failure.
 */
char *
readdisklabel(dev_t dev, void (*strat)(struct buf *),
    struct disklabel *lp, int spoofonly)
{
	struct sun_disklabel *slp;
	struct buf *bp = NULL;
	char *msg;

	if ((msg = initdisklabel(lp)))
		goto done;

	/*
	 * On sparc64 we check for a CD label first, because our
	 * CD install media contains both sparc & sparc64 labels.
	 * We want the sparc64 machine to find the "CD label", not
	 * the SunOS label, for loading it's kernel.
	 */
#if NCD > 0
	if (strat == cdstrategy) {
#if defined(CD9660)
		if (iso_disklabelspoof(dev, strat, lp) == 0)
			goto done;
#endif
#if defined(UDF)
		if (udf_disklabelspoof(dev, strat, lp) == 0)
			goto done;
#endif
	}
#endif /* NCD > 0 */

	/* get a buffer and initialize it */
	bp = geteblk((int)lp->d_secsize);
	bp->b_dev = dev;

	if (spoofonly)
		goto doslabel;

	bp->b_blkno = LABELSECTOR;
	bp->b_bcount = lp->d_secsize;
	bp->b_flags = B_BUSY | B_READ;
	(*strat)(bp);
	if (biowait(bp)) {
		msg = "disk label read error";
		goto done;
	}

	slp = (struct sun_disklabel *)bp->b_data;
	if (slp->sl_magic == SUN_DKMAGIC) {
		msg = disklabel_sun_to_bsd(slp, lp);
		goto done;
	}

	msg = checkdisklabel(bp->b_data + LABELOFFSET, lp);
	if (msg == NULL)
		goto done;

doslabel:
	msg = readdoslabel(bp, strat, lp, NULL, spoofonly);
	if (msg == NULL)
		goto done;

	/* A CD9660/UDF label may be on a non-CD drive, so recheck */
#if defined(CD9660)
	if (iso_disklabelspoof(dev, strat, lp) == 0) {
		msg = NULL;
		goto done;
	}
#endif
#if defined(UDF)
	if (udf_disklabelspoof(dev, strat, lp) == 0) {
		msg = NULL;
		goto done;
	}
#endif

done:
	if (bp) {
		bp->b_flags |= B_INVAL;
		brelse(bp);
	}
	return (msg);
}

/*
 * Write disk label back to device after modification.
 */
int
writedisklabel(dev_t dev, void (*strat)(struct buf *), struct disklabel *lp)
{
	struct buf *bp = NULL;
	int error;

	/* get a buffer and initialize it */
	bp = geteblk((int)lp->d_secsize);
	bp->b_dev = dev;

	error = disklabel_bsd_to_sun(lp, (struct sun_disklabel *)bp->b_data);
	if (error)
		goto done;

	/* Write out the updated label. */
	bp->b_blkno = LABELSECTOR;
	bp->b_bcount = lp->d_secsize;
	bp->b_flags = B_BUSY | B_WRITE;
	(*strat)(bp);
	error = biowait(bp);

done:
	if (bp) {
		bp->b_flags |= B_INVAL;
		brelse(bp);
	}
	return (error);
}

/************************************************************************
 *
 * The rest of this was taken from arch/sparc/scsi/sun_disklabel.c
 * and then substantially rewritten by Gordon W. Ross
 *
 ************************************************************************/

/* What partition types to assume for Sun disklabels: */
static u_char
sun_fstypes[16] = {
	FS_BSDFFS,	/* a */
	FS_SWAP,	/* b */
	FS_UNUSED,	/* c - whole disk */
	FS_BSDFFS,	/* d */
	FS_BSDFFS,	/* e */
	FS_BSDFFS,	/* f */
	FS_BSDFFS,	/* g */
	FS_BSDFFS,	/* h */
	FS_BSDFFS,	/* i */
	FS_BSDFFS,	/* j */
	FS_BSDFFS,	/* k */
	FS_BSDFFS,	/* l */
	FS_BSDFFS,	/* m */
	FS_BSDFFS,	/* n */
	FS_BSDFFS,	/* o */
	FS_BSDFFS	/* p */
};

/*
 * Given a struct sun_disklabel, assume it has an extended partition
 * table and compute the correct value for sl_xpsum.
 */
static __inline u_int
sun_extended_sum(struct sun_disklabel *sl, void *end)
{
	u_int sum, *xp, *ep;

	xp = (u_int *)&sl->sl_xpmag;
	ep = (u_int *)end;

	sum = 0;
	for (; xp < ep; xp++)
		sum += *xp;
	return (sum);
}

/*
 * Given a SunOS disk label, set lp to a BSD disk label.
 * Returns NULL on success, else an error string.
 *
 * The BSD label is cleared out before this is called.
 */
char *
disklabel_sun_to_bsd(struct sun_disklabel *sl, struct disklabel *lp)
{
	struct sun_preamble *preamble = (struct sun_preamble *)sl;
	struct sun_partinfo *ppp;
	struct sun_dkpart *spp;
	struct partition *npp;
	u_short cksum = 0, *sp1, *sp2;
	int i, secpercyl;

	/* Verify the XOR check. */
	sp1 = (u_short *)sl;
	sp2 = (u_short *)(sl + 1);
	while (sp1 < sp2)
		cksum ^= *sp1++;
	if (cksum != 0)
		return ("SunOS disk label, bad checksum");

	/* Format conversion. */
	lp->d_magic = DISKMAGIC;
	lp->d_magic2 = DISKMAGIC;
	lp->d_flags = D_VENDOR;
	memcpy(lp->d_packname, sl->sl_text, sizeof(lp->d_packname));

	lp->d_secsize = 512;
	lp->d_nsectors = sl->sl_nsectors;
	lp->d_ntracks = sl->sl_ntracks;
	lp->d_ncylinders = sl->sl_ncylinders;

	secpercyl = sl->sl_nsectors * sl->sl_ntracks;
	lp->d_secpercyl = secpercyl;
	if (DL_GETDSIZE(lp) == 0)
		DL_SETDSIZE(lp, (daddr64_t)secpercyl * sl->sl_ncylinders);
	lp->d_version = 1;

	lp->d_sparespercyl = sl->sl_sparespercyl;
	lp->d_acylinders = sl->sl_acylinders;
	lp->d_rpm = sl->sl_rpm;
	lp->d_interleave = sl->sl_interleave;

	lp->d_npartitions = MAXPARTITIONS;
	/* These are as defined in <ufs/ffs/fs.h> */
	lp->d_bbsize = 8192;	/* XXX */
	lp->d_sbsize = 8192;	/* XXX */

	for (i = 0; i < 8; i++) {
		spp = &sl->sl_part[i];
		npp = &lp->d_partitions[i];
		DL_SETPOFFSET(npp, spp->sdkp_cyloffset * secpercyl);
		DL_SETPSIZE(npp, spp->sdkp_nsectors);
		if (DL_GETPSIZE(npp) == 0) {
			npp->p_fstype = FS_UNUSED;
		} else {
			npp->p_fstype = sun_fstypes[i];
			if (npp->p_fstype == FS_BSDFFS) {
				/*
				 * The sun label does not store the FFS fields,
				 * so just set them with default values here.
				 */
				npp->p_fragblock =
				    DISKLABELV1_FFS_FRAGBLOCK(2048, 8);
				npp->p_cpg = 16;
			}
		}
	}

	/* Clear "extended" partition info, tentatively */
	for (i = 0; i < SUNXPART; i++) {
		npp = &lp->d_partitions[i+8];
		DL_SETPOFFSET(npp, 0);
		DL_SETPSIZE(npp, 0);
		npp->p_fstype = FS_UNUSED;
	}

	/* Check to see if there's an "extended" partition table
	 * SL_XPMAG partitions had checksums up to just before the
	 * (new) sl_types variable, while SL_XPMAGTYP partitions have
	 * checksums up to the just before the (new) sl_xxx1 variable.
	 */
	if ((sl->sl_xpmag == SL_XPMAG &&
	    sun_extended_sum(sl, &sl->sl_types) == sl->sl_xpsum) ||
	    (sl->sl_xpmag == SL_XPMAGTYP &&
	    sun_extended_sum(sl, &sl->sl_xxx1) == sl->sl_xpsum)) {
		/*
		 * There is.  Copy over the "extended" partitions.
		 * This code parallels the loop for partitions a-h.
		 */
		for (i = 0; i < SUNXPART; i++) {
			spp = &sl->sl_xpart[i];
			npp = &lp->d_partitions[i+8];
			DL_SETPOFFSET(npp, spp->sdkp_cyloffset * secpercyl);
			DL_SETPSIZE(npp, spp->sdkp_nsectors);
			if (DL_GETPSIZE(npp) == 0) {
				npp->p_fstype = FS_UNUSED;
				continue;
			}
			npp->p_fstype = sun_fstypes[i+8];
			if (npp->p_fstype == FS_BSDFFS) {
				npp->p_fragblock =
				    DISKLABELV1_FFS_FRAGBLOCK(2048, 8);
				npp->p_cpg = 16;
			}
		}
		if (sl->sl_xpmag == SL_XPMAGTYP) {
			for (i = 0; i < MAXPARTITIONS; i++) {
				npp = &lp->d_partitions[i];
				npp->p_fstype = sl->sl_types[i];
				npp->p_fragblock = sl->sl_fragblock[i];
				npp->p_cpg = sl->sl_cpg[i];
			}
		}
	} else if (preamble->sl_nparts <= 8) {
		/*
		 * A more traditional Sun label.  Recognise certain filesystem
		 * types from it, if they are available.
		 */
		i = preamble->sl_nparts;
		if (i == 0)
			i = 8;

		npp = &lp->d_partitions[i-1];
		ppp = &preamble->sl_part[i-1];
		for (; i > 0; i--, npp--, ppp--) {
			if (npp->p_size == 0)
				continue;
			if ((ppp->spi_tag == 0) && (ppp->spi_flag == 0))
				continue;

			switch (ppp->spi_tag) {
			case SPTAG_SUNOS_ROOT:
			case SPTAG_SUNOS_USR:
			case SPTAG_SUNOS_VAR:
			case SPTAG_SUNOS_HOME:
				npp->p_fstype = FS_BSDFFS;
				npp->p_fragblock =
				    DISKLABELV1_FFS_FRAGBLOCK(2048, 8);
				npp->p_cpg = 16;
				break;
			case SPTAG_LINUX_EXT2:
				npp->p_fstype = FS_EXT2FS;
				break;
			default:
				/* FS_SWAP for _SUNOS_SWAP and _LINUX_SWAP? */
				npp->p_fstype = FS_UNUSED;
				break;
			}
		}
	}

	lp->d_checksum = 0;
	lp->d_checksum = dkcksum(lp);
	return (NULL);
}

/*
 * Given a BSD disk label, update the Sun disklabel
 * pointed to by cp with the new info.  Note that the
 * Sun disklabel may have other info we need to keep.
 * Returns zero or error code.
 */
static int
disklabel_bsd_to_sun(struct disklabel *lp, struct sun_disklabel *sl)
{
	struct partition *npp;
	struct sun_dkpart *spp;
	int i, secpercyl;
	u_short cksum, *sp1, *sp2;

	/* Enforce preconditions */
	if (lp->d_secsize != 512 || lp->d_nsectors == 0 || lp->d_ntracks == 0)
		return (EINVAL);

	/* Format conversion. */
	memcpy(sl->sl_text, lp->d_packname, sizeof(lp->d_packname));
	sl->sl_rpm = lp->d_rpm;
	sl->sl_pcylinders = lp->d_ncylinders + lp->d_acylinders; /* XXX */
	sl->sl_sparespercyl = lp->d_sparespercyl;
	sl->sl_interleave = lp->d_interleave;
	sl->sl_ncylinders = lp->d_ncylinders;
	sl->sl_acylinders = lp->d_acylinders;
	sl->sl_ntracks = lp->d_ntracks;
	sl->sl_nsectors = lp->d_nsectors;

	secpercyl = sl->sl_nsectors * sl->sl_ntracks;
	for (i = 0; i < 8; i++) {
		spp = &sl->sl_part[i];
		npp = &lp->d_partitions[i];

		if (DL_GETPOFFSET(npp) % secpercyl)
			return (EINVAL);
		spp->sdkp_cyloffset = DL_GETPOFFSET(npp) / secpercyl;
		spp->sdkp_nsectors = DL_GETPSIZE(npp);
	}
	sl->sl_magic = SUN_DKMAGIC;

	for (i = 0; i < SUNXPART; i++) {
		if (DL_GETPOFFSET(&lp->d_partitions[i+8]) ||
		    DL_GETPSIZE(&lp->d_partitions[i+8]))
			break;
	}
	for (i = 0; i < SUNXPART; i++) {
		spp = &sl->sl_xpart[i];
		npp = &lp->d_partitions[i+8];
		if (DL_GETPOFFSET(npp) % secpercyl)
			return (EINVAL);
		sl->sl_xpart[i].sdkp_cyloffset =
		    DL_GETPOFFSET(npp) / secpercyl;
		sl->sl_xpart[i].sdkp_nsectors = DL_GETPSIZE(npp);
	}
	for (i = 0; i < MAXPARTITIONS; i++) {
		npp = &lp->d_partitions[i];
		sl->sl_types[i] = npp->p_fstype;
		sl->sl_fragblock[i] = npp->p_fragblock;
		sl->sl_cpg[i] = npp->p_cpg;
	}
	sl->sl_xpmag = SL_XPMAGTYP;
	sl->sl_xpsum = sun_extended_sum(sl, &sl->sl_xxx1);

	/* Correct the XOR check. */
	sp1 = (u_short *)sl;
	sp2 = (u_short *)(sl + 1);
	sl->sl_cksum = cksum = 0;
	while (sp1 < sp2)
		cksum ^= *sp1++;
	sl->sl_cksum = cksum;

	return (0);
}

/*
 * Search the bad sector table looking for the specified sector.
 * Return index if found.
 * Return -1 if not found.
 */
int
isbad(struct dkbad *bt, int cyl, int trk, int sec)
{
	int i;
	long blk, bblk;

	blk = ((long)cyl << 16) + (trk << 8) + sec;
	for (i = 0; i < NBT_BAD; i++) {
		bblk = ((long)bt->bt_bad[i].bt_cyl << 16) +
			bt->bt_bad[i].bt_trksec;
		if (blk == bblk)
			return (i);
		if (blk < bblk || bblk < 0)
			break;
	}
	return (-1);
}
