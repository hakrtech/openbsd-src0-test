/*	$OpenBSD: hidms.c,v 1.7 2014/05/12 09:50:44 mpi Exp $ */
/*	$NetBSD: ums.c,v 1.60 2003/03/11 16:44:00 augustss Exp $	*/

/*
 * Copyright (c) 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Lennart Augustsson (lennart@augustsson.net) at
 * Carlstedt Research & Technology.
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
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * HID spec: http://www.usb.org/developers/devclass_docs/HID1_11.pdf
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/device.h>
#include <sys/ioctl.h>

#include <dev/usb/usb.h>
#include <dev/usb/usbhid.h>

#include <dev/usb/usb_quirks.h>
#include <dev/usb/hid.h>

#include <dev/wscons/wsconsio.h>
#include <dev/wscons/wsmousevar.h>

#include <dev/usb/hidmsvar.h>

#ifdef HIDMS_DEBUG
#define DPRINTF(x)	do { if (hidmsdebug) printf x; } while (0)
#define DPRINTFN(n,x)	do { if (hidmsdebug>(n)) printf x; } while (0)
int	hidmsdebug = 0;
#else
#define DPRINTF(x)
#define DPRINTFN(n,x)
#endif

#define HIDMS_BUT(i)	((i) == 1 || (i) == 2 ? 3 - (i) : i)

#define MOUSE_FLAGS_MASK	(HIO_CONST | HIO_RELATIVE)
#define NOTMOUSE(f)		(((f) & MOUSE_FLAGS_MASK) != HIO_RELATIVE)

int
hidms_setup(struct device *self, struct hidms *ms, uint32_t quirks,
    int id, void *desc, int dlen)
{
	struct hid_item h;
	struct hid_data *d;
	uint32_t flags;
	int i, wheel, twheel;

	ms->sc_device = self;
	ms->sc_rawmode = 1;

	if (quirks & UQ_MS_REVZ)
		ms->sc_flags |= HIDMS_REVZ;
	if (quirks & UQ_SPUR_BUT_UP)
		ms->sc_flags |= HIDMS_SPUR_BUT_UP;
	if (quirks & UQ_MS_LEADING_BYTE)
		ms->sc_flags |= HIDMS_LEADINGBYTE;

	if (!hid_locate(desc, dlen, HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_X), id,
	    hid_input, &ms->sc_loc_x, &flags)) {
		printf("\n%s: mouse has no X report\n", self->dv_xname);
		return ENXIO;
	}
	switch(flags & MOUSE_FLAGS_MASK) {
	case 0:
		ms->sc_flags |= HIDMS_ABSX;
		break;
	case HIO_RELATIVE:
		break;
	default:
		printf("\n%s: X report 0x%04x not supported\n",
		    self->dv_xname, flags);
		return ENXIO;
	}

	if (!hid_locate(desc, dlen, HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_Y), id,
	    hid_input, &ms->sc_loc_y, &flags)) {
		printf("\n%s: mouse has no Y report\n", self->dv_xname);
		return ENXIO;
	}
	switch(flags & MOUSE_FLAGS_MASK) {
	case 0:
		ms->sc_flags |= HIDMS_ABSY;
		break;
	case HIO_RELATIVE:
		break;
	default:
		printf("\n%s: Y report 0x%04x not supported\n",
		    self->dv_xname, flags);
		return ENXIO;
	}

	/*
	 * Try to guess the Z activator: check WHEEL, TWHEEL, and Z,
	 * in that order.
	 */

	wheel = hid_locate(desc, dlen,
	    HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_WHEEL), id,
	    hid_input, &ms->sc_loc_z, &flags);
	if (wheel == 0)
		twheel = hid_locate(desc, dlen,
		    HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_TWHEEL), id,
		    hid_input, &ms->sc_loc_z, &flags);
	else
		twheel = 0;

	if (wheel || twheel) {
		if (NOTMOUSE(flags)) {
			DPRINTF(("\n%s: Wheel report 0x%04x not supported\n",
			    self->dv_xname, flags));
			ms->sc_loc_z.size = 0; /* Bad Z coord, ignore it */
		} else {
			ms->sc_flags |= HIDMS_Z;
			/* Wheels need the Z axis reversed. */
			ms->sc_flags ^= HIDMS_REVZ;
		}
		/*
		 * We might have both a wheel and Z direction; in this case,
		 * report the Z direction on the W axis.
		*/
		if (hid_locate(desc, dlen,
		    HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_Z), id,
		    hid_input, &ms->sc_loc_w, &flags)) {
			if (NOTMOUSE(flags)) {
				DPRINTF(("\n%s: Z report 0x%04x not supported\n",
				    self->dv_xname, flags));
				/* Bad Z coord, ignore it */
				ms->sc_loc_w.size = 0;
			}
			else
				ms->sc_flags |= HIDMS_W;
		}
	} else if (hid_locate(desc, dlen,
	    HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_Z), id,
	    hid_input, &ms->sc_loc_z, &flags)) {
		if (NOTMOUSE(flags)) {
			DPRINTF(("\n%s: Z report 0x%04x not supported\n",
			    self->dv_xname, flags));
			ms->sc_loc_z.size = 0; /* Bad Z coord, ignore it */
		} else {
			ms->sc_flags |= HIDMS_Z;
		}
	}

	/*
	 * The Microsoft Wireless Intellimouse 2.0 reports its wheel
	 * using 0x0048 (I've called it HUG_TWHEEL) and seems to expect
	 * us to know that the byte after the wheel is the tilt axis.
	 * There are no other HID axis descriptors other than X, Y and
	 * TWHEEL, so we report TWHEEL on the W axis.
	 */
	if (twheel) {
		ms->sc_loc_w = ms->sc_loc_z;
		ms->sc_loc_w.pos = ms->sc_loc_w.pos + 8;
		ms->sc_flags |= HIDMS_W | HIDMS_LEADINGBYTE;
		/* Wheels need their axis reversed. */
		ms->sc_flags ^= HIDMS_REVW;
	}

	/* figure out the number of buttons */
	for (i = 1; i <= MAX_BUTTONS; i++)
		if (!hid_locate(desc, dlen, HID_USAGE2(HUP_BUTTON, i), id,
		    hid_input, &ms->sc_loc_btn[i - 1], NULL))
			break;
	ms->sc_num_buttons = i - 1;

	if (hid_locate(desc, dlen, HID_USAGE2(HUP_DIGITIZERS,
	    HUD_TIP_SWITCH), id, hid_input,
	    &ms->sc_loc_btn[ms->sc_num_buttons], NULL)){
		ms->sc_flags |= HIDMS_TIP;
		ms->sc_num_buttons++;
	}

	if (hid_locate(desc, dlen, HID_USAGE2(HUP_DIGITIZERS,
	    HUD_ERASER), id, hid_input,
	    &ms->sc_loc_btn[ms->sc_num_buttons], NULL)){
		ms->sc_flags |= HIDMS_ERASER;
		ms->sc_num_buttons++;
	}

	if (hid_locate(desc, dlen, HID_USAGE2(HUP_DIGITIZERS,
	    HUD_BARREL_SWITCH), id, hid_input,
	    &ms->sc_loc_btn[ms->sc_num_buttons], NULL)){
		ms->sc_flags |= HIDMS_BARREL;
		ms->sc_num_buttons++;
	}

	/*
	 * The Microsoft Wireless Notebook Optical Mouse seems to be in worse
	 * shape than the Wireless Intellimouse 2.0, as its X, Y, wheel, and
	 * all of its other button positions are all off. It also reports that
	 * it has two addional buttons and a tilt wheel.
	 */
	if (quirks & UQ_MS_BAD_CLASS) {
		/* HIDMS_LEADINGBYTE cleared on purpose */
		ms->sc_flags = HIDMS_Z | HIDMS_SPUR_BUT_UP;
		ms->sc_num_buttons = 3;
		/* XXX change sc_hdev isize to 5? */
		/* 1st byte of descriptor report contains garbage */
		ms->sc_loc_x.pos = 16;
		ms->sc_loc_y.pos = 24;
		ms->sc_loc_z.pos = 32;
		ms->sc_loc_btn[0].pos = 8;
		ms->sc_loc_btn[1].pos = 9;
		ms->sc_loc_btn[2].pos = 10;
	}
	/* Parse descriptors to get touch panel bounds */
	d = hid_start_parse(desc, dlen, hid_input);
	while (hid_get_item(d, &h)) {
		if (h.kind != hid_input ||
		    HID_GET_USAGE_PAGE(h.usage) != HUP_GENERIC_DESKTOP)
			continue;
		DPRINTF(("hidms: usage=0x%x range %d..%d\n",
			h.usage, h.logical_minimum, h.logical_maximum));
		switch (HID_GET_USAGE(h.usage)) {
		case HUG_X:
			if (ms->sc_flags & HIDMS_ABSX) {
				ms->sc_tsscale.minx = h.logical_minimum;
				ms->sc_tsscale.maxx = h.logical_maximum;
			}
			break;
		case HUG_Y:
			if (ms->sc_flags & HIDMS_ABSY) {
				ms->sc_tsscale.miny = h.logical_minimum;
				ms->sc_tsscale.maxy = h.logical_maximum;
			}
			break;
		}
	}
	hid_end_parse(d);
	return 0;
}

void
hidms_attach(struct hidms *ms, const struct wsmouse_accessops *ops)
{
	struct wsmousedev_attach_args a;
#ifdef HIDMS_DEBUG
	int i;
#endif

	printf(": %d button%s",
	    ms->sc_num_buttons, ms->sc_num_buttons <= 1 ? "" : "s");
	switch (ms->sc_flags & (HIDMS_Z | HIDMS_W)) {
	case HIDMS_Z:
		printf(", Z dir");
		break;
	case HIDMS_W:
		printf(", W dir");
		break;
	case HIDMS_Z | HIDMS_W:
		printf(", Z and W dir");
		break;
	}

	if (ms->sc_flags & HIDMS_TIP)
		printf(", tip");
	if (ms->sc_flags & HIDMS_BARREL)
		printf(", barrel");
	if (ms->sc_flags & HIDMS_ERASER)
		printf(", eraser");

	printf("\n");

#ifdef HIDMS_DEBUG
	DPRINTF(("hidms_attach: ms=%p\n", ms));
	DPRINTF(("hidms_attach: X\t%d/%d\n",
	     ms->sc_loc_x.pos, ms->sc_loc_x.size));
	DPRINTF(("hidms_attach: Y\t%d/%d\n",
	    ms->sc_loc_y.pos, ms->sc_loc_y.size));
	if (ms->sc_flags & HIDMS_Z)
		DPRINTF(("hidms_attach: Z\t%d/%d\n",
		    ms->sc_loc_z.pos, ms->sc_loc_z.size));
	if (ms->sc_flags & HIDMS_W)
		DPRINTF(("hidms_attach: W\t%d/%d\n",
		    ms->sc_loc_w.pos, ms->sc_loc_w.size));
	for (i = 1; i <= ms->sc_num_buttons; i++) {
		DPRINTF(("hidms_attach: B%d\t%d/%d\n",
		    i, ms->sc_loc_btn[i - 1].pos, ms->sc_loc_btn[i - 1].size));
	}
#endif

	a.accessops = ops;
	a.accesscookie = ms->sc_device;
	ms->sc_wsmousedev = config_found(ms->sc_device, &a, wsmousedevprint);
}

int
hidms_detach(struct hidms *ms, int flags)
{
	int rv = 0;

	DPRINTF(("hidms_detach: ms=%p flags=%d\n", ms, flags));

	/* No need to do reference counting of hidms, wsmouse has all the goo */
	if (ms->sc_wsmousedev != NULL)
		rv = config_detach(ms->sc_wsmousedev, flags);

	return (rv);
}

void
hidms_input(struct hidms *ms, uint8_t *data, u_int len)
{
	int dx, dy, dz, dw;
	u_int32_t buttons = 0;
	int flags;
	int i, s;

	DPRINTFN(5,("hidms_input: len=%d\n", len));

	/*
	 * The Microsoft Wireless Intellimouse 2.0 sends one extra leading
	 * byte of data compared to most USB mice.  This byte frequently
	 * switches from 0x01 (usual state) to 0x02.  It may be used to
	 * report non-standard events (such as battery life).  However,
	 * at the same time, it generates a left click event on the
	 * button byte, where there shouldn't be any.  We simply discard
	 * the packet in this case.
	 *
	 * This problem affects the MS Wireless Notebook Optical Mouse, too.
	 * However, the leading byte for this mouse is normally 0x11, and
	 * the phantom mouse click occurs when it's 0x14.
	 */
	if (ms->sc_flags & HIDMS_LEADINGBYTE) {
		if (*data++ == 0x02)
			return;
		/* len--; */
	} else if (ms->sc_flags & HIDMS_SPUR_BUT_UP) {
		if (*data == 0x14 || *data == 0x15)
			return;
	}

	flags = WSMOUSE_INPUT_DELTA;
	if (ms->sc_flags & HIDMS_ABSX)
		flags |= WSMOUSE_INPUT_ABSOLUTE_X;
	if (ms->sc_flags & HIDMS_ABSY)
		flags |= WSMOUSE_INPUT_ABSOLUTE_Y;

	dx =  hid_get_data(data, len, &ms->sc_loc_x);
	dy = -hid_get_data(data, len, &ms->sc_loc_y);
	dz =  hid_get_data(data, len, &ms->sc_loc_z);
	dw =  hid_get_data(data, len, &ms->sc_loc_w);

	if (ms->sc_flags & HIDMS_ABSY)
		dy = -dy;
	if (ms->sc_flags & HIDMS_REVZ)
		dz = -dz;
	if (ms->sc_flags & HIDMS_REVW)
		dw = -dw;

	if (ms->sc_tsscale.swapxy && !ms->sc_rawmode) {
		int tmp = dx;
		dx = dy;
		dy = tmp;
	}

	if (!ms->sc_rawmode &&
	    (ms->sc_tsscale.maxx - ms->sc_tsscale.minx) != 0 &&
	    (ms->sc_tsscale.maxy - ms->sc_tsscale.miny) != 0) {
		/* Scale down to the screen resolution. */
		dx = ((dx - ms->sc_tsscale.minx) * ms->sc_tsscale.resx) /
		    (ms->sc_tsscale.maxx - ms->sc_tsscale.minx);
		dy = ((dy - ms->sc_tsscale.miny) * ms->sc_tsscale.resy) /
		    (ms->sc_tsscale.maxy - ms->sc_tsscale.miny);
	}

	for (i = 0; i < ms->sc_num_buttons; i++)
		if (hid_get_data(data, len, &ms->sc_loc_btn[i]))
			buttons |= (1 << HIDMS_BUT(i));

	if (dx != 0 || dy != 0 || dz != 0 || dw != 0 ||
	    buttons != ms->sc_buttons) {
		DPRINTFN(10, ("hidms_input: x:%d y:%d z:%d w:%d buttons:0x%x\n",
			dx, dy, dz, dw, buttons));
		ms->sc_buttons = buttons;
		if (ms->sc_wsmousedev != NULL) {
			s = spltty();
			wsmouse_input(ms->sc_wsmousedev, buttons,
			    dx, dy, dz, dw, flags);
			splx(s);
		}
	}
}

int
hidms_enable(struct hidms *ms)
{
	if (ms->sc_enabled)
		return EBUSY;

	ms->sc_enabled = 1;
	ms->sc_buttons = 0;
	return 0;
}

int
hidms_ioctl(struct hidms *ms, u_long cmd, caddr_t data, int flag,
    struct proc *p)
{
	struct wsmouse_calibcoords *wsmc = (struct wsmouse_calibcoords *)data;

	switch (cmd) {
	case WSMOUSEIO_SCALIBCOORDS:
		if (!(wsmc->minx >= 0 && wsmc->maxx >= 0 &&
		    wsmc->miny >= 0 && wsmc->maxy >= 0 &&
		    wsmc->resx >= 0 && wsmc->resy >= 0 &&
		    wsmc->minx < 32768 && wsmc->maxx < 32768 &&
		    wsmc->miny < 32768 && wsmc->maxy < 32768 &&
		    (wsmc->maxx - wsmc->minx) != 0 &&
		    (wsmc->maxy - wsmc->miny) != 0 &&
		    wsmc->resx < 32768 && wsmc->resy < 32768 &&
		    wsmc->swapxy >= 0 && wsmc->swapxy <= 1 &&
		    wsmc->samplelen >= 0 && wsmc->samplelen <= 1))
			return (EINVAL);

		ms->sc_tsscale.minx = wsmc->minx;
		ms->sc_tsscale.maxx = wsmc->maxx;
		ms->sc_tsscale.miny = wsmc->miny;
		ms->sc_tsscale.maxy = wsmc->maxy;
		ms->sc_tsscale.swapxy = wsmc->swapxy;
		ms->sc_tsscale.resx = wsmc->resx;
		ms->sc_tsscale.resy = wsmc->resy;
		ms->sc_rawmode = wsmc->samplelen;
		return 0;
	case WSMOUSEIO_GCALIBCOORDS:
		wsmc->minx = ms->sc_tsscale.minx;
		wsmc->maxx = ms->sc_tsscale.maxx;
		wsmc->miny = ms->sc_tsscale.miny;
		wsmc->maxy = ms->sc_tsscale.maxy;
		wsmc->swapxy = ms->sc_tsscale.swapxy;
		wsmc->resx = ms->sc_tsscale.resx;
		wsmc->resy = ms->sc_tsscale.resy;
		wsmc->samplelen = ms->sc_rawmode;
		return 0;
	case WSMOUSEIO_GTYPE:
		if (ms->sc_flags & HIDMS_ABSX && ms->sc_flags & HIDMS_ABSY) {
			*(u_int *)data = WSMOUSE_TYPE_TPANEL;
			return 0;
		}
		/* FALLTHROUGH */
	default:
		return -1;
	}
}

void
hidms_disable(struct hidms *ms)
{
	ms->sc_enabled = 0;
}
