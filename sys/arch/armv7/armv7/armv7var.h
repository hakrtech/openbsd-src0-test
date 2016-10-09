/* $OpenBSD: armv7var.h,v 1.14 2016/10/09 01:40:43 jsg Exp $ */
/*
 * Copyright (c) 2005,2008 Dale Rahn <drahn@openbsd.com>
 * Copyright (c) 2012-2013 Patrick Wildt <patrick@blueri.se>
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

#ifndef __ARMV7VAR_H__
#define __ARMV7VAR_H__

/* Boards device list */
struct board_dev {
	char	*name;
	int	unit;
};

struct armv7_board {
	uint32_t		board_id;
	struct board_dev	*devs;
	void			(*init)(void);
};

/* Needed by omap, sunxi */
struct armv7_softc {
	struct device sc_dv;

	struct board_dev *sc_board_devs;
};

/* Physical memory range for on-chip devices. */

struct armv7mem {
	bus_addr_t	addr;
	bus_size_t	size;
};

#define ARMV7_DEV_NMEM 6
#define ARMV7_DEV_NIRQ 4
#define ARMV7_DEV_NDMA 4

/* Descriptor for all on-chip devices. */
struct armv7_dev {
	char	*name;			/* driver name or made up name */
	int	unit;			/* driver instance number or -1 */
	struct	armv7mem mem[ARMV7_DEV_NMEM]; /* memory ranges */
	int	irq[ARMV7_DEV_NIRQ];	/* IRQ number(s) */
	int	dma[ARMV7_DEV_NDMA];	/* DMA chan number(s) */
};

/* Passed as third arg to attach functions. */
struct armv7_attach_args {
	struct armv7_dev	*aa_dev;
	bus_space_tag_t		aa_iot;
	bus_dma_tag_t		aa_dmat;
};

extern struct armv7_dev *armv7_devs;

void	armv7_set_devs(struct armv7_dev *);
struct	armv7_dev *armv7_find_dev(const char *, int);
void	armv7_attach(struct device *, struct device *, void *);
int	armv7_submatch(struct device *, void *, void *);

/* board identification - from uboot */
#define BOARD_ID_OMAP3_BEAGLE 1546
#define BOARD_ID_OMAP3_OVERO 1798
#define BOARD_ID_OMAP4_PANDA 2791
#define BOARD_ID_EXYNOS4_SMDKC210 2838
#define BOARD_ID_EXYNOS4_NURI 3379
#define BOARD_ID_AM335X_BEAGLEBONE 3589
#define BOARD_ID_EXYNOS5_CHROMEBOOK 3774
#define BOARD_ID_SUN4I_A10 4104
#define BOARD_ID_SUN7I_A20 4283
extern uint32_t board_id;

#endif /* __ARMV7VAR_H__ */

