/*	$OpenBSD: i386_softraid.c,v 1.6 2015/10/28 18:55:27 stsp Exp $	*/
/*
 * Copyright (c) 2012 Joel Sing <jsing@openbsd.org>
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

#include <sys/param.h>	/* DEV_BSIZE */
#include <sys/types.h>
#include <sys/disklabel.h>
#include <sys/dkio.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#include <dev/biovar.h>
#include <dev/softraidvar.h>
#include <ufs/ufs/dinode.h>

#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <util.h>

#include "installboot.h"
#include "i386_installboot.h"

void	sr_install_bootblk(int, int, int);
void	sr_install_bootldr(int, char *);

void
sr_install_bootblk(int devfd, int vol, int disk)
{
	struct bioc_disk bd;
	struct disklabel dl;
	struct partition *pp;
	uint32_t poffset;
	char *dev;
	char part, efipart;
	int diskfd;

	/* Get device name for this disk/chunk. */
	memset(&bd, 0, sizeof(bd));
	bd.bd_volid = vol;
	bd.bd_diskid = disk;
	if (ioctl(devfd, BIOCDISK, &bd) == -1)
		err(1, "BIOCDISK");

	/* Check disk status. */
	if (bd.bd_status != BIOC_SDONLINE && bd.bd_status != BIOC_SDREBUILD) {
		fprintf(stderr, "softraid chunk %u not online - skipping...\n",
		    disk);
		return;
	}

	if (strlen(bd.bd_vendor) < 1)
		errx(1, "invalid disk name");
	part = bd.bd_vendor[strlen(bd.bd_vendor) - 1];
	if (part < 'a' || part >= 'a' + MAXPARTITIONS)
		errx(1, "invalid partition %c\n", part);
	bd.bd_vendor[strlen(bd.bd_vendor) - 1] = '\0';

	/* Open this device and check its disklabel. */
	if ((diskfd = opendev(bd.bd_vendor, (nowrite? O_RDONLY:O_RDWR),
	    OPENDEV_PART, &dev)) < 0)
		err(1, "open: %s", dev);

	/* Get and check disklabel. */
	if (ioctl(diskfd, DIOCGDINFO, &dl) != 0)
		err(1, "disklabel: %s", dev);
	if (dl.d_magic != DISKMAGIC)
		err(1, "bad disklabel magic=0x%08x", dl.d_magic);

	/* Warn on unknown disklabel types. */
	if (dl.d_type == 0)
		warnx("disklabel type unknown");

	efipart = findgptefisys(diskfd, &dl);
	if (efipart != -1) {
		write_efisystem(&dl, (char)efipart);
		return;
	}

	/* Determine poffset and set symbol value. */
	pp = &dl.d_partitions[part - 'a'];
	if (pp->p_offseth != 0)
		errx(1, "partition offset too high");
	poffset = pp->p_offset;			/* Offset of RAID partition. */
	poffset += SR_BOOT_LOADER_OFFSET;	/* SR boot loader area. */
	sym_set_value(pbr_symbols, "_p_offset", poffset);

	if (verbose)
		fprintf(stderr, "%s%c: installing boot blocks on %s, "
		    "part offset %u\n", bd.bd_vendor, part, dev, poffset);

	/* Write boot blocks to device. */
	write_bootblocks(diskfd, dev, &dl);

	close(diskfd);
}

void
sr_install_bootldr(int devfd, char *dev)
{
	struct bioc_installboot bb;
	struct stat sb;
	struct ufs1_dinode *ino_p;
	uint32_t bootsize, inodeblk, inodedbl;
	uint16_t bsize = SR_FS_BLOCKSIZE;
	uint16_t nblocks;
	uint8_t bshift = 5;		/* fragsize == blocksize */
	int fd, i;
	u_char *p;

	/*
	 * Install boot loader into softraid boot loader storage area.
	 *
	 * In order to allow us to reuse the existing biosboot we construct
	 * a fake FFS filesystem with a single inode, which points to the
	 * boot loader.
	 */

	nblocks = howmany(SR_BOOT_LOADER_SIZE, SR_FS_BLOCKSIZE / DEV_BSIZE);
	inodeblk = nblocks - 1;
	bootsize = nblocks * SR_FS_BLOCKSIZE;

	p = calloc(1, bootsize);
	if (p == NULL)
		err(1, NULL);

	fd = open(stage2, O_RDONLY, 0);
	if (fd == -1)
		err(1, NULL);

	if (fstat(fd, &sb) == -1)
		err(1, NULL);

	nblocks = howmany(sb.st_blocks, SR_FS_BLOCKSIZE / DEV_BSIZE);
	if (sb.st_blocks * S_BLKSIZE > bootsize -
	    (int)(sizeof(struct ufs1_dinode)))
		errx(1, "boot code will not fit");

	/* We only need to fill the direct block array. */
	ino_p = (struct ufs1_dinode *)&p[bootsize - sizeof(struct ufs1_dinode)];

	ino_p->di_mode = sb.st_mode;
	ino_p->di_nlink = 1;
	ino_p->di_inumber = 0xfeebfaab;
	ino_p->di_size = read(fd, p, sb.st_blocks * S_BLKSIZE);
	ino_p->di_blocks = nblocks;
	for (i = 0; i < nblocks; i++)
		ino_p->di_db[i] = i;

	inodedbl = ((u_char*)&ino_p->di_db[0] -
	    &p[bootsize - SR_FS_BLOCKSIZE]) + INODEOFF;

	memset(&bb, 0, sizeof(bb));
	bb.bb_bootldr = p;
	bb.bb_bootldr_size = bootsize;
	bb.bb_bootblk = "XXX";
	bb.bb_bootblk_size = sizeof("XXX");
	strncpy(bb.bb_dev, dev, sizeof(bb.bb_dev));
	if (!nowrite) {
		if (verbose)
			fprintf(stderr, "%s: installing boot loader on "
			    "softraid volume\n", dev);
		if (ioctl(devfd, BIOCINSTALLBOOT, &bb) == -1)
			errx(1, "softraid installboot failed");
	}

	/*
	 * Set the values that will need to go into biosboot
	 * (the partition boot record, a.k.a. the PBR).
	 */
	sym_set_value(pbr_symbols, "_fs_bsize_p", (bsize / 16));
	sym_set_value(pbr_symbols, "_fs_bsize_s", (bsize / 512));
	sym_set_value(pbr_symbols, "_fsbtodb", bshift);
	sym_set_value(pbr_symbols, "_inodeblk", inodeblk);
	sym_set_value(pbr_symbols, "_inodedbl", inodedbl);
	sym_set_value(pbr_symbols, "_nblocks", nblocks);

	if (verbose)
		fprintf(stderr, "%s is %d blocks x %d bytes\n",
		    stage2, nblocks, bsize);

	close(fd);
}
