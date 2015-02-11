/*	$OpenBSD: r600.c,v 1.12 2015/02/11 07:01:37 jsg Exp $	*/
/*
 * Copyright 2008 Advanced Micro Devices, Inc.
 * Copyright 2008 Red Hat Inc.
 * Copyright 2009 Jerome Glisse.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors: Dave Airlie
 *          Alex Deucher
 *          Jerome Glisse
 */
#include <dev/pci/drm/drmP.h>
#include <dev/pci/drm/radeon_drm.h>
#include "radeon.h"
#include "radeon_asic.h"
#include "radeon_mode.h"
#include "r600d.h"
#include "atom.h"
#include "avivod.h"

#define PFP_UCODE_SIZE 576
#define PM4_UCODE_SIZE 1792
#define RLC_UCODE_SIZE 768
#define R700_PFP_UCODE_SIZE 848
#define R700_PM4_UCODE_SIZE 1360
#define R700_RLC_UCODE_SIZE 1024
#define EVERGREEN_PFP_UCODE_SIZE 1120
#define EVERGREEN_PM4_UCODE_SIZE 1376
#define EVERGREEN_RLC_UCODE_SIZE 768
#define CAYMAN_RLC_UCODE_SIZE 1024
#define ARUBA_RLC_UCODE_SIZE 1536

/* Firmware Names */
MODULE_FIRMWARE("radeon/R600_pfp.bin");
MODULE_FIRMWARE("radeon/R600_me.bin");
MODULE_FIRMWARE("radeon/RV610_pfp.bin");
MODULE_FIRMWARE("radeon/RV610_me.bin");
MODULE_FIRMWARE("radeon/RV630_pfp.bin");
MODULE_FIRMWARE("radeon/RV630_me.bin");
MODULE_FIRMWARE("radeon/RV620_pfp.bin");
MODULE_FIRMWARE("radeon/RV620_me.bin");
MODULE_FIRMWARE("radeon/RV635_pfp.bin");
MODULE_FIRMWARE("radeon/RV635_me.bin");
MODULE_FIRMWARE("radeon/RV670_pfp.bin");
MODULE_FIRMWARE("radeon/RV670_me.bin");
MODULE_FIRMWARE("radeon/RS780_pfp.bin");
MODULE_FIRMWARE("radeon/RS780_me.bin");
MODULE_FIRMWARE("radeon/RV770_pfp.bin");
MODULE_FIRMWARE("radeon/RV770_me.bin");
MODULE_FIRMWARE("radeon/RV730_pfp.bin");
MODULE_FIRMWARE("radeon/RV730_me.bin");
MODULE_FIRMWARE("radeon/RV710_pfp.bin");
MODULE_FIRMWARE("radeon/RV710_me.bin");
MODULE_FIRMWARE("radeon/R600_rlc.bin");
MODULE_FIRMWARE("radeon/R700_rlc.bin");
MODULE_FIRMWARE("radeon/CEDAR_pfp.bin");
MODULE_FIRMWARE("radeon/CEDAR_me.bin");
MODULE_FIRMWARE("radeon/CEDAR_rlc.bin");
MODULE_FIRMWARE("radeon/REDWOOD_pfp.bin");
MODULE_FIRMWARE("radeon/REDWOOD_me.bin");
MODULE_FIRMWARE("radeon/REDWOOD_rlc.bin");
MODULE_FIRMWARE("radeon/JUNIPER_pfp.bin");
MODULE_FIRMWARE("radeon/JUNIPER_me.bin");
MODULE_FIRMWARE("radeon/JUNIPER_rlc.bin");
MODULE_FIRMWARE("radeon/CYPRESS_pfp.bin");
MODULE_FIRMWARE("radeon/CYPRESS_me.bin");
MODULE_FIRMWARE("radeon/CYPRESS_rlc.bin");
MODULE_FIRMWARE("radeon/PALM_pfp.bin");
MODULE_FIRMWARE("radeon/PALM_me.bin");
MODULE_FIRMWARE("radeon/SUMO_rlc.bin");
MODULE_FIRMWARE("radeon/SUMO_pfp.bin");
MODULE_FIRMWARE("radeon/SUMO_me.bin");
MODULE_FIRMWARE("radeon/SUMO2_pfp.bin");
MODULE_FIRMWARE("radeon/SUMO2_me.bin");

int r600_debugfs_mc_info_init(struct radeon_device *rdev);

/* r600,rv610,rv630,rv620,rv635,rv670 */
int r600_mc_wait_for_idle(struct radeon_device *rdev);
static void r600_gpu_init(struct radeon_device *rdev);
void r600_fini(struct radeon_device *rdev);
void r600_irq_disable(struct radeon_device *rdev);
static void r600_pcie_gen2_enable(struct radeon_device *rdev);
int r600_ih_ring_alloc(struct radeon_device *rdev);
void r600_ih_ring_fini(struct radeon_device *rdev);
void r600_vram_gtt_location(struct radeon_device *rdev, struct radeon_mc *mc);
void r600_rlc_start(struct radeon_device *rdev);

/* get temperature in millidegrees */
int rv6xx_get_temp(struct radeon_device *rdev)
{
	u32 temp = (RREG32(CG_THERMAL_STATUS) & ASIC_T_MASK) >>
		ASIC_T_SHIFT;
	int actual_temp = temp & 0xff;

	if (temp & 0x100)
		actual_temp -= 256;

	return actual_temp * 1000;
}

void r600_pm_get_dynpm_state(struct radeon_device *rdev)
{
	int i;

	rdev->pm.dynpm_can_upclock = true;
	rdev->pm.dynpm_can_downclock = true;

	/* power state array is low to high, default is first */
	if ((rdev->flags & RADEON_IS_IGP) || (rdev->family == CHIP_R600)) {
		int min_power_state_index = 0;

		if (rdev->pm.num_power_states > 2)
			min_power_state_index = 1;

		switch (rdev->pm.dynpm_planned_action) {
		case DYNPM_ACTION_MINIMUM:
			rdev->pm.requested_power_state_index = min_power_state_index;
			rdev->pm.requested_clock_mode_index = 0;
			rdev->pm.dynpm_can_downclock = false;
			break;
		case DYNPM_ACTION_DOWNCLOCK:
			if (rdev->pm.current_power_state_index == min_power_state_index) {
				rdev->pm.requested_power_state_index = rdev->pm.current_power_state_index;
				rdev->pm.dynpm_can_downclock = false;
			} else {
				if (rdev->pm.active_crtc_count > 1) {
					for (i = 0; i < rdev->pm.num_power_states; i++) {
						if (rdev->pm.power_state[i].flags & RADEON_PM_STATE_SINGLE_DISPLAY_ONLY)
							continue;
						else if (i >= rdev->pm.current_power_state_index) {
							rdev->pm.requested_power_state_index =
								rdev->pm.current_power_state_index;
							break;
						} else {
							rdev->pm.requested_power_state_index = i;
							break;
						}
					}
				} else {
					if (rdev->pm.current_power_state_index == 0)
						rdev->pm.requested_power_state_index =
							rdev->pm.num_power_states - 1;
					else
						rdev->pm.requested_power_state_index =
							rdev->pm.current_power_state_index - 1;
				}
			}
			rdev->pm.requested_clock_mode_index = 0;
			/* don't use the power state if crtcs are active and no display flag is set */
			if ((rdev->pm.active_crtc_count > 0) &&
			    (rdev->pm.power_state[rdev->pm.requested_power_state_index].
			     clock_info[rdev->pm.requested_clock_mode_index].flags &
			     RADEON_PM_MODE_NO_DISPLAY)) {
				rdev->pm.requested_power_state_index++;
			}
			break;
		case DYNPM_ACTION_UPCLOCK:
			if (rdev->pm.current_power_state_index == (rdev->pm.num_power_states - 1)) {
				rdev->pm.requested_power_state_index = rdev->pm.current_power_state_index;
				rdev->pm.dynpm_can_upclock = false;
			} else {
				if (rdev->pm.active_crtc_count > 1) {
					for (i = (rdev->pm.num_power_states - 1); i >= 0; i--) {
						if (rdev->pm.power_state[i].flags & RADEON_PM_STATE_SINGLE_DISPLAY_ONLY)
							continue;
						else if (i <= rdev->pm.current_power_state_index) {
							rdev->pm.requested_power_state_index =
								rdev->pm.current_power_state_index;
							break;
						} else {
							rdev->pm.requested_power_state_index = i;
							break;
						}
					}
				} else
					rdev->pm.requested_power_state_index =
						rdev->pm.current_power_state_index + 1;
			}
			rdev->pm.requested_clock_mode_index = 0;
			break;
		case DYNPM_ACTION_DEFAULT:
			rdev->pm.requested_power_state_index = rdev->pm.default_power_state_index;
			rdev->pm.requested_clock_mode_index = 0;
			rdev->pm.dynpm_can_upclock = false;
			break;
		case DYNPM_ACTION_NONE:
		default:
			DRM_ERROR("Requested mode for not defined action\n");
			return;
		}
	} else {
		/* XXX select a power state based on AC/DC, single/dualhead, etc. */
		/* for now just select the first power state and switch between clock modes */
		/* power state array is low to high, default is first (0) */
		if (rdev->pm.active_crtc_count > 1) {
			rdev->pm.requested_power_state_index = -1;
			/* start at 1 as we don't want the default mode */
			for (i = 1; i < rdev->pm.num_power_states; i++) {
				if (rdev->pm.power_state[i].flags & RADEON_PM_STATE_SINGLE_DISPLAY_ONLY)
					continue;
				else if ((rdev->pm.power_state[i].type == POWER_STATE_TYPE_PERFORMANCE) ||
					 (rdev->pm.power_state[i].type == POWER_STATE_TYPE_BATTERY)) {
					rdev->pm.requested_power_state_index = i;
					break;
				}
			}
			/* if nothing selected, grab the default state. */
			if (rdev->pm.requested_power_state_index == -1)
				rdev->pm.requested_power_state_index = 0;
		} else
			rdev->pm.requested_power_state_index = 1;

		switch (rdev->pm.dynpm_planned_action) {
		case DYNPM_ACTION_MINIMUM:
			rdev->pm.requested_clock_mode_index = 0;
			rdev->pm.dynpm_can_downclock = false;
			break;
		case DYNPM_ACTION_DOWNCLOCK:
			if (rdev->pm.requested_power_state_index == rdev->pm.current_power_state_index) {
				if (rdev->pm.current_clock_mode_index == 0) {
					rdev->pm.requested_clock_mode_index = 0;
					rdev->pm.dynpm_can_downclock = false;
				} else
					rdev->pm.requested_clock_mode_index =
						rdev->pm.current_clock_mode_index - 1;
			} else {
				rdev->pm.requested_clock_mode_index = 0;
				rdev->pm.dynpm_can_downclock = false;
			}
			/* don't use the power state if crtcs are active and no display flag is set */
			if ((rdev->pm.active_crtc_count > 0) &&
			    (rdev->pm.power_state[rdev->pm.requested_power_state_index].
			     clock_info[rdev->pm.requested_clock_mode_index].flags &
			     RADEON_PM_MODE_NO_DISPLAY)) {
				rdev->pm.requested_clock_mode_index++;
			}
			break;
		case DYNPM_ACTION_UPCLOCK:
			if (rdev->pm.requested_power_state_index == rdev->pm.current_power_state_index) {
				if (rdev->pm.current_clock_mode_index ==
				    (rdev->pm.power_state[rdev->pm.requested_power_state_index].num_clock_modes - 1)) {
					rdev->pm.requested_clock_mode_index = rdev->pm.current_clock_mode_index;
					rdev->pm.dynpm_can_upclock = false;
				} else
					rdev->pm.requested_clock_mode_index =
						rdev->pm.current_clock_mode_index + 1;
			} else {
				rdev->pm.requested_clock_mode_index =
					rdev->pm.power_state[rdev->pm.requested_power_state_index].num_clock_modes - 1;
				rdev->pm.dynpm_can_upclock = false;
			}
			break;
		case DYNPM_ACTION_DEFAULT:
			rdev->pm.requested_power_state_index = rdev->pm.default_power_state_index;
			rdev->pm.requested_clock_mode_index = 0;
			rdev->pm.dynpm_can_upclock = false;
			break;
		case DYNPM_ACTION_NONE:
		default:
			DRM_ERROR("Requested mode for not defined action\n");
			return;
		}
	}

	DRM_DEBUG_DRIVER("Requested: e: %d m: %d p: %d\n",
		  rdev->pm.power_state[rdev->pm.requested_power_state_index].
		  clock_info[rdev->pm.requested_clock_mode_index].sclk,
		  rdev->pm.power_state[rdev->pm.requested_power_state_index].
		  clock_info[rdev->pm.requested_clock_mode_index].mclk,
		  rdev->pm.power_state[rdev->pm.requested_power_state_index].
		  pcie_lanes);
}

void rs780_pm_init_profile(struct radeon_device *rdev)
{
	if (rdev->pm.num_power_states == 2) {
		/* default */
		rdev->pm.profiles[PM_PROFILE_DEFAULT_IDX].dpms_off_ps_idx = rdev->pm.default_power_state_index;
		rdev->pm.profiles[PM_PROFILE_DEFAULT_IDX].dpms_on_ps_idx = rdev->pm.default_power_state_index;
		rdev->pm.profiles[PM_PROFILE_DEFAULT_IDX].dpms_off_cm_idx = 0;
		rdev->pm.profiles[PM_PROFILE_DEFAULT_IDX].dpms_on_cm_idx = 0;
		/* low sh */
		rdev->pm.profiles[PM_PROFILE_LOW_SH_IDX].dpms_off_ps_idx = 0;
		rdev->pm.profiles[PM_PROFILE_LOW_SH_IDX].dpms_on_ps_idx = 0;
		rdev->pm.profiles[PM_PROFILE_LOW_SH_IDX].dpms_off_cm_idx = 0;
		rdev->pm.profiles[PM_PROFILE_LOW_SH_IDX].dpms_on_cm_idx = 0;
		/* mid sh */
		rdev->pm.profiles[PM_PROFILE_MID_SH_IDX].dpms_off_ps_idx = 0;
		rdev->pm.profiles[PM_PROFILE_MID_SH_IDX].dpms_on_ps_idx = 0;
		rdev->pm.profiles[PM_PROFILE_MID_SH_IDX].dpms_off_cm_idx = 0;
		rdev->pm.profiles[PM_PROFILE_MID_SH_IDX].dpms_on_cm_idx = 0;
		/* high sh */
		rdev->pm.profiles[PM_PROFILE_HIGH_SH_IDX].dpms_off_ps_idx = 0;
		rdev->pm.profiles[PM_PROFILE_HIGH_SH_IDX].dpms_on_ps_idx = 1;
		rdev->pm.profiles[PM_PROFILE_HIGH_SH_IDX].dpms_off_cm_idx = 0;
		rdev->pm.profiles[PM_PROFILE_HIGH_SH_IDX].dpms_on_cm_idx = 0;
		/* low mh */
		rdev->pm.profiles[PM_PROFILE_LOW_MH_IDX].dpms_off_ps_idx = 0;
		rdev->pm.profiles[PM_PROFILE_LOW_MH_IDX].dpms_on_ps_idx = 0;
		rdev->pm.profiles[PM_PROFILE_LOW_MH_IDX].dpms_off_cm_idx = 0;
		rdev->pm.profiles[PM_PROFILE_LOW_MH_IDX].dpms_on_cm_idx = 0;
		/* mid mh */
		rdev->pm.profiles[PM_PROFILE_MID_MH_IDX].dpms_off_ps_idx = 0;
		rdev->pm.profiles[PM_PROFILE_MID_MH_IDX].dpms_on_ps_idx = 0;
		rdev->pm.profiles[PM_PROFILE_MID_MH_IDX].dpms_off_cm_idx = 0;
		rdev->pm.profiles[PM_PROFILE_MID_MH_IDX].dpms_on_cm_idx = 0;
		/* high mh */
		rdev->pm.profiles[PM_PROFILE_HIGH_MH_IDX].dpms_off_ps_idx = 0;
		rdev->pm.profiles[PM_PROFILE_HIGH_MH_IDX].dpms_on_ps_idx = 1;
		rdev->pm.profiles[PM_PROFILE_HIGH_MH_IDX].dpms_off_cm_idx = 0;
		rdev->pm.profiles[PM_PROFILE_HIGH_MH_IDX].dpms_on_cm_idx = 0;
	} else if (rdev->pm.num_power_states == 3) {
		/* default */
		rdev->pm.profiles[PM_PROFILE_DEFAULT_IDX].dpms_off_ps_idx = rdev->pm.default_power_state_index;
		rdev->pm.profiles[PM_PROFILE_DEFAULT_IDX].dpms_on_ps_idx = rdev->pm.default_power_state_index;
		rdev->pm.profiles[PM_PROFILE_DEFAULT_IDX].dpms_off_cm_idx = 0;
		rdev->pm.profiles[PM_PROFILE_DEFAULT_IDX].dpms_on_cm_idx = 0;
		/* low sh */
		rdev->pm.profiles[PM_PROFILE_LOW_SH_IDX].dpms_off_ps_idx = 1;
		rdev->pm.profiles[PM_PROFILE_LOW_SH_IDX].dpms_on_ps_idx = 1;
		rdev->pm.profiles[PM_PROFILE_LOW_SH_IDX].dpms_off_cm_idx = 0;
		rdev->pm.profiles[PM_PROFILE_LOW_SH_IDX].dpms_on_cm_idx = 0;
		/* mid sh */
		rdev->pm.profiles[PM_PROFILE_MID_SH_IDX].dpms_off_ps_idx = 1;
		rdev->pm.profiles[PM_PROFILE_MID_SH_IDX].dpms_on_ps_idx = 1;
		rdev->pm.profiles[PM_PROFILE_MID_SH_IDX].dpms_off_cm_idx = 0;
		rdev->pm.profiles[PM_PROFILE_MID_SH_IDX].dpms_on_cm_idx = 0;
		/* high sh */
		rdev->pm.profiles[PM_PROFILE_HIGH_SH_IDX].dpms_off_ps_idx = 1;
		rdev->pm.profiles[PM_PROFILE_HIGH_SH_IDX].dpms_on_ps_idx = 2;
		rdev->pm.profiles[PM_PROFILE_HIGH_SH_IDX].dpms_off_cm_idx = 0;
		rdev->pm.profiles[PM_PROFILE_HIGH_SH_IDX].dpms_on_cm_idx = 0;
		/* low mh */
		rdev->pm.profiles[PM_PROFILE_LOW_MH_IDX].dpms_off_ps_idx = 1;
		rdev->pm.profiles[PM_PROFILE_LOW_MH_IDX].dpms_on_ps_idx = 1;
		rdev->pm.profiles[PM_PROFILE_LOW_MH_IDX].dpms_off_cm_idx = 0;
		rdev->pm.profiles[PM_PROFILE_LOW_MH_IDX].dpms_on_cm_idx = 0;
		/* mid mh */
		rdev->pm.profiles[PM_PROFILE_MID_MH_IDX].dpms_off_ps_idx = 1;
		rdev->pm.profiles[PM_PROFILE_MID_MH_IDX].dpms_on_ps_idx = 1;
		rdev->pm.profiles[PM_PROFILE_MID_MH_IDX].dpms_off_cm_idx = 0;
		rdev->pm.profiles[PM_PROFILE_MID_MH_IDX].dpms_on_cm_idx = 0;
		/* high mh */
		rdev->pm.profiles[PM_PROFILE_HIGH_MH_IDX].dpms_off_ps_idx = 1;
		rdev->pm.profiles[PM_PROFILE_HIGH_MH_IDX].dpms_on_ps_idx = 2;
		rdev->pm.profiles[PM_PROFILE_HIGH_MH_IDX].dpms_off_cm_idx = 0;
		rdev->pm.profiles[PM_PROFILE_HIGH_MH_IDX].dpms_on_cm_idx = 0;
	} else {
		/* default */
		rdev->pm.profiles[PM_PROFILE_DEFAULT_IDX].dpms_off_ps_idx = rdev->pm.default_power_state_index;
		rdev->pm.profiles[PM_PROFILE_DEFAULT_IDX].dpms_on_ps_idx = rdev->pm.default_power_state_index;
		rdev->pm.profiles[PM_PROFILE_DEFAULT_IDX].dpms_off_cm_idx = 0;
		rdev->pm.profiles[PM_PROFILE_DEFAULT_IDX].dpms_on_cm_idx = 0;
		/* low sh */
		rdev->pm.profiles[PM_PROFILE_LOW_SH_IDX].dpms_off_ps_idx = 2;
		rdev->pm.profiles[PM_PROFILE_LOW_SH_IDX].dpms_on_ps_idx = 2;
		rdev->pm.profiles[PM_PROFILE_LOW_SH_IDX].dpms_off_cm_idx = 0;
		rdev->pm.profiles[PM_PROFILE_LOW_SH_IDX].dpms_on_cm_idx = 0;
		/* mid sh */
		rdev->pm.profiles[PM_PROFILE_MID_SH_IDX].dpms_off_ps_idx = 2;
		rdev->pm.profiles[PM_PROFILE_MID_SH_IDX].dpms_on_ps_idx = 2;
		rdev->pm.profiles[PM_PROFILE_MID_SH_IDX].dpms_off_cm_idx = 0;
		rdev->pm.profiles[PM_PROFILE_MID_SH_IDX].dpms_on_cm_idx = 0;
		/* high sh */
		rdev->pm.profiles[PM_PROFILE_HIGH_SH_IDX].dpms_off_ps_idx = 2;
		rdev->pm.profiles[PM_PROFILE_HIGH_SH_IDX].dpms_on_ps_idx = 3;
		rdev->pm.profiles[PM_PROFILE_HIGH_SH_IDX].dpms_off_cm_idx = 0;
		rdev->pm.profiles[PM_PROFILE_HIGH_SH_IDX].dpms_on_cm_idx = 0;
		/* low mh */
		rdev->pm.profiles[PM_PROFILE_LOW_MH_IDX].dpms_off_ps_idx = 2;
		rdev->pm.profiles[PM_PROFILE_LOW_MH_IDX].dpms_on_ps_idx = 0;
		rdev->pm.profiles[PM_PROFILE_LOW_MH_IDX].dpms_off_cm_idx = 0;
		rdev->pm.profiles[PM_PROFILE_LOW_MH_IDX].dpms_on_cm_idx = 0;
		/* mid mh */
		rdev->pm.profiles[PM_PROFILE_MID_MH_IDX].dpms_off_ps_idx = 2;
		rdev->pm.profiles[PM_PROFILE_MID_MH_IDX].dpms_on_ps_idx = 0;
		rdev->pm.profiles[PM_PROFILE_MID_MH_IDX].dpms_off_cm_idx = 0;
		rdev->pm.profiles[PM_PROFILE_MID_MH_IDX].dpms_on_cm_idx = 0;
		/* high mh */
		rdev->pm.profiles[PM_PROFILE_HIGH_MH_IDX].dpms_off_ps_idx = 2;
		rdev->pm.profiles[PM_PROFILE_HIGH_MH_IDX].dpms_on_ps_idx = 3;
		rdev->pm.profiles[PM_PROFILE_HIGH_MH_IDX].dpms_off_cm_idx = 0;
		rdev->pm.profiles[PM_PROFILE_HIGH_MH_IDX].dpms_on_cm_idx = 0;
	}
}

void r600_pm_init_profile(struct radeon_device *rdev)
{
	int idx;

	if (rdev->family == CHIP_R600) {
		/* XXX */
		/* default */
		rdev->pm.profiles[PM_PROFILE_DEFAULT_IDX].dpms_off_ps_idx = rdev->pm.default_power_state_index;
		rdev->pm.profiles[PM_PROFILE_DEFAULT_IDX].dpms_on_ps_idx = rdev->pm.default_power_state_index;
		rdev->pm.profiles[PM_PROFILE_DEFAULT_IDX].dpms_off_cm_idx = 0;
		rdev->pm.profiles[PM_PROFILE_DEFAULT_IDX].dpms_on_cm_idx = 0;
		/* low sh */
		rdev->pm.profiles[PM_PROFILE_LOW_SH_IDX].dpms_off_ps_idx = rdev->pm.default_power_state_index;
		rdev->pm.profiles[PM_PROFILE_LOW_SH_IDX].dpms_on_ps_idx = rdev->pm.default_power_state_index;
		rdev->pm.profiles[PM_PROFILE_LOW_SH_IDX].dpms_off_cm_idx = 0;
		rdev->pm.profiles[PM_PROFILE_LOW_SH_IDX].dpms_on_cm_idx = 0;
		/* mid sh */
		rdev->pm.profiles[PM_PROFILE_MID_SH_IDX].dpms_off_ps_idx = rdev->pm.default_power_state_index;
		rdev->pm.profiles[PM_PROFILE_MID_SH_IDX].dpms_on_ps_idx = rdev->pm.default_power_state_index;
		rdev->pm.profiles[PM_PROFILE_MID_SH_IDX].dpms_off_cm_idx = 0;
		rdev->pm.profiles[PM_PROFILE_MID_SH_IDX].dpms_on_cm_idx = 0;
		/* high sh */
		rdev->pm.profiles[PM_PROFILE_HIGH_SH_IDX].dpms_off_ps_idx = rdev->pm.default_power_state_index;
		rdev->pm.profiles[PM_PROFILE_HIGH_SH_IDX].dpms_on_ps_idx = rdev->pm.default_power_state_index;
		rdev->pm.profiles[PM_PROFILE_HIGH_SH_IDX].dpms_off_cm_idx = 0;
		rdev->pm.profiles[PM_PROFILE_HIGH_SH_IDX].dpms_on_cm_idx = 0;
		/* low mh */
		rdev->pm.profiles[PM_PROFILE_LOW_MH_IDX].dpms_off_ps_idx = rdev->pm.default_power_state_index;
		rdev->pm.profiles[PM_PROFILE_LOW_MH_IDX].dpms_on_ps_idx = rdev->pm.default_power_state_index;
		rdev->pm.profiles[PM_PROFILE_LOW_MH_IDX].dpms_off_cm_idx = 0;
		rdev->pm.profiles[PM_PROFILE_LOW_MH_IDX].dpms_on_cm_idx = 0;
		/* mid mh */
		rdev->pm.profiles[PM_PROFILE_MID_MH_IDX].dpms_off_ps_idx = rdev->pm.default_power_state_index;
		rdev->pm.profiles[PM_PROFILE_MID_MH_IDX].dpms_on_ps_idx = rdev->pm.default_power_state_index;
		rdev->pm.profiles[PM_PROFILE_MID_MH_IDX].dpms_off_cm_idx = 0;
		rdev->pm.profiles[PM_PROFILE_MID_MH_IDX].dpms_on_cm_idx = 0;
		/* high mh */
		rdev->pm.profiles[PM_PROFILE_HIGH_MH_IDX].dpms_off_ps_idx = rdev->pm.default_power_state_index;
		rdev->pm.profiles[PM_PROFILE_HIGH_MH_IDX].dpms_on_ps_idx = rdev->pm.default_power_state_index;
		rdev->pm.profiles[PM_PROFILE_HIGH_MH_IDX].dpms_off_cm_idx = 0;
		rdev->pm.profiles[PM_PROFILE_HIGH_MH_IDX].dpms_on_cm_idx = 0;
	} else {
		if (rdev->pm.num_power_states < 4) {
			/* default */
			rdev->pm.profiles[PM_PROFILE_DEFAULT_IDX].dpms_off_ps_idx = rdev->pm.default_power_state_index;
			rdev->pm.profiles[PM_PROFILE_DEFAULT_IDX].dpms_on_ps_idx = rdev->pm.default_power_state_index;
			rdev->pm.profiles[PM_PROFILE_DEFAULT_IDX].dpms_off_cm_idx = 0;
			rdev->pm.profiles[PM_PROFILE_DEFAULT_IDX].dpms_on_cm_idx = 2;
			/* low sh */
			rdev->pm.profiles[PM_PROFILE_LOW_SH_IDX].dpms_off_ps_idx = 1;
			rdev->pm.profiles[PM_PROFILE_LOW_SH_IDX].dpms_on_ps_idx = 1;
			rdev->pm.profiles[PM_PROFILE_LOW_SH_IDX].dpms_off_cm_idx = 0;
			rdev->pm.profiles[PM_PROFILE_LOW_SH_IDX].dpms_on_cm_idx = 0;
			/* mid sh */
			rdev->pm.profiles[PM_PROFILE_MID_SH_IDX].dpms_off_ps_idx = 1;
			rdev->pm.profiles[PM_PROFILE_MID_SH_IDX].dpms_on_ps_idx = 1;
			rdev->pm.profiles[PM_PROFILE_MID_SH_IDX].dpms_off_cm_idx = 0;
			rdev->pm.profiles[PM_PROFILE_MID_SH_IDX].dpms_on_cm_idx = 1;
			/* high sh */
			rdev->pm.profiles[PM_PROFILE_HIGH_SH_IDX].dpms_off_ps_idx = 1;
			rdev->pm.profiles[PM_PROFILE_HIGH_SH_IDX].dpms_on_ps_idx = 1;
			rdev->pm.profiles[PM_PROFILE_HIGH_SH_IDX].dpms_off_cm_idx = 0;
			rdev->pm.profiles[PM_PROFILE_HIGH_SH_IDX].dpms_on_cm_idx = 2;
			/* low mh */
			rdev->pm.profiles[PM_PROFILE_LOW_MH_IDX].dpms_off_ps_idx = 2;
			rdev->pm.profiles[PM_PROFILE_LOW_MH_IDX].dpms_on_ps_idx = 2;
			rdev->pm.profiles[PM_PROFILE_LOW_MH_IDX].dpms_off_cm_idx = 0;
			rdev->pm.profiles[PM_PROFILE_LOW_MH_IDX].dpms_on_cm_idx = 0;
			/* low mh */
			rdev->pm.profiles[PM_PROFILE_MID_MH_IDX].dpms_off_ps_idx = 2;
			rdev->pm.profiles[PM_PROFILE_MID_MH_IDX].dpms_on_ps_idx = 2;
			rdev->pm.profiles[PM_PROFILE_MID_MH_IDX].dpms_off_cm_idx = 0;
			rdev->pm.profiles[PM_PROFILE_MID_MH_IDX].dpms_on_cm_idx = 1;
			/* high mh */
			rdev->pm.profiles[PM_PROFILE_HIGH_MH_IDX].dpms_off_ps_idx = 2;
			rdev->pm.profiles[PM_PROFILE_HIGH_MH_IDX].dpms_on_ps_idx = 2;
			rdev->pm.profiles[PM_PROFILE_HIGH_MH_IDX].dpms_off_cm_idx = 0;
			rdev->pm.profiles[PM_PROFILE_HIGH_MH_IDX].dpms_on_cm_idx = 2;
		} else {
			/* default */
			rdev->pm.profiles[PM_PROFILE_DEFAULT_IDX].dpms_off_ps_idx = rdev->pm.default_power_state_index;
			rdev->pm.profiles[PM_PROFILE_DEFAULT_IDX].dpms_on_ps_idx = rdev->pm.default_power_state_index;
			rdev->pm.profiles[PM_PROFILE_DEFAULT_IDX].dpms_off_cm_idx = 0;
			rdev->pm.profiles[PM_PROFILE_DEFAULT_IDX].dpms_on_cm_idx = 2;
			/* low sh */
			if (rdev->flags & RADEON_IS_MOBILITY)
				idx = radeon_pm_get_type_index(rdev, POWER_STATE_TYPE_BATTERY, 0);
			else
				idx = radeon_pm_get_type_index(rdev, POWER_STATE_TYPE_PERFORMANCE, 0);
			rdev->pm.profiles[PM_PROFILE_LOW_SH_IDX].dpms_off_ps_idx = idx;
			rdev->pm.profiles[PM_PROFILE_LOW_SH_IDX].dpms_on_ps_idx = idx;
			rdev->pm.profiles[PM_PROFILE_LOW_SH_IDX].dpms_off_cm_idx = 0;
			rdev->pm.profiles[PM_PROFILE_LOW_SH_IDX].dpms_on_cm_idx = 0;
			/* mid sh */
			rdev->pm.profiles[PM_PROFILE_MID_SH_IDX].dpms_off_ps_idx = idx;
			rdev->pm.profiles[PM_PROFILE_MID_SH_IDX].dpms_on_ps_idx = idx;
			rdev->pm.profiles[PM_PROFILE_MID_SH_IDX].dpms_off_cm_idx = 0;
			rdev->pm.profiles[PM_PROFILE_MID_SH_IDX].dpms_on_cm_idx = 1;
			/* high sh */
			idx = radeon_pm_get_type_index(rdev, POWER_STATE_TYPE_PERFORMANCE, 0);
			rdev->pm.profiles[PM_PROFILE_HIGH_SH_IDX].dpms_off_ps_idx = idx;
			rdev->pm.profiles[PM_PROFILE_HIGH_SH_IDX].dpms_on_ps_idx = idx;
			rdev->pm.profiles[PM_PROFILE_HIGH_SH_IDX].dpms_off_cm_idx = 0;
			rdev->pm.profiles[PM_PROFILE_HIGH_SH_IDX].dpms_on_cm_idx = 2;
			/* low mh */
			if (rdev->flags & RADEON_IS_MOBILITY)
				idx = radeon_pm_get_type_index(rdev, POWER_STATE_TYPE_BATTERY, 1);
			else
				idx = radeon_pm_get_type_index(rdev, POWER_STATE_TYPE_PERFORMANCE, 1);
			rdev->pm.profiles[PM_PROFILE_LOW_MH_IDX].dpms_off_ps_idx = idx;
			rdev->pm.profiles[PM_PROFILE_LOW_MH_IDX].dpms_on_ps_idx = idx;
			rdev->pm.profiles[PM_PROFILE_LOW_MH_IDX].dpms_off_cm_idx = 0;
			rdev->pm.profiles[PM_PROFILE_LOW_MH_IDX].dpms_on_cm_idx = 0;
			/* mid mh */
			rdev->pm.profiles[PM_PROFILE_MID_MH_IDX].dpms_off_ps_idx = idx;
			rdev->pm.profiles[PM_PROFILE_MID_MH_IDX].dpms_on_ps_idx = idx;
			rdev->pm.profiles[PM_PROFILE_MID_MH_IDX].dpms_off_cm_idx = 0;
			rdev->pm.profiles[PM_PROFILE_MID_MH_IDX].dpms_on_cm_idx = 1;
			/* high mh */
			idx = radeon_pm_get_type_index(rdev, POWER_STATE_TYPE_PERFORMANCE, 1);
			rdev->pm.profiles[PM_PROFILE_HIGH_MH_IDX].dpms_off_ps_idx = idx;
			rdev->pm.profiles[PM_PROFILE_HIGH_MH_IDX].dpms_on_ps_idx = idx;
			rdev->pm.profiles[PM_PROFILE_HIGH_MH_IDX].dpms_off_cm_idx = 0;
			rdev->pm.profiles[PM_PROFILE_HIGH_MH_IDX].dpms_on_cm_idx = 2;
		}
	}
}

void r600_pm_misc(struct radeon_device *rdev)
{
	int req_ps_idx = rdev->pm.requested_power_state_index;
	int req_cm_idx = rdev->pm.requested_clock_mode_index;
	struct radeon_power_state *ps = &rdev->pm.power_state[req_ps_idx];
	struct radeon_voltage *voltage = &ps->clock_info[req_cm_idx].voltage;

	if ((voltage->type == VOLTAGE_SW) && voltage->voltage) {
		/* 0xff01 is a flag rather then an actual voltage */
		if (voltage->voltage == 0xff01)
			return;
		if (voltage->voltage != rdev->pm.current_vddc) {
			radeon_atom_set_voltage(rdev, voltage->voltage, SET_VOLTAGE_TYPE_ASIC_VDDC);
			rdev->pm.current_vddc = voltage->voltage;
			DRM_DEBUG_DRIVER("Setting: v: %d\n", voltage->voltage);
		}
	}
}

bool r600_gui_idle(struct radeon_device *rdev)
{
	if (RREG32(GRBM_STATUS) & GUI_ACTIVE)
		return false;
	else
		return true;
}

/* hpd for digital panel detect/disconnect */
bool r600_hpd_sense(struct radeon_device *rdev, enum radeon_hpd_id hpd)
{
	bool connected = false;

	if (ASIC_IS_DCE3(rdev)) {
		switch (hpd) {
		case RADEON_HPD_1:
			if (RREG32(DC_HPD1_INT_STATUS) & DC_HPDx_SENSE)
				connected = true;
			break;
		case RADEON_HPD_2:
			if (RREG32(DC_HPD2_INT_STATUS) & DC_HPDx_SENSE)
				connected = true;
			break;
		case RADEON_HPD_3:
			if (RREG32(DC_HPD3_INT_STATUS) & DC_HPDx_SENSE)
				connected = true;
			break;
		case RADEON_HPD_4:
			if (RREG32(DC_HPD4_INT_STATUS) & DC_HPDx_SENSE)
				connected = true;
			break;
			/* DCE 3.2 */
		case RADEON_HPD_5:
			if (RREG32(DC_HPD5_INT_STATUS) & DC_HPDx_SENSE)
				connected = true;
			break;
		case RADEON_HPD_6:
			if (RREG32(DC_HPD6_INT_STATUS) & DC_HPDx_SENSE)
				connected = true;
			break;
		default:
			break;
		}
	} else {
		switch (hpd) {
		case RADEON_HPD_1:
			if (RREG32(DC_HOT_PLUG_DETECT1_INT_STATUS) & DC_HOT_PLUG_DETECTx_SENSE)
				connected = true;
			break;
		case RADEON_HPD_2:
			if (RREG32(DC_HOT_PLUG_DETECT2_INT_STATUS) & DC_HOT_PLUG_DETECTx_SENSE)
				connected = true;
			break;
		case RADEON_HPD_3:
			if (RREG32(DC_HOT_PLUG_DETECT3_INT_STATUS) & DC_HOT_PLUG_DETECTx_SENSE)
				connected = true;
			break;
		default:
			break;
		}
	}
	return connected;
}

void r600_hpd_set_polarity(struct radeon_device *rdev,
			   enum radeon_hpd_id hpd)
{
	u32 tmp;
	bool connected = r600_hpd_sense(rdev, hpd);

	if (ASIC_IS_DCE3(rdev)) {
		switch (hpd) {
		case RADEON_HPD_1:
			tmp = RREG32(DC_HPD1_INT_CONTROL);
			if (connected)
				tmp &= ~DC_HPDx_INT_POLARITY;
			else
				tmp |= DC_HPDx_INT_POLARITY;
			WREG32(DC_HPD1_INT_CONTROL, tmp);
			break;
		case RADEON_HPD_2:
			tmp = RREG32(DC_HPD2_INT_CONTROL);
			if (connected)
				tmp &= ~DC_HPDx_INT_POLARITY;
			else
				tmp |= DC_HPDx_INT_POLARITY;
			WREG32(DC_HPD2_INT_CONTROL, tmp);
			break;
		case RADEON_HPD_3:
			tmp = RREG32(DC_HPD3_INT_CONTROL);
			if (connected)
				tmp &= ~DC_HPDx_INT_POLARITY;
			else
				tmp |= DC_HPDx_INT_POLARITY;
			WREG32(DC_HPD3_INT_CONTROL, tmp);
			break;
		case RADEON_HPD_4:
			tmp = RREG32(DC_HPD4_INT_CONTROL);
			if (connected)
				tmp &= ~DC_HPDx_INT_POLARITY;
			else
				tmp |= DC_HPDx_INT_POLARITY;
			WREG32(DC_HPD4_INT_CONTROL, tmp);
			break;
		case RADEON_HPD_5:
			tmp = RREG32(DC_HPD5_INT_CONTROL);
			if (connected)
				tmp &= ~DC_HPDx_INT_POLARITY;
			else
				tmp |= DC_HPDx_INT_POLARITY;
			WREG32(DC_HPD5_INT_CONTROL, tmp);
			break;
			/* DCE 3.2 */
		case RADEON_HPD_6:
			tmp = RREG32(DC_HPD6_INT_CONTROL);
			if (connected)
				tmp &= ~DC_HPDx_INT_POLARITY;
			else
				tmp |= DC_HPDx_INT_POLARITY;
			WREG32(DC_HPD6_INT_CONTROL, tmp);
			break;
		default:
			break;
		}
	} else {
		switch (hpd) {
		case RADEON_HPD_1:
			tmp = RREG32(DC_HOT_PLUG_DETECT1_INT_CONTROL);
			if (connected)
				tmp &= ~DC_HOT_PLUG_DETECTx_INT_POLARITY;
			else
				tmp |= DC_HOT_PLUG_DETECTx_INT_POLARITY;
			WREG32(DC_HOT_PLUG_DETECT1_INT_CONTROL, tmp);
			break;
		case RADEON_HPD_2:
			tmp = RREG32(DC_HOT_PLUG_DETECT2_INT_CONTROL);
			if (connected)
				tmp &= ~DC_HOT_PLUG_DETECTx_INT_POLARITY;
			else
				tmp |= DC_HOT_PLUG_DETECTx_INT_POLARITY;
			WREG32(DC_HOT_PLUG_DETECT2_INT_CONTROL, tmp);
			break;
		case RADEON_HPD_3:
			tmp = RREG32(DC_HOT_PLUG_DETECT3_INT_CONTROL);
			if (connected)
				tmp &= ~DC_HOT_PLUG_DETECTx_INT_POLARITY;
			else
				tmp |= DC_HOT_PLUG_DETECTx_INT_POLARITY;
			WREG32(DC_HOT_PLUG_DETECT3_INT_CONTROL, tmp);
			break;
		default:
			break;
		}
	}
}

void r600_hpd_init(struct radeon_device *rdev)
{
	struct drm_device *dev = rdev->ddev;
	struct drm_connector *connector;
	unsigned enable = 0;

	list_for_each_entry(connector, &dev->mode_config.connector_list, head) {
		struct radeon_connector *radeon_connector = to_radeon_connector(connector);

		if (connector->connector_type == DRM_MODE_CONNECTOR_eDP ||
		    connector->connector_type == DRM_MODE_CONNECTOR_LVDS) {
			/* don't try to enable hpd on eDP or LVDS avoid breaking the
			 * aux dp channel on imac and help (but not completely fix)
			 * https://bugzilla.redhat.com/show_bug.cgi?id=726143
			 */
			continue;
		}
		if (ASIC_IS_DCE3(rdev)) {
			u32 tmp = DC_HPDx_CONNECTION_TIMER(0x9c4) | DC_HPDx_RX_INT_TIMER(0xfa);
			if (ASIC_IS_DCE32(rdev))
				tmp |= DC_HPDx_EN;

			switch (radeon_connector->hpd.hpd) {
			case RADEON_HPD_1:
				WREG32(DC_HPD1_CONTROL, tmp);
				break;
			case RADEON_HPD_2:
				WREG32(DC_HPD2_CONTROL, tmp);
				break;
			case RADEON_HPD_3:
				WREG32(DC_HPD3_CONTROL, tmp);
				break;
			case RADEON_HPD_4:
				WREG32(DC_HPD4_CONTROL, tmp);
				break;
				/* DCE 3.2 */
			case RADEON_HPD_5:
				WREG32(DC_HPD5_CONTROL, tmp);
				break;
			case RADEON_HPD_6:
				WREG32(DC_HPD6_CONTROL, tmp);
				break;
			default:
				break;
			}
		} else {
			switch (radeon_connector->hpd.hpd) {
			case RADEON_HPD_1:
				WREG32(DC_HOT_PLUG_DETECT1_CONTROL, DC_HOT_PLUG_DETECTx_EN);
				break;
			case RADEON_HPD_2:
				WREG32(DC_HOT_PLUG_DETECT2_CONTROL, DC_HOT_PLUG_DETECTx_EN);
				break;
			case RADEON_HPD_3:
				WREG32(DC_HOT_PLUG_DETECT3_CONTROL, DC_HOT_PLUG_DETECTx_EN);
				break;
			default:
				break;
			}
		}
		enable |= 1 << radeon_connector->hpd.hpd;
		radeon_hpd_set_polarity(rdev, radeon_connector->hpd.hpd);
	}
	radeon_irq_kms_enable_hpd(rdev, enable);
}

void r600_hpd_fini(struct radeon_device *rdev)
{
	struct drm_device *dev = rdev->ddev;
	struct drm_connector *connector;
	unsigned disable = 0;

	list_for_each_entry(connector, &dev->mode_config.connector_list, head) {
		struct radeon_connector *radeon_connector = to_radeon_connector(connector);
		if (ASIC_IS_DCE3(rdev)) {
			switch (radeon_connector->hpd.hpd) {
			case RADEON_HPD_1:
				WREG32(DC_HPD1_CONTROL, 0);
				break;
			case RADEON_HPD_2:
				WREG32(DC_HPD2_CONTROL, 0);
				break;
			case RADEON_HPD_3:
				WREG32(DC_HPD3_CONTROL, 0);
				break;
			case RADEON_HPD_4:
				WREG32(DC_HPD4_CONTROL, 0);
				break;
				/* DCE 3.2 */
			case RADEON_HPD_5:
				WREG32(DC_HPD5_CONTROL, 0);
				break;
			case RADEON_HPD_6:
				WREG32(DC_HPD6_CONTROL, 0);
				break;
			default:
				break;
			}
		} else {
			switch (radeon_connector->hpd.hpd) {
			case RADEON_HPD_1:
				WREG32(DC_HOT_PLUG_DETECT1_CONTROL, 0);
				break;
			case RADEON_HPD_2:
				WREG32(DC_HOT_PLUG_DETECT2_CONTROL, 0);
				break;
			case RADEON_HPD_3:
				WREG32(DC_HOT_PLUG_DETECT3_CONTROL, 0);
				break;
			default:
				break;
			}
		}
		disable |= 1 << radeon_connector->hpd.hpd;
	}
	radeon_irq_kms_disable_hpd(rdev, disable);
}

/*
 * R600 PCIE GART
 */
void r600_pcie_gart_tlb_flush(struct radeon_device *rdev)
{
	unsigned i;
	u32 tmp;

	/* flush hdp cache so updates hit vram */
	if ((rdev->family >= CHIP_RV770) && (rdev->family <= CHIP_RV740) &&
	    !(rdev->flags & RADEON_IS_AGP)) {
		volatile uint32_t *ptr = rdev->gart.ptr;
		u32 tmp;

		/* r7xx hw bug.  write to HDP_DEBUG1 followed by fb read
		 * rather than write to HDP_REG_COHERENCY_FLUSH_CNTL
		 * This seems to cause problems on some AGP cards. Just use the old
		 * method for them.
		 */
		WREG32(HDP_DEBUG1, 0);
		tmp = *ptr;
	} else
		WREG32(R_005480_HDP_MEM_COHERENCY_FLUSH_CNTL, 0x1);

	WREG32(VM_CONTEXT0_INVALIDATION_LOW_ADDR, rdev->mc.gtt_start >> 12);
	WREG32(VM_CONTEXT0_INVALIDATION_HIGH_ADDR, (rdev->mc.gtt_end - 1) >> 12);
	WREG32(VM_CONTEXT0_REQUEST_RESPONSE, REQUEST_TYPE(1));
	for (i = 0; i < rdev->usec_timeout; i++) {
		/* read MC_STATUS */
		tmp = RREG32(VM_CONTEXT0_REQUEST_RESPONSE);
		tmp = (tmp & RESPONSE_TYPE_MASK) >> RESPONSE_TYPE_SHIFT;
		if (tmp == 2) {
			printk(KERN_WARNING "[drm] r600 flush TLB failed\n");
			return;
		}
		if (tmp) {
			return;
		}
		udelay(1);
	}
}

int r600_pcie_gart_init(struct radeon_device *rdev)
{
	int r;

	if (rdev->gart.robj) {
		WARN(1, "R600 PCIE GART already initialized\n");
		return 0;
	}
	/* Initialize common gart structure */
	r = radeon_gart_init(rdev);
	if (r)
		return r;
	rdev->gart.table_size = rdev->gart.num_gpu_pages * 8;
	return radeon_gart_table_vram_alloc(rdev);
}

static int r600_pcie_gart_enable(struct radeon_device *rdev)
{
	u32 tmp;
	int r, i;

	if (rdev->gart.robj == NULL) {
		dev_err(rdev->dev, "No VRAM object for PCIE GART.\n");
		return -EINVAL;
	}
	r = radeon_gart_table_vram_pin(rdev);
	if (r)
		return r;
	radeon_gart_restore(rdev);

	/* Setup L2 cache */
	WREG32(VM_L2_CNTL, ENABLE_L2_CACHE | ENABLE_L2_FRAGMENT_PROCESSING |
				ENABLE_L2_PTE_CACHE_LRU_UPDATE_BY_WRITE |
				EFFECTIVE_L2_QUEUE_SIZE(7));
	WREG32(VM_L2_CNTL2, 0);
	WREG32(VM_L2_CNTL3, BANK_SELECT_0(0) | BANK_SELECT_1(1));
	/* Setup TLB control */
	tmp = ENABLE_L1_TLB | ENABLE_L1_FRAGMENT_PROCESSING |
		SYSTEM_ACCESS_MODE_NOT_IN_SYS |
		EFFECTIVE_L1_TLB_SIZE(5) | EFFECTIVE_L1_QUEUE_SIZE(5) |
		ENABLE_WAIT_L2_QUERY;
	WREG32(MC_VM_L1_TLB_MCB_RD_SYS_CNTL, tmp);
	WREG32(MC_VM_L1_TLB_MCB_WR_SYS_CNTL, tmp);
	WREG32(MC_VM_L1_TLB_MCB_RD_HDP_CNTL, tmp | ENABLE_L1_STRICT_ORDERING);
	WREG32(MC_VM_L1_TLB_MCB_WR_HDP_CNTL, tmp);
	WREG32(MC_VM_L1_TLB_MCD_RD_A_CNTL, tmp);
	WREG32(MC_VM_L1_TLB_MCD_WR_A_CNTL, tmp);
	WREG32(MC_VM_L1_TLB_MCD_RD_B_CNTL, tmp);
	WREG32(MC_VM_L1_TLB_MCD_WR_B_CNTL, tmp);
	WREG32(MC_VM_L1_TLB_MCB_RD_GFX_CNTL, tmp);
	WREG32(MC_VM_L1_TLB_MCB_WR_GFX_CNTL, tmp);
	WREG32(MC_VM_L1_TLB_MCB_RD_PDMA_CNTL, tmp);
	WREG32(MC_VM_L1_TLB_MCB_WR_PDMA_CNTL, tmp);
	WREG32(MC_VM_L1_TLB_MCB_RD_SEM_CNTL, tmp | ENABLE_SEMAPHORE_MODE);
	WREG32(MC_VM_L1_TLB_MCB_WR_SEM_CNTL, tmp | ENABLE_SEMAPHORE_MODE);
	WREG32(VM_CONTEXT0_PAGE_TABLE_START_ADDR, rdev->mc.gtt_start >> 12);
	WREG32(VM_CONTEXT0_PAGE_TABLE_END_ADDR, rdev->mc.gtt_end >> 12);
	WREG32(VM_CONTEXT0_PAGE_TABLE_BASE_ADDR, rdev->gart.table_addr >> 12);
	WREG32(VM_CONTEXT0_CNTL, ENABLE_CONTEXT | PAGE_TABLE_DEPTH(0) |
				RANGE_PROTECTION_FAULT_ENABLE_DEFAULT);
	WREG32(VM_CONTEXT0_PROTECTION_FAULT_DEFAULT_ADDR,
			(u32)(rdev->dummy_page.addr >> 12));
	for (i = 1; i < 7; i++)
		WREG32(VM_CONTEXT0_CNTL + (i * 4), 0);

	r600_pcie_gart_tlb_flush(rdev);
	DRM_INFO("PCIE GART of %uM enabled (table at 0x%016llX).\n",
		 (unsigned)(rdev->mc.gtt_size >> 20),
		 (unsigned long long)rdev->gart.table_addr);
	rdev->gart.ready = true;
	return 0;
}

static void r600_pcie_gart_disable(struct radeon_device *rdev)
{
	u32 tmp;
	int i;

	/* Disable all tables */
	for (i = 0; i < 7; i++)
		WREG32(VM_CONTEXT0_CNTL + (i * 4), 0);

	/* Disable L2 cache */
	WREG32(VM_L2_CNTL, ENABLE_L2_FRAGMENT_PROCESSING |
				EFFECTIVE_L2_QUEUE_SIZE(7));
	WREG32(VM_L2_CNTL3, BANK_SELECT_0(0) | BANK_SELECT_1(1));
	/* Setup L1 TLB control */
	tmp = EFFECTIVE_L1_TLB_SIZE(5) | EFFECTIVE_L1_QUEUE_SIZE(5) |
		ENABLE_WAIT_L2_QUERY;
	WREG32(MC_VM_L1_TLB_MCD_RD_A_CNTL, tmp);
	WREG32(MC_VM_L1_TLB_MCD_WR_A_CNTL, tmp);
	WREG32(MC_VM_L1_TLB_MCD_RD_B_CNTL, tmp);
	WREG32(MC_VM_L1_TLB_MCD_WR_B_CNTL, tmp);
	WREG32(MC_VM_L1_TLB_MCB_RD_GFX_CNTL, tmp);
	WREG32(MC_VM_L1_TLB_MCB_WR_GFX_CNTL, tmp);
	WREG32(MC_VM_L1_TLB_MCB_RD_PDMA_CNTL, tmp);
	WREG32(MC_VM_L1_TLB_MCB_WR_PDMA_CNTL, tmp);
	WREG32(MC_VM_L1_TLB_MCB_RD_SEM_CNTL, tmp);
	WREG32(MC_VM_L1_TLB_MCB_WR_SEM_CNTL, tmp);
	WREG32(MC_VM_L1_TLB_MCB_RD_SYS_CNTL, tmp);
	WREG32(MC_VM_L1_TLB_MCB_WR_SYS_CNTL, tmp);
	WREG32(MC_VM_L1_TLB_MCB_RD_HDP_CNTL, tmp);
	WREG32(MC_VM_L1_TLB_MCB_WR_HDP_CNTL, tmp);
	radeon_gart_table_vram_unpin(rdev);
}

static void r600_pcie_gart_fini(struct radeon_device *rdev)
{
	radeon_gart_fini(rdev);
	r600_pcie_gart_disable(rdev);
	radeon_gart_table_vram_free(rdev);
}

static void r600_agp_enable(struct radeon_device *rdev)
{
	u32 tmp;
	int i;

	/* Setup L2 cache */
	WREG32(VM_L2_CNTL, ENABLE_L2_CACHE | ENABLE_L2_FRAGMENT_PROCESSING |
				ENABLE_L2_PTE_CACHE_LRU_UPDATE_BY_WRITE |
				EFFECTIVE_L2_QUEUE_SIZE(7));
	WREG32(VM_L2_CNTL2, 0);
	WREG32(VM_L2_CNTL3, BANK_SELECT_0(0) | BANK_SELECT_1(1));
	/* Setup TLB control */
	tmp = ENABLE_L1_TLB | ENABLE_L1_FRAGMENT_PROCESSING |
		SYSTEM_ACCESS_MODE_NOT_IN_SYS |
		EFFECTIVE_L1_TLB_SIZE(5) | EFFECTIVE_L1_QUEUE_SIZE(5) |
		ENABLE_WAIT_L2_QUERY;
	WREG32(MC_VM_L1_TLB_MCB_RD_SYS_CNTL, tmp);
	WREG32(MC_VM_L1_TLB_MCB_WR_SYS_CNTL, tmp);
	WREG32(MC_VM_L1_TLB_MCB_RD_HDP_CNTL, tmp | ENABLE_L1_STRICT_ORDERING);
	WREG32(MC_VM_L1_TLB_MCB_WR_HDP_CNTL, tmp);
	WREG32(MC_VM_L1_TLB_MCD_RD_A_CNTL, tmp);
	WREG32(MC_VM_L1_TLB_MCD_WR_A_CNTL, tmp);
	WREG32(MC_VM_L1_TLB_MCD_RD_B_CNTL, tmp);
	WREG32(MC_VM_L1_TLB_MCD_WR_B_CNTL, tmp);
	WREG32(MC_VM_L1_TLB_MCB_RD_GFX_CNTL, tmp);
	WREG32(MC_VM_L1_TLB_MCB_WR_GFX_CNTL, tmp);
	WREG32(MC_VM_L1_TLB_MCB_RD_PDMA_CNTL, tmp);
	WREG32(MC_VM_L1_TLB_MCB_WR_PDMA_CNTL, tmp);
	WREG32(MC_VM_L1_TLB_MCB_RD_SEM_CNTL, tmp | ENABLE_SEMAPHORE_MODE);
	WREG32(MC_VM_L1_TLB_MCB_WR_SEM_CNTL, tmp | ENABLE_SEMAPHORE_MODE);
	for (i = 0; i < 7; i++)
		WREG32(VM_CONTEXT0_CNTL + (i * 4), 0);
}

int r600_mc_wait_for_idle(struct radeon_device *rdev)
{
	unsigned i;
	u32 tmp;

	for (i = 0; i < rdev->usec_timeout; i++) {
		/* read MC_STATUS */
		tmp = RREG32(R_000E50_SRBM_STATUS) & 0x3F00;
		if (!tmp)
			return 0;
		udelay(1);
	}
	return -1;
}

static void r600_mc_program(struct radeon_device *rdev)
{
	struct rv515_mc_save save;
	u32 tmp;
	int i, j;

	/* Initialize HDP */
	for (i = 0, j = 0; i < 32; i++, j += 0x18) {
		WREG32((0x2c14 + j), 0x00000000);
		WREG32((0x2c18 + j), 0x00000000);
		WREG32((0x2c1c + j), 0x00000000);
		WREG32((0x2c20 + j), 0x00000000);
		WREG32((0x2c24 + j), 0x00000000);
	}
	WREG32(HDP_REG_COHERENCY_FLUSH_CNTL, 0);

	rv515_mc_stop(rdev, &save);
	if (r600_mc_wait_for_idle(rdev)) {
		dev_warn(rdev->dev, "Wait for MC idle timedout !\n");
	}
	/* Lockout access through VGA aperture (doesn't exist before R600) */
	WREG32(VGA_HDP_CONTROL, VGA_MEMORY_DISABLE);
	/* Update configuration */
	if (rdev->flags & RADEON_IS_AGP) {
		if (rdev->mc.vram_start < rdev->mc.gtt_start) {
			/* VRAM before AGP */
			WREG32(MC_VM_SYSTEM_APERTURE_LOW_ADDR,
				rdev->mc.vram_start >> 12);
			WREG32(MC_VM_SYSTEM_APERTURE_HIGH_ADDR,
				rdev->mc.gtt_end >> 12);
		} else {
			/* VRAM after AGP */
			WREG32(MC_VM_SYSTEM_APERTURE_LOW_ADDR,
				rdev->mc.gtt_start >> 12);
			WREG32(MC_VM_SYSTEM_APERTURE_HIGH_ADDR,
				rdev->mc.vram_end >> 12);
		}
	} else {
		WREG32(MC_VM_SYSTEM_APERTURE_LOW_ADDR, rdev->mc.vram_start >> 12);
		WREG32(MC_VM_SYSTEM_APERTURE_HIGH_ADDR, rdev->mc.vram_end >> 12);
	}
	WREG32(MC_VM_SYSTEM_APERTURE_DEFAULT_ADDR, rdev->vram_scratch.gpu_addr >> 12);
	tmp = ((rdev->mc.vram_end >> 24) & 0xFFFF) << 16;
	tmp |= ((rdev->mc.vram_start >> 24) & 0xFFFF);
	WREG32(MC_VM_FB_LOCATION, tmp);
	WREG32(HDP_NONSURFACE_BASE, (rdev->mc.vram_start >> 8));
	WREG32(HDP_NONSURFACE_INFO, (2 << 7));
	WREG32(HDP_NONSURFACE_SIZE, 0x3FFFFFFF);
	if (rdev->flags & RADEON_IS_AGP) {
		WREG32(MC_VM_AGP_TOP, rdev->mc.gtt_end >> 22);
		WREG32(MC_VM_AGP_BOT, rdev->mc.gtt_start >> 22);
		WREG32(MC_VM_AGP_BASE, rdev->mc.agp_base >> 22);
	} else {
		WREG32(MC_VM_AGP_BASE, 0);
		WREG32(MC_VM_AGP_TOP, 0x0FFFFFFF);
		WREG32(MC_VM_AGP_BOT, 0x0FFFFFFF);
	}
	if (r600_mc_wait_for_idle(rdev)) {
		dev_warn(rdev->dev, "Wait for MC idle timedout !\n");
	}
	rv515_mc_resume(rdev, &save);
	/* we need to own VRAM, so turn off the VGA renderer here
	 * to stop it overwriting our objects */
	rv515_vga_render_disable(rdev);
}

/**
 * r600_vram_gtt_location - try to find VRAM & GTT location
 * @rdev: radeon device structure holding all necessary informations
 * @mc: memory controller structure holding memory informations
 *
 * Function will place try to place VRAM at same place as in CPU (PCI)
 * address space as some GPU seems to have issue when we reprogram at
 * different address space.
 *
 * If there is not enough space to fit the unvisible VRAM after the
 * aperture then we limit the VRAM size to the aperture.
 *
 * If we are using AGP then place VRAM adjacent to AGP aperture are we need
 * them to be in one from GPU point of view so that we can program GPU to
 * catch access outside them (weird GPU policy see ??).
 *
 * This function will never fails, worst case are limiting VRAM or GTT.
 *
 * Note: GTT start, end, size should be initialized before calling this
 * function on AGP platform.
 */
void
r600_vram_gtt_location(struct radeon_device *rdev, struct radeon_mc *mc)
{
	u64 size_bf, size_af;

	if (mc->mc_vram_size > 0xE0000000) {
		/* leave room for at least 512M GTT */
		dev_warn(rdev->dev, "limiting VRAM\n");
		mc->real_vram_size = 0xE0000000;
		mc->mc_vram_size = 0xE0000000;
	}
	if (rdev->flags & RADEON_IS_AGP) {
		size_bf = mc->gtt_start;
		size_af = 0xFFFFFFFF - mc->gtt_end;
		if (size_bf > size_af) {
			if (mc->mc_vram_size > size_bf) {
				dev_warn(rdev->dev, "limiting VRAM\n");
				mc->real_vram_size = size_bf;
				mc->mc_vram_size = size_bf;
			}
			mc->vram_start = mc->gtt_start - mc->mc_vram_size;
		} else {
			if (mc->mc_vram_size > size_af) {
				dev_warn(rdev->dev, "limiting VRAM\n");
				mc->real_vram_size = size_af;
				mc->mc_vram_size = size_af;
			}
			mc->vram_start = mc->gtt_end + 1;
		}
		mc->vram_end = mc->vram_start + mc->mc_vram_size - 1;
		dev_info(rdev->dev, "VRAM: %lluM 0x%08llX - 0x%08llX (%lluM used)\n",
				mc->mc_vram_size >> 20, mc->vram_start,
				mc->vram_end, mc->real_vram_size >> 20);
	} else {
		u64 base = 0;
		if (rdev->flags & RADEON_IS_IGP) {
			base = RREG32(MC_VM_FB_LOCATION) & 0xFFFF;
			base <<= 24;
		}
		radeon_vram_location(rdev, &rdev->mc, base);
		rdev->mc.gtt_base_align = 0;
		radeon_gtt_location(rdev, mc);
	}
}

static int r600_mc_init(struct radeon_device *rdev)
{
	u32 tmp;
	int chansize, numchan;

	/* Get VRAM informations */
	rdev->mc.vram_is_ddr = true;
	tmp = RREG32(RAMCFG);
	if (tmp & CHANSIZE_OVERRIDE) {
		chansize = 16;
	} else if (tmp & CHANSIZE_MASK) {
		chansize = 64;
	} else {
		chansize = 32;
	}
	tmp = RREG32(CHMAP);
	switch ((tmp & NOOFCHAN_MASK) >> NOOFCHAN_SHIFT) {
	case 0:
	default:
		numchan = 1;
		break;
	case 1:
		numchan = 2;
		break;
	case 2:
		numchan = 4;
		break;
	case 3:
		numchan = 8;
		break;
	}
	rdev->mc.vram_width = numchan * chansize;
	/* Could aper size report 0 ? */
	rdev->mc.aper_base = rdev->fb_aper_offset;
	rdev->mc.aper_size = rdev->fb_aper_size;
	/* Setup GPU memory space */
	rdev->mc.mc_vram_size = RREG32(CONFIG_MEMSIZE);
	rdev->mc.real_vram_size = RREG32(CONFIG_MEMSIZE);
	rdev->mc.visible_vram_size = rdev->mc.aper_size;
	r600_vram_gtt_location(rdev, &rdev->mc);

	if (rdev->flags & RADEON_IS_IGP) {
		rs690_pm_info(rdev);
		rdev->mc.igp_sideport_enabled = radeon_atombios_sideport_present(rdev);
	}
	radeon_update_bandwidth_info(rdev);
	return 0;
}

int r600_vram_scratch_init(struct radeon_device *rdev)
{
	int r;

	if (rdev->vram_scratch.robj == NULL) {
		r = radeon_bo_create(rdev, RADEON_GPU_PAGE_SIZE,
				     PAGE_SIZE, true, RADEON_GEM_DOMAIN_VRAM,
				     NULL, &rdev->vram_scratch.robj);
		if (r) {
			return r;
		}
	}

	r = radeon_bo_reserve(rdev->vram_scratch.robj, false);
	if (unlikely(r != 0))
		return r;
	r = radeon_bo_pin(rdev->vram_scratch.robj,
			  RADEON_GEM_DOMAIN_VRAM, &rdev->vram_scratch.gpu_addr);
	if (r) {
		radeon_bo_unreserve(rdev->vram_scratch.robj);
		return r;
	}
	r = radeon_bo_kmap(rdev->vram_scratch.robj,
				(void **)&rdev->vram_scratch.ptr);
	if (r)
		radeon_bo_unpin(rdev->vram_scratch.robj);
	radeon_bo_unreserve(rdev->vram_scratch.robj);

	return r;
}

void r600_vram_scratch_fini(struct radeon_device *rdev)
{
	int r;

	if (rdev->vram_scratch.robj == NULL) {
		return;
	}
	r = radeon_bo_reserve(rdev->vram_scratch.robj, false);
	if (likely(r == 0)) {
		radeon_bo_kunmap(rdev->vram_scratch.robj);
		radeon_bo_unpin(rdev->vram_scratch.robj);
		radeon_bo_unreserve(rdev->vram_scratch.robj);
	}
	radeon_bo_unref(&rdev->vram_scratch.robj);
}

/* We doesn't check that the GPU really needs a reset we simply do the
 * reset, it's up to the caller to determine if the GPU needs one. We
 * might add an helper function to check that.
 */
static void r600_gpu_soft_reset_gfx(struct radeon_device *rdev)
{
	u32 grbm_busy_mask = S_008010_VC_BUSY(1) | S_008010_VGT_BUSY_NO_DMA(1) |
				S_008010_VGT_BUSY(1) | S_008010_TA03_BUSY(1) |
				S_008010_TC_BUSY(1) | S_008010_SX_BUSY(1) |
				S_008010_SH_BUSY(1) | S_008010_SPI03_BUSY(1) |
				S_008010_SMX_BUSY(1) | S_008010_SC_BUSY(1) |
				S_008010_PA_BUSY(1) | S_008010_DB03_BUSY(1) |
				S_008010_CR_BUSY(1) | S_008010_CB03_BUSY(1) |
				S_008010_GUI_ACTIVE(1);
	u32 grbm2_busy_mask = S_008014_SPI0_BUSY(1) | S_008014_SPI1_BUSY(1) |
			S_008014_SPI2_BUSY(1) | S_008014_SPI3_BUSY(1) |
			S_008014_TA0_BUSY(1) | S_008014_TA1_BUSY(1) |
			S_008014_TA2_BUSY(1) | S_008014_TA3_BUSY(1) |
			S_008014_DB0_BUSY(1) | S_008014_DB1_BUSY(1) |
			S_008014_DB2_BUSY(1) | S_008014_DB3_BUSY(1) |
			S_008014_CB0_BUSY(1) | S_008014_CB1_BUSY(1) |
			S_008014_CB2_BUSY(1) | S_008014_CB3_BUSY(1);
	u32 tmp;

	if (!(RREG32(GRBM_STATUS) & GUI_ACTIVE))
		return;

	dev_info(rdev->dev, "  R_008010_GRBM_STATUS      = 0x%08X\n",
		RREG32(R_008010_GRBM_STATUS));
	dev_info(rdev->dev, "  R_008014_GRBM_STATUS2     = 0x%08X\n",
		RREG32(R_008014_GRBM_STATUS2));
	dev_info(rdev->dev, "  R_000E50_SRBM_STATUS      = 0x%08X\n",
		RREG32(R_000E50_SRBM_STATUS));
	dev_info(rdev->dev, "  R_008674_CP_STALLED_STAT1 = 0x%08X\n",
		RREG32(CP_STALLED_STAT1));
	dev_info(rdev->dev, "  R_008678_CP_STALLED_STAT2 = 0x%08X\n",
		RREG32(CP_STALLED_STAT2));
	dev_info(rdev->dev, "  R_00867C_CP_BUSY_STAT     = 0x%08X\n",
		RREG32(CP_BUSY_STAT));
	dev_info(rdev->dev, "  R_008680_CP_STAT          = 0x%08X\n",
		RREG32(CP_STAT));

	/* Disable CP parsing/prefetching */
	WREG32(R_0086D8_CP_ME_CNTL, S_0086D8_CP_ME_HALT(1));

	/* Check if any of the rendering block is busy and reset it */
	if ((RREG32(R_008010_GRBM_STATUS) & grbm_busy_mask) ||
	    (RREG32(R_008014_GRBM_STATUS2) & grbm2_busy_mask)) {
		tmp = S_008020_SOFT_RESET_CR(1) |
			S_008020_SOFT_RESET_DB(1) |
			S_008020_SOFT_RESET_CB(1) |
			S_008020_SOFT_RESET_PA(1) |
			S_008020_SOFT_RESET_SC(1) |
			S_008020_SOFT_RESET_SMX(1) |
			S_008020_SOFT_RESET_SPI(1) |
			S_008020_SOFT_RESET_SX(1) |
			S_008020_SOFT_RESET_SH(1) |
			S_008020_SOFT_RESET_TC(1) |
			S_008020_SOFT_RESET_TA(1) |
			S_008020_SOFT_RESET_VC(1) |
			S_008020_SOFT_RESET_VGT(1);
		dev_info(rdev->dev, "  R_008020_GRBM_SOFT_RESET=0x%08X\n", tmp);
		WREG32(R_008020_GRBM_SOFT_RESET, tmp);
		RREG32(R_008020_GRBM_SOFT_RESET);
		mdelay(15);
		WREG32(R_008020_GRBM_SOFT_RESET, 0);
	}
	/* Reset CP (we always reset CP) */
	tmp = S_008020_SOFT_RESET_CP(1);
	dev_info(rdev->dev, "R_008020_GRBM_SOFT_RESET=0x%08X\n", tmp);
	WREG32(R_008020_GRBM_SOFT_RESET, tmp);
	RREG32(R_008020_GRBM_SOFT_RESET);
	mdelay(15);
	WREG32(R_008020_GRBM_SOFT_RESET, 0);

	dev_info(rdev->dev, "  R_008010_GRBM_STATUS      = 0x%08X\n",
		RREG32(R_008010_GRBM_STATUS));
	dev_info(rdev->dev, "  R_008014_GRBM_STATUS2     = 0x%08X\n",
		RREG32(R_008014_GRBM_STATUS2));
	dev_info(rdev->dev, "  R_000E50_SRBM_STATUS      = 0x%08X\n",
		RREG32(R_000E50_SRBM_STATUS));
	dev_info(rdev->dev, "  R_008674_CP_STALLED_STAT1 = 0x%08X\n",
		RREG32(CP_STALLED_STAT1));
	dev_info(rdev->dev, "  R_008678_CP_STALLED_STAT2 = 0x%08X\n",
		RREG32(CP_STALLED_STAT2));
	dev_info(rdev->dev, "  R_00867C_CP_BUSY_STAT     = 0x%08X\n",
		RREG32(CP_BUSY_STAT));
	dev_info(rdev->dev, "  R_008680_CP_STAT          = 0x%08X\n",
		RREG32(CP_STAT));

}

static void r600_gpu_soft_reset_dma(struct radeon_device *rdev)
{
	u32 tmp;

	if (RREG32(DMA_STATUS_REG) & DMA_IDLE)
		return;

	dev_info(rdev->dev, "  R_00D034_DMA_STATUS_REG   = 0x%08X\n",
		RREG32(DMA_STATUS_REG));

	/* Disable DMA */
	tmp = RREG32(DMA_RB_CNTL);
	tmp &= ~DMA_RB_ENABLE;
	WREG32(DMA_RB_CNTL, tmp);

	/* Reset dma */
	if (rdev->family >= CHIP_RV770)
		WREG32(SRBM_SOFT_RESET, RV770_SOFT_RESET_DMA);
	else
		WREG32(SRBM_SOFT_RESET, SOFT_RESET_DMA);
	RREG32(SRBM_SOFT_RESET);
	udelay(50);
	WREG32(SRBM_SOFT_RESET, 0);

	dev_info(rdev->dev, "  R_00D034_DMA_STATUS_REG   = 0x%08X\n",
		RREG32(DMA_STATUS_REG));
}

static int r600_gpu_soft_reset(struct radeon_device *rdev, u32 reset_mask)
{
	struct rv515_mc_save save;

	if (!(RREG32(GRBM_STATUS) & GUI_ACTIVE))
		reset_mask &= ~(RADEON_RESET_GFX | RADEON_RESET_COMPUTE);

	if (RREG32(DMA_STATUS_REG) & DMA_IDLE)
		reset_mask &= ~RADEON_RESET_DMA;

	if (reset_mask == 0)
		return 0;

	dev_info(rdev->dev, "GPU softreset: 0x%08X\n", reset_mask);

	rv515_mc_stop(rdev, &save);
	if (r600_mc_wait_for_idle(rdev)) {
		dev_warn(rdev->dev, "Wait for MC idle timedout !\n");
	}

	if (reset_mask & (RADEON_RESET_GFX | RADEON_RESET_COMPUTE))
		r600_gpu_soft_reset_gfx(rdev);

	if (reset_mask & RADEON_RESET_DMA)
		r600_gpu_soft_reset_dma(rdev);

	/* Wait a little for things to settle down */
	mdelay(1);

	rv515_mc_resume(rdev, &save);
	return 0;
}

bool r600_gpu_is_lockup(struct radeon_device *rdev, struct radeon_ring *ring)
{
	u32 srbm_status;
	u32 grbm_status;
	u32 grbm_status2;

	srbm_status = RREG32(R_000E50_SRBM_STATUS);
	grbm_status = RREG32(R_008010_GRBM_STATUS);
	grbm_status2 = RREG32(R_008014_GRBM_STATUS2);
	if (!G_008010_GUI_ACTIVE(grbm_status)) {
		radeon_ring_lockup_update(ring);
		return false;
	}
	/* force CP activities */
	radeon_ring_force_activity(rdev, ring);
	return radeon_ring_test_lockup(rdev, ring);
}

/**
 * r600_dma_is_lockup - Check if the DMA engine is locked up
 *
 * @rdev: radeon_device pointer
 * @ring: radeon_ring structure holding ring information
 *
 * Check if the async DMA engine is locked up (r6xx-evergreen).
 * Returns true if the engine appears to be locked up, false if not.
 */
bool r600_dma_is_lockup(struct radeon_device *rdev, struct radeon_ring *ring)
{
	u32 dma_status_reg;

	dma_status_reg = RREG32(DMA_STATUS_REG);
	if (dma_status_reg & DMA_IDLE) {
		radeon_ring_lockup_update(ring);
		return false;
	}
	/* force ring activities */
	radeon_ring_force_activity(rdev, ring);
	return radeon_ring_test_lockup(rdev, ring);
}

int r600_asic_reset(struct radeon_device *rdev)
{
	return r600_gpu_soft_reset(rdev, (RADEON_RESET_GFX |
					  RADEON_RESET_COMPUTE |
					  RADEON_RESET_DMA));
}

u32 r6xx_remap_render_backend(struct radeon_device *rdev,
			      u32 tiling_pipe_num,
			      u32 max_rb_num,
			      u32 total_max_rb_num,
			      u32 disabled_rb_mask)
{
	u32 rendering_pipe_num, rb_num_width, req_rb_num;
	u32 pipe_rb_ratio, pipe_rb_remain, tmp;
	u32 data = 0, mask = 1 << (max_rb_num - 1);
	unsigned i, j;

	/* mask out the RBs that don't exist on that asic */
	tmp = disabled_rb_mask | ((0xff << max_rb_num) & 0xff);
	/* make sure at least one RB is available */
	if ((tmp & 0xff) != 0xff)
		disabled_rb_mask = tmp;

	rendering_pipe_num = 1 << tiling_pipe_num;
	req_rb_num = total_max_rb_num - r600_count_pipe_bits(disabled_rb_mask);
	BUG_ON(rendering_pipe_num < req_rb_num);

	pipe_rb_ratio = rendering_pipe_num / req_rb_num;
	pipe_rb_remain = rendering_pipe_num - pipe_rb_ratio * req_rb_num;

	if (rdev->family <= CHIP_RV740) {
		/* r6xx/r7xx */
		rb_num_width = 2;
	} else {
		/* eg+ */
		rb_num_width = 4;
	}

	for (i = 0; i < max_rb_num; i++) {
		if (!(mask & disabled_rb_mask)) {
			for (j = 0; j < pipe_rb_ratio; j++) {
				data <<= rb_num_width;
				data |= max_rb_num - i - 1;
			}
			if (pipe_rb_remain) {
				data <<= rb_num_width;
				data |= max_rb_num - i - 1;
				pipe_rb_remain--;
			}
		}
		mask >>= 1;
	}

	return data;
}

static inline uint32_t hweight32(uint32_t x)
{
	x = (x & 0x55555555) + ((x & 0xaaaaaaaa) >> 1);
	x = (x & 0x33333333) + ((x & 0xcccccccc) >> 2);
	x = (x + (x >> 4)) & 0x0f0f0f0f;
	x = (x + (x >> 8));
	x = (x + (x >> 16)) & 0x000000ff;
	return x;
}

int r600_count_pipe_bits(uint32_t val)
{
	return hweight32(val);
}

static void r600_gpu_init(struct radeon_device *rdev)
{
	u32 tiling_config;
	u32 ramcfg;
	u32 cc_rb_backend_disable;
	u32 cc_gc_shader_pipe_config;
	u32 tmp;
	int i, j;
	u32 sq_config;
	u32 sq_gpr_resource_mgmt_1 = 0;
	u32 sq_gpr_resource_mgmt_2 = 0;
	u32 sq_thread_resource_mgmt = 0;
	u32 sq_stack_resource_mgmt_1 = 0;
	u32 sq_stack_resource_mgmt_2 = 0;
	u32 disabled_rb_mask;

	rdev->config.r600.tiling_group_size = 256;
	switch (rdev->family) {
	case CHIP_R600:
		rdev->config.r600.max_pipes = 4;
		rdev->config.r600.max_tile_pipes = 8;
		rdev->config.r600.max_simds = 4;
		rdev->config.r600.max_backends = 4;
		rdev->config.r600.max_gprs = 256;
		rdev->config.r600.max_threads = 192;
		rdev->config.r600.max_stack_entries = 256;
		rdev->config.r600.max_hw_contexts = 8;
		rdev->config.r600.max_gs_threads = 16;
		rdev->config.r600.sx_max_export_size = 128;
		rdev->config.r600.sx_max_export_pos_size = 16;
		rdev->config.r600.sx_max_export_smx_size = 128;
		rdev->config.r600.sq_num_cf_insts = 2;
		break;
	case CHIP_RV630:
	case CHIP_RV635:
		rdev->config.r600.max_pipes = 2;
		rdev->config.r600.max_tile_pipes = 2;
		rdev->config.r600.max_simds = 3;
		rdev->config.r600.max_backends = 1;
		rdev->config.r600.max_gprs = 128;
		rdev->config.r600.max_threads = 192;
		rdev->config.r600.max_stack_entries = 128;
		rdev->config.r600.max_hw_contexts = 8;
		rdev->config.r600.max_gs_threads = 4;
		rdev->config.r600.sx_max_export_size = 128;
		rdev->config.r600.sx_max_export_pos_size = 16;
		rdev->config.r600.sx_max_export_smx_size = 128;
		rdev->config.r600.sq_num_cf_insts = 2;
		break;
	case CHIP_RV610:
	case CHIP_RV620:
	case CHIP_RS780:
	case CHIP_RS880:
		rdev->config.r600.max_pipes = 1;
		rdev->config.r600.max_tile_pipes = 1;
		rdev->config.r600.max_simds = 2;
		rdev->config.r600.max_backends = 1;
		rdev->config.r600.max_gprs = 128;
		rdev->config.r600.max_threads = 192;
		rdev->config.r600.max_stack_entries = 128;
		rdev->config.r600.max_hw_contexts = 4;
		rdev->config.r600.max_gs_threads = 4;
		rdev->config.r600.sx_max_export_size = 128;
		rdev->config.r600.sx_max_export_pos_size = 16;
		rdev->config.r600.sx_max_export_smx_size = 128;
		rdev->config.r600.sq_num_cf_insts = 1;
		break;
	case CHIP_RV670:
		rdev->config.r600.max_pipes = 4;
		rdev->config.r600.max_tile_pipes = 4;
		rdev->config.r600.max_simds = 4;
		rdev->config.r600.max_backends = 4;
		rdev->config.r600.max_gprs = 192;
		rdev->config.r600.max_threads = 192;
		rdev->config.r600.max_stack_entries = 256;
		rdev->config.r600.max_hw_contexts = 8;
		rdev->config.r600.max_gs_threads = 16;
		rdev->config.r600.sx_max_export_size = 128;
		rdev->config.r600.sx_max_export_pos_size = 16;
		rdev->config.r600.sx_max_export_smx_size = 128;
		rdev->config.r600.sq_num_cf_insts = 2;
		break;
	default:
		break;
	}

	/* Initialize HDP */
	for (i = 0, j = 0; i < 32; i++, j += 0x18) {
		WREG32((0x2c14 + j), 0x00000000);
		WREG32((0x2c18 + j), 0x00000000);
		WREG32((0x2c1c + j), 0x00000000);
		WREG32((0x2c20 + j), 0x00000000);
		WREG32((0x2c24 + j), 0x00000000);
	}

	WREG32(GRBM_CNTL, GRBM_READ_TIMEOUT(0xff));

	/* Setup tiling */
	tiling_config = 0;
	ramcfg = RREG32(RAMCFG);
	switch (rdev->config.r600.max_tile_pipes) {
	case 1:
		tiling_config |= PIPE_TILING(0);
		break;
	case 2:
		tiling_config |= PIPE_TILING(1);
		break;
	case 4:
		tiling_config |= PIPE_TILING(2);
		break;
	case 8:
		tiling_config |= PIPE_TILING(3);
		break;
	default:
		break;
	}
	rdev->config.r600.tiling_npipes = rdev->config.r600.max_tile_pipes;
	rdev->config.r600.tiling_nbanks = 4 << ((ramcfg & NOOFBANK_MASK) >> NOOFBANK_SHIFT);
	tiling_config |= BANK_TILING((ramcfg & NOOFBANK_MASK) >> NOOFBANK_SHIFT);
	tiling_config |= GROUP_SIZE((ramcfg & BURSTLENGTH_MASK) >> BURSTLENGTH_SHIFT);

	tmp = (ramcfg & NOOFROWS_MASK) >> NOOFROWS_SHIFT;
	if (tmp > 3) {
		tiling_config |= ROW_TILING(3);
		tiling_config |= SAMPLE_SPLIT(3);
	} else {
		tiling_config |= ROW_TILING(tmp);
		tiling_config |= SAMPLE_SPLIT(tmp);
	}
	tiling_config |= BANK_SWAPS(1);

	cc_rb_backend_disable = RREG32(CC_RB_BACKEND_DISABLE) & 0x00ff0000;
	tmp = R6XX_MAX_BACKENDS -
		r600_count_pipe_bits((cc_rb_backend_disable >> 16) & R6XX_MAX_BACKENDS_MASK);
	if (tmp < rdev->config.r600.max_backends) {
		rdev->config.r600.max_backends = tmp;
	}

	cc_gc_shader_pipe_config = RREG32(CC_GC_SHADER_PIPE_CONFIG) & 0x00ffff00;
	tmp = R6XX_MAX_PIPES -
		r600_count_pipe_bits((cc_gc_shader_pipe_config >> 8) & R6XX_MAX_PIPES_MASK);
	if (tmp < rdev->config.r600.max_pipes) {
		rdev->config.r600.max_pipes = tmp;
	}
	tmp = R6XX_MAX_SIMDS -
		r600_count_pipe_bits((cc_gc_shader_pipe_config >> 16) & R6XX_MAX_SIMDS_MASK);
	if (tmp < rdev->config.r600.max_simds) {
		rdev->config.r600.max_simds = tmp;
	}

	disabled_rb_mask = (RREG32(CC_RB_BACKEND_DISABLE) >> 16) & R6XX_MAX_BACKENDS_MASK;
	tmp = (tiling_config & PIPE_TILING__MASK) >> PIPE_TILING__SHIFT;
	tmp = r6xx_remap_render_backend(rdev, tmp, rdev->config.r600.max_backends,
					R6XX_MAX_BACKENDS, disabled_rb_mask);
	tiling_config |= tmp << 16;
	rdev->config.r600.backend_map = tmp;

	rdev->config.r600.tile_config = tiling_config;
	WREG32(GB_TILING_CONFIG, tiling_config);
	WREG32(DCP_TILING_CONFIG, tiling_config & 0xffff);
	WREG32(HDP_TILING_CONFIG, tiling_config & 0xffff);
	WREG32(DMA_TILING_CONFIG, tiling_config & 0xffff);

	tmp = R6XX_MAX_PIPES - r600_count_pipe_bits((cc_gc_shader_pipe_config & INACTIVE_QD_PIPES_MASK) >> 8);
	WREG32(VGT_OUT_DEALLOC_CNTL, (tmp * 4) & DEALLOC_DIST_MASK);
	WREG32(VGT_VERTEX_REUSE_BLOCK_CNTL, ((tmp * 4) - 2) & VTX_REUSE_DEPTH_MASK);

	/* Setup some CP states */
	WREG32(CP_QUEUE_THRESHOLDS, (ROQ_IB1_START(0x16) | ROQ_IB2_START(0x2b)));
	WREG32(CP_MEQ_THRESHOLDS, (MEQ_END(0x40) | ROQ_END(0x40)));

	WREG32(TA_CNTL_AUX, (DISABLE_CUBE_ANISO | SYNC_GRADIENT |
			     SYNC_WALKER | SYNC_ALIGNER));
	/* Setup various GPU states */
	if (rdev->family == CHIP_RV670)
		WREG32(ARB_GDEC_RD_CNTL, 0x00000021);

	tmp = RREG32(SX_DEBUG_1);
	tmp |= SMX_EVENT_RELEASE;
	if ((rdev->family > CHIP_R600))
		tmp |= ENABLE_NEW_SMX_ADDRESS;
	WREG32(SX_DEBUG_1, tmp);

	if (((rdev->family) == CHIP_R600) ||
	    ((rdev->family) == CHIP_RV630) ||
	    ((rdev->family) == CHIP_RV610) ||
	    ((rdev->family) == CHIP_RV620) ||
	    ((rdev->family) == CHIP_RS780) ||
	    ((rdev->family) == CHIP_RS880)) {
		WREG32(DB_DEBUG, PREZ_MUST_WAIT_FOR_POSTZ_DONE);
	} else {
		WREG32(DB_DEBUG, 0);
	}
	WREG32(DB_WATERMARKS, (DEPTH_FREE(4) | DEPTH_CACHELINE_FREE(16) |
			       DEPTH_FLUSH(16) | DEPTH_PENDING_FREE(4)));

	WREG32(PA_SC_MULTI_CHIP_CNTL, 0);
	WREG32(VGT_NUM_INSTANCES, 0);

	WREG32(SPI_CONFIG_CNTL, GPR_WRITE_PRIORITY(0));
	WREG32(SPI_CONFIG_CNTL_1, VTX_DONE_DELAY(0));

	tmp = RREG32(SQ_MS_FIFO_SIZES);
	if (((rdev->family) == CHIP_RV610) ||
	    ((rdev->family) == CHIP_RV620) ||
	    ((rdev->family) == CHIP_RS780) ||
	    ((rdev->family) == CHIP_RS880)) {
		tmp = (CACHE_FIFO_SIZE(0xa) |
		       FETCH_FIFO_HIWATER(0xa) |
		       DONE_FIFO_HIWATER(0xe0) |
		       ALU_UPDATE_FIFO_HIWATER(0x8));
	} else if (((rdev->family) == CHIP_R600) ||
		   ((rdev->family) == CHIP_RV630)) {
		tmp &= ~DONE_FIFO_HIWATER(0xff);
		tmp |= DONE_FIFO_HIWATER(0x4);
	}
	WREG32(SQ_MS_FIFO_SIZES, tmp);

	/* SQ_CONFIG, SQ_GPR_RESOURCE_MGMT, SQ_THREAD_RESOURCE_MGMT, SQ_STACK_RESOURCE_MGMT
	 * should be adjusted as needed by the 2D/3D drivers.  This just sets default values
	 */
	sq_config = RREG32(SQ_CONFIG);
	sq_config &= ~(PS_PRIO(3) |
		       VS_PRIO(3) |
		       GS_PRIO(3) |
		       ES_PRIO(3));
	sq_config |= (DX9_CONSTS |
		      VC_ENABLE |
		      PS_PRIO(0) |
		      VS_PRIO(1) |
		      GS_PRIO(2) |
		      ES_PRIO(3));

	if ((rdev->family) == CHIP_R600) {
		sq_gpr_resource_mgmt_1 = (NUM_PS_GPRS(124) |
					  NUM_VS_GPRS(124) |
					  NUM_CLAUSE_TEMP_GPRS(4));
		sq_gpr_resource_mgmt_2 = (NUM_GS_GPRS(0) |
					  NUM_ES_GPRS(0));
		sq_thread_resource_mgmt = (NUM_PS_THREADS(136) |
					   NUM_VS_THREADS(48) |
					   NUM_GS_THREADS(4) |
					   NUM_ES_THREADS(4));
		sq_stack_resource_mgmt_1 = (NUM_PS_STACK_ENTRIES(128) |
					    NUM_VS_STACK_ENTRIES(128));
		sq_stack_resource_mgmt_2 = (NUM_GS_STACK_ENTRIES(0) |
					    NUM_ES_STACK_ENTRIES(0));
	} else if (((rdev->family) == CHIP_RV610) ||
		   ((rdev->family) == CHIP_RV620) ||
		   ((rdev->family) == CHIP_RS780) ||
		   ((rdev->family) == CHIP_RS880)) {
		/* no vertex cache */
		sq_config &= ~VC_ENABLE;

		sq_gpr_resource_mgmt_1 = (NUM_PS_GPRS(44) |
					  NUM_VS_GPRS(44) |
					  NUM_CLAUSE_TEMP_GPRS(2));
		sq_gpr_resource_mgmt_2 = (NUM_GS_GPRS(17) |
					  NUM_ES_GPRS(17));
		sq_thread_resource_mgmt = (NUM_PS_THREADS(79) |
					   NUM_VS_THREADS(78) |
					   NUM_GS_THREADS(4) |
					   NUM_ES_THREADS(31));
		sq_stack_resource_mgmt_1 = (NUM_PS_STACK_ENTRIES(40) |
					    NUM_VS_STACK_ENTRIES(40));
		sq_stack_resource_mgmt_2 = (NUM_GS_STACK_ENTRIES(32) |
					    NUM_ES_STACK_ENTRIES(16));
	} else if (((rdev->family) == CHIP_RV630) ||
		   ((rdev->family) == CHIP_RV635)) {
		sq_gpr_resource_mgmt_1 = (NUM_PS_GPRS(44) |
					  NUM_VS_GPRS(44) |
					  NUM_CLAUSE_TEMP_GPRS(2));
		sq_gpr_resource_mgmt_2 = (NUM_GS_GPRS(18) |
					  NUM_ES_GPRS(18));
		sq_thread_resource_mgmt = (NUM_PS_THREADS(79) |
					   NUM_VS_THREADS(78) |
					   NUM_GS_THREADS(4) |
					   NUM_ES_THREADS(31));
		sq_stack_resource_mgmt_1 = (NUM_PS_STACK_ENTRIES(40) |
					    NUM_VS_STACK_ENTRIES(40));
		sq_stack_resource_mgmt_2 = (NUM_GS_STACK_ENTRIES(32) |
					    NUM_ES_STACK_ENTRIES(16));
	} else if ((rdev->family) == CHIP_RV670) {
		sq_gpr_resource_mgmt_1 = (NUM_PS_GPRS(44) |
					  NUM_VS_GPRS(44) |
					  NUM_CLAUSE_TEMP_GPRS(2));
		sq_gpr_resource_mgmt_2 = (NUM_GS_GPRS(17) |
					  NUM_ES_GPRS(17));
		sq_thread_resource_mgmt = (NUM_PS_THREADS(79) |
					   NUM_VS_THREADS(78) |
					   NUM_GS_THREADS(4) |
					   NUM_ES_THREADS(31));
		sq_stack_resource_mgmt_1 = (NUM_PS_STACK_ENTRIES(64) |
					    NUM_VS_STACK_ENTRIES(64));
		sq_stack_resource_mgmt_2 = (NUM_GS_STACK_ENTRIES(64) |
					    NUM_ES_STACK_ENTRIES(64));
	}

	WREG32(SQ_CONFIG, sq_config);
	WREG32(SQ_GPR_RESOURCE_MGMT_1,  sq_gpr_resource_mgmt_1);
	WREG32(SQ_GPR_RESOURCE_MGMT_2,  sq_gpr_resource_mgmt_2);
	WREG32(SQ_THREAD_RESOURCE_MGMT, sq_thread_resource_mgmt);
	WREG32(SQ_STACK_RESOURCE_MGMT_1, sq_stack_resource_mgmt_1);
	WREG32(SQ_STACK_RESOURCE_MGMT_2, sq_stack_resource_mgmt_2);

	if (((rdev->family) == CHIP_RV610) ||
	    ((rdev->family) == CHIP_RV620) ||
	    ((rdev->family) == CHIP_RS780) ||
	    ((rdev->family) == CHIP_RS880)) {
		WREG32(VGT_CACHE_INVALIDATION, CACHE_INVALIDATION(TC_ONLY));
	} else {
		WREG32(VGT_CACHE_INVALIDATION, CACHE_INVALIDATION(VC_AND_TC));
	}

	/* More default values. 2D/3D driver should adjust as needed */
	WREG32(PA_SC_AA_SAMPLE_LOCS_2S, (S0_X(0xc) | S0_Y(0x4) |
					 S1_X(0x4) | S1_Y(0xc)));
	WREG32(PA_SC_AA_SAMPLE_LOCS_4S, (S0_X(0xe) | S0_Y(0xe) |
					 S1_X(0x2) | S1_Y(0x2) |
					 S2_X(0xa) | S2_Y(0x6) |
					 S3_X(0x6) | S3_Y(0xa)));
	WREG32(PA_SC_AA_SAMPLE_LOCS_8S_WD0, (S0_X(0xe) | S0_Y(0xb) |
					     S1_X(0x4) | S1_Y(0xc) |
					     S2_X(0x1) | S2_Y(0x6) |
					     S3_X(0xa) | S3_Y(0xe)));
	WREG32(PA_SC_AA_SAMPLE_LOCS_8S_WD1, (S4_X(0x6) | S4_Y(0x1) |
					     S5_X(0x0) | S5_Y(0x0) |
					     S6_X(0xb) | S6_Y(0x4) |
					     S7_X(0x7) | S7_Y(0x8)));

	WREG32(VGT_STRMOUT_EN, 0);
	tmp = rdev->config.r600.max_pipes * 16;
	switch (rdev->family) {
	case CHIP_RV610:
	case CHIP_RV620:
	case CHIP_RS780:
	case CHIP_RS880:
		tmp += 32;
		break;
	case CHIP_RV670:
		tmp += 128;
		break;
	default:
		break;
	}
	if (tmp > 256) {
		tmp = 256;
	}
	WREG32(VGT_ES_PER_GS, 128);
	WREG32(VGT_GS_PER_ES, tmp);
	WREG32(VGT_GS_PER_VS, 2);
	WREG32(VGT_GS_VERTEX_REUSE, 16);

	/* more default values. 2D/3D driver should adjust as needed */
	WREG32(PA_SC_LINE_STIPPLE_STATE, 0);
	WREG32(VGT_STRMOUT_EN, 0);
	WREG32(SX_MISC, 0);
	WREG32(PA_SC_MODE_CNTL, 0);
	WREG32(PA_SC_AA_CONFIG, 0);
	WREG32(PA_SC_LINE_STIPPLE, 0);
	WREG32(SPI_INPUT_Z, 0);
	WREG32(SPI_PS_IN_CONTROL_0, NUM_INTERP(2));
	WREG32(CB_COLOR7_FRAG, 0);

	/* Clear render buffer base addresses */
	WREG32(CB_COLOR0_BASE, 0);
	WREG32(CB_COLOR1_BASE, 0);
	WREG32(CB_COLOR2_BASE, 0);
	WREG32(CB_COLOR3_BASE, 0);
	WREG32(CB_COLOR4_BASE, 0);
	WREG32(CB_COLOR5_BASE, 0);
	WREG32(CB_COLOR6_BASE, 0);
	WREG32(CB_COLOR7_BASE, 0);
	WREG32(CB_COLOR7_FRAG, 0);

	switch (rdev->family) {
	case CHIP_RV610:
	case CHIP_RV620:
	case CHIP_RS780:
	case CHIP_RS880:
		tmp = TC_L2_SIZE(8);
		break;
	case CHIP_RV630:
	case CHIP_RV635:
		tmp = TC_L2_SIZE(4);
		break;
	case CHIP_R600:
		tmp = TC_L2_SIZE(0) | L2_DISABLE_LATE_HIT;
		break;
	default:
		tmp = TC_L2_SIZE(0);
		break;
	}
	WREG32(TC_CNTL, tmp);

	tmp = RREG32(HDP_HOST_PATH_CNTL);
	WREG32(HDP_HOST_PATH_CNTL, tmp);

	tmp = RREG32(ARB_POP);
	tmp |= ENABLE_TC128;
	WREG32(ARB_POP, tmp);

	WREG32(PA_SC_MULTI_CHIP_CNTL, 0);
	WREG32(PA_CL_ENHANCE, (CLIP_VTX_REORDER_ENA |
			       NUM_CLIP_SEQ(3)));
	WREG32(PA_SC_ENHANCE, FORCE_EOV_MAX_CLK_CNT(4095));
	WREG32(VC_ENHANCE, 0);
}


/*
 * Indirect registers accessor
 */
u32 r600_pciep_rreg(struct radeon_device *rdev, u32 reg)
{
	u32 r;

	WREG32(PCIE_PORT_INDEX, ((reg) & 0xff));
	(void)RREG32(PCIE_PORT_INDEX);
	r = RREG32(PCIE_PORT_DATA);
	return r;
}

void r600_pciep_wreg(struct radeon_device *rdev, u32 reg, u32 v)
{
	WREG32(PCIE_PORT_INDEX, ((reg) & 0xff));
	(void)RREG32(PCIE_PORT_INDEX);
	WREG32(PCIE_PORT_DATA, (v));
	(void)RREG32(PCIE_PORT_DATA);
}

/*
 * CP & Ring
 */
void r600_cp_stop(struct radeon_device *rdev)
{
	radeon_ttm_set_active_vram_size(rdev, rdev->mc.visible_vram_size);
	WREG32(R_0086D8_CP_ME_CNTL, S_0086D8_CP_ME_HALT(1));
	WREG32(SCRATCH_UMSK, 0);
	rdev->ring[RADEON_RING_TYPE_GFX_INDEX].ready = false;
}

int r600_init_microcode(struct radeon_device *rdev)
{
	const char *chip_name;
	const char *rlc_chip_name;
	size_t pfp_req_size, me_req_size, rlc_req_size;
	char fw_name[30];
	int err;

	DRM_DEBUG("\n");

#if 0
	pdev = platform_device_register_simple("radeon_cp", 0, NULL, 0);
	err = IS_ERR(pdev);
	if (err) {
		printk(KERN_ERR "radeon_cp: Failed to register firmware\n");
		return -EINVAL;
	}
#endif

	switch (rdev->family) {
	case CHIP_R600:
		chip_name = "r600";
		rlc_chip_name = "r600";
		break;
	case CHIP_RV610:
		chip_name = "rv610";
		rlc_chip_name = "r600";
		break;
	case CHIP_RV630:
		chip_name = "rv630";
		rlc_chip_name = "r600";
		break;
	case CHIP_RV620:
		chip_name = "rv620";
		rlc_chip_name = "r600";
		break;
	case CHIP_RV635:
		chip_name = "rv635";
		rlc_chip_name = "r600";
		break;
	case CHIP_RV670:
		chip_name = "rv670";
		rlc_chip_name = "r600";
		break;
	case CHIP_RS780:
	case CHIP_RS880:
		chip_name = "rs780";
		rlc_chip_name = "r600";
		break;
	case CHIP_RV770:
		chip_name = "rv770";
		rlc_chip_name = "r700";
		break;
	case CHIP_RV730:
	case CHIP_RV740:
		chip_name = "rv730";
		rlc_chip_name = "r700";
		break;
	case CHIP_RV710:
		chip_name = "rv710";
		rlc_chip_name = "r700";
		break;
	case CHIP_CEDAR:
		chip_name = "cedar";
		rlc_chip_name = "cedar";
		break;
	case CHIP_REDWOOD:
		chip_name = "redwood";
		rlc_chip_name = "redwood";
		break;
	case CHIP_JUNIPER:
		chip_name = "juniper";
		rlc_chip_name = "juniper";
		break;
	case CHIP_CYPRESS:
	case CHIP_HEMLOCK:
		chip_name = "cypress";
		rlc_chip_name = "cypress";
		break;
	case CHIP_PALM:
		chip_name = "palm";
		rlc_chip_name = "sumo";
		break;
	case CHIP_SUMO:
		chip_name = "sumo";
		rlc_chip_name = "sumo";
		break;
	case CHIP_SUMO2:
		chip_name = "sumo2";
		rlc_chip_name = "sumo";
		break;
	default: BUG();
	}

	if (rdev->family >= CHIP_CEDAR) {
		pfp_req_size = EVERGREEN_PFP_UCODE_SIZE * 4;
		me_req_size = EVERGREEN_PM4_UCODE_SIZE * 4;
		rlc_req_size = EVERGREEN_RLC_UCODE_SIZE * 4;
	} else if (rdev->family >= CHIP_RV770) {
		pfp_req_size = R700_PFP_UCODE_SIZE * 4;
		me_req_size = R700_PM4_UCODE_SIZE * 4;
		rlc_req_size = R700_RLC_UCODE_SIZE * 4;
	} else {
		pfp_req_size = PFP_UCODE_SIZE * 4;
		me_req_size = PM4_UCODE_SIZE * 12;
		rlc_req_size = RLC_UCODE_SIZE * 4;
	}

	DRM_INFO("Loading %s Microcode\n", chip_name);

	snprintf(fw_name, sizeof(fw_name), "radeon-%s_pfp", chip_name);
	err = loadfirmware(fw_name, &rdev->pfp_fw, &rdev->pfp_fw_size);
	if (err)
		goto out;
	if (rdev->pfp_fw_size != pfp_req_size) {
		DRM_ERROR(
		       "r600_cp: Bogus length %zu in firmware \"%s\"\n",
		       rdev->pfp_fw_size, fw_name);
		err = -EINVAL;
		goto out;
	}

	snprintf(fw_name, sizeof(fw_name), "radeon-%s_me", chip_name);
	err = loadfirmware(fw_name, &rdev->me_fw, &rdev->me_fw_size);
	if (err)
		goto out;
	if (rdev->me_fw_size != me_req_size) {
		DRM_ERROR(
		       "r600_cp: Bogus length %zu in firmware \"%s\"\n",
		       rdev->me_fw_size, fw_name);
		err = -EINVAL;
	}

	snprintf(fw_name, sizeof(fw_name), "radeon-%s_rlc", rlc_chip_name);
	err = loadfirmware(fw_name, &rdev->rlc_fw, &rdev->rlc_fw_size);
	if (err)
		goto out;
	if (rdev->rlc_fw_size != rlc_req_size) {
		DRM_ERROR(
		       "r600_rlc: Bogus length %zu in firmware \"%s\"\n",
		       rdev->rlc_fw_size, fw_name);
		err = -EINVAL;
	}

out:
	if (err) {
		if (err != -EINVAL)
			printk(KERN_ERR
			       "r600_cp: Failed to load firmware \"%s\"\n",
			       fw_name);
		if (rdev->pfp_fw) {
			free(rdev->pfp_fw, M_DEVBUF, 0);
			rdev->pfp_fw = NULL;
		}
		if (rdev->me_fw) {
			free(rdev->me_fw, M_DEVBUF, 0);
			rdev->me_fw = NULL;
		}
		if (rdev->rlc_fw) {
			free(rdev->rlc_fw, M_DEVBUF, 0);
			rdev->rlc_fw = NULL;
		}
	}
	return err;
}

static int r600_cp_load_microcode(struct radeon_device *rdev)
{
	const __be32 *fw_data;
	int i;

	if (!rdev->me_fw || !rdev->pfp_fw)
		return -EINVAL;

	r600_cp_stop(rdev);

	WREG32(CP_RB_CNTL,
#ifdef __BIG_ENDIAN
	       BUF_SWAP_32BIT |
#endif
	       RB_NO_UPDATE | RB_BLKSZ(15) | RB_BUFSZ(3));

	/* Reset cp */
	WREG32(GRBM_SOFT_RESET, SOFT_RESET_CP);
	RREG32(GRBM_SOFT_RESET);
	mdelay(15);
	WREG32(GRBM_SOFT_RESET, 0);

	WREG32(CP_ME_RAM_WADDR, 0);

	fw_data = (const __be32 *)rdev->me_fw;
	WREG32(CP_ME_RAM_WADDR, 0);
	for (i = 0; i < PM4_UCODE_SIZE * 3; i++)
		WREG32(CP_ME_RAM_DATA,
		       be32_to_cpup(fw_data++));

	fw_data = (const __be32 *)rdev->pfp_fw;
	WREG32(CP_PFP_UCODE_ADDR, 0);
	for (i = 0; i < PFP_UCODE_SIZE; i++)
		WREG32(CP_PFP_UCODE_DATA,
		       be32_to_cpup(fw_data++));

	WREG32(CP_PFP_UCODE_ADDR, 0);
	WREG32(CP_ME_RAM_WADDR, 0);
	WREG32(CP_ME_RAM_RADDR, 0);
	return 0;
}

int r600_cp_start(struct radeon_device *rdev)
{
	struct radeon_ring *ring = &rdev->ring[RADEON_RING_TYPE_GFX_INDEX];
	int r;
	uint32_t cp_me;

	r = radeon_ring_lock(rdev, ring, 7);
	if (r) {
		DRM_ERROR("radeon: cp failed to lock ring (%d).\n", r);
		return r;
	}
	radeon_ring_write(ring, PACKET3(PACKET3_ME_INITIALIZE, 5));
	radeon_ring_write(ring, 0x1);
	if (rdev->family >= CHIP_RV770) {
		radeon_ring_write(ring, 0x0);
		radeon_ring_write(ring, rdev->config.rv770.max_hw_contexts - 1);
	} else {
		radeon_ring_write(ring, 0x3);
		radeon_ring_write(ring, rdev->config.r600.max_hw_contexts - 1);
	}
	radeon_ring_write(ring, PACKET3_ME_INITIALIZE_DEVICE_ID(1));
	radeon_ring_write(ring, 0);
	radeon_ring_write(ring, 0);
	radeon_ring_unlock_commit(rdev, ring);

	cp_me = 0xff;
	WREG32(R_0086D8_CP_ME_CNTL, cp_me);
	return 0;
}

int r600_cp_resume(struct radeon_device *rdev)
{
	struct radeon_ring *ring = &rdev->ring[RADEON_RING_TYPE_GFX_INDEX];
	u32 tmp;
	u32 rb_bufsz;
	int r;

	/* Reset cp */
	WREG32(GRBM_SOFT_RESET, SOFT_RESET_CP);
	RREG32(GRBM_SOFT_RESET);
	mdelay(15);
	WREG32(GRBM_SOFT_RESET, 0);

	/* Set ring buffer size */
	rb_bufsz = drm_order(ring->ring_size / 8);
	tmp = (drm_order(RADEON_GPU_PAGE_SIZE/8) << 8) | rb_bufsz;
#ifdef __BIG_ENDIAN
	tmp |= BUF_SWAP_32BIT;
#endif
	WREG32(CP_RB_CNTL, tmp);
	WREG32(CP_SEM_WAIT_TIMER, 0x0);

	/* Set the write pointer delay */
	WREG32(CP_RB_WPTR_DELAY, 0);

	/* Initialize the ring buffer's read and write pointers */
	WREG32(CP_RB_CNTL, tmp | RB_RPTR_WR_ENA);
	WREG32(CP_RB_RPTR_WR, 0);
	ring->wptr = 0;
	WREG32(CP_RB_WPTR, ring->wptr);

	/* set the wb address whether it's enabled or not */
	WREG32(CP_RB_RPTR_ADDR,
	       ((rdev->wb.gpu_addr + RADEON_WB_CP_RPTR_OFFSET) & 0xFFFFFFFC));
	WREG32(CP_RB_RPTR_ADDR_HI, upper_32_bits(rdev->wb.gpu_addr + RADEON_WB_CP_RPTR_OFFSET) & 0xFF);
	WREG32(SCRATCH_ADDR, ((rdev->wb.gpu_addr + RADEON_WB_SCRATCH_OFFSET) >> 8) & 0xFFFFFFFF);

	if (rdev->wb.enabled)
		WREG32(SCRATCH_UMSK, 0xff);
	else {
		tmp |= RB_NO_UPDATE;
		WREG32(SCRATCH_UMSK, 0);
	}

	mdelay(1);
	WREG32(CP_RB_CNTL, tmp);

	WREG32(CP_RB_BASE, ring->gpu_addr >> 8);
	WREG32(CP_DEBUG, (1 << 27) | (1 << 28));

	ring->rptr = RREG32(CP_RB_RPTR);

	r600_cp_start(rdev);
	ring->ready = true;
	r = radeon_ring_test(rdev, RADEON_RING_TYPE_GFX_INDEX, ring);
	if (r) {
		ring->ready = false;
		return r;
	}
	return 0;
}

void r600_ring_init(struct radeon_device *rdev, struct radeon_ring *ring, unsigned ring_size)
{
	u32 rb_bufsz;
	int r;

	/* Align ring size */
	rb_bufsz = drm_order(ring_size / 8);
	ring_size = (1 << (rb_bufsz + 1)) * 4;
	ring->ring_size = ring_size;
	ring->align_mask = 16 - 1;

	if (radeon_ring_supports_scratch_reg(rdev, ring)) {
		r = radeon_scratch_get(rdev, &ring->rptr_save_reg);
		if (r) {
			DRM_ERROR("failed to get scratch reg for rptr save (%d).\n", r);
			ring->rptr_save_reg = 0;
		}
	}
}

void r600_cp_fini(struct radeon_device *rdev)
{
	struct radeon_ring *ring = &rdev->ring[RADEON_RING_TYPE_GFX_INDEX];
	r600_cp_stop(rdev);
	radeon_ring_fini(rdev, ring);
	radeon_scratch_free(rdev, ring->rptr_save_reg);
}

/*
 * DMA
 * Starting with R600, the GPU has an asynchronous
 * DMA engine.  The programming model is very similar
 * to the 3D engine (ring buffer, IBs, etc.), but the
 * DMA controller has it's own packet format that is
 * different form the PM4 format used by the 3D engine.
 * It supports copying data, writing embedded data,
 * solid fills, and a number of other things.  It also
 * has support for tiling/detiling of buffers.
 */
/**
 * r600_dma_stop - stop the async dma engine
 *
 * @rdev: radeon_device pointer
 *
 * Stop the async dma engine (r6xx-evergreen).
 */
void r600_dma_stop(struct radeon_device *rdev)
{
	u32 rb_cntl = RREG32(DMA_RB_CNTL);

	radeon_ttm_set_active_vram_size(rdev, rdev->mc.visible_vram_size);

	rb_cntl &= ~DMA_RB_ENABLE;
	WREG32(DMA_RB_CNTL, rb_cntl);

	rdev->ring[R600_RING_TYPE_DMA_INDEX].ready = false;
}

/**
 * r600_dma_resume - setup and start the async dma engine
 *
 * @rdev: radeon_device pointer
 *
 * Set up the DMA ring buffer and enable it. (r6xx-evergreen).
 * Returns 0 for success, error for failure.
 */
int r600_dma_resume(struct radeon_device *rdev)
{
	struct radeon_ring *ring = &rdev->ring[R600_RING_TYPE_DMA_INDEX];
	u32 rb_cntl, dma_cntl, ib_cntl;
	u32 rb_bufsz;
	int r;

	/* Reset dma */
	if (rdev->family >= CHIP_RV770)
		WREG32(SRBM_SOFT_RESET, RV770_SOFT_RESET_DMA);
	else
		WREG32(SRBM_SOFT_RESET, SOFT_RESET_DMA);
	RREG32(SRBM_SOFT_RESET);
	udelay(50);
	WREG32(SRBM_SOFT_RESET, 0);

	WREG32(DMA_SEM_INCOMPLETE_TIMER_CNTL, 0);
	WREG32(DMA_SEM_WAIT_FAIL_TIMER_CNTL, 0);

	/* Set ring buffer size in dwords */
	rb_bufsz = drm_order(ring->ring_size / 4);
	rb_cntl = rb_bufsz << 1;
#ifdef __BIG_ENDIAN
	rb_cntl |= DMA_RB_SWAP_ENABLE | DMA_RPTR_WRITEBACK_SWAP_ENABLE;
#endif
	WREG32(DMA_RB_CNTL, rb_cntl);

	/* Initialize the ring buffer's read and write pointers */
	WREG32(DMA_RB_RPTR, 0);
	WREG32(DMA_RB_WPTR, 0);

	/* set the wb address whether it's enabled or not */
	WREG32(DMA_RB_RPTR_ADDR_HI,
	       upper_32_bits(rdev->wb.gpu_addr + R600_WB_DMA_RPTR_OFFSET) & 0xFF);
	WREG32(DMA_RB_RPTR_ADDR_LO,
	       ((rdev->wb.gpu_addr + R600_WB_DMA_RPTR_OFFSET) & 0xFFFFFFFC));

	if (rdev->wb.enabled)
		rb_cntl |= DMA_RPTR_WRITEBACK_ENABLE;

	WREG32(DMA_RB_BASE, ring->gpu_addr >> 8);

	/* enable DMA IBs */
	ib_cntl = DMA_IB_ENABLE;
#ifdef __BIG_ENDIAN
	ib_cntl |= DMA_IB_SWAP_ENABLE;
#endif
	WREG32(DMA_IB_CNTL, ib_cntl);

	dma_cntl = RREG32(DMA_CNTL);
	dma_cntl &= ~CTXEMPTY_INT_ENABLE;
	WREG32(DMA_CNTL, dma_cntl);

	if (rdev->family >= CHIP_RV770)
		WREG32(DMA_MODE, 1);

	ring->wptr = 0;
	WREG32(DMA_RB_WPTR, ring->wptr << 2);

	ring->rptr = RREG32(DMA_RB_RPTR) >> 2;

	WREG32(DMA_RB_CNTL, rb_cntl | DMA_RB_ENABLE);

	ring->ready = true;

	r = radeon_ring_test(rdev, R600_RING_TYPE_DMA_INDEX, ring);
	if (r) {
		ring->ready = false;
		return r;
	}

	radeon_ttm_set_active_vram_size(rdev, rdev->mc.real_vram_size);

	return 0;
}

/**
 * r600_dma_fini - tear down the async dma engine
 *
 * @rdev: radeon_device pointer
 *
 * Stop the async dma engine and free the ring (r6xx-evergreen).
 */
void r600_dma_fini(struct radeon_device *rdev)
{
	r600_dma_stop(rdev);
	radeon_ring_fini(rdev, &rdev->ring[R600_RING_TYPE_DMA_INDEX]);
}

/*
 * GPU scratch registers helpers function.
 */
void r600_scratch_init(struct radeon_device *rdev)
{
	int i;

	rdev->scratch.num_reg = 7;
	rdev->scratch.reg_base = SCRATCH_REG0;
	for (i = 0; i < rdev->scratch.num_reg; i++) {
		rdev->scratch.free[i] = true;
		rdev->scratch.reg[i] = rdev->scratch.reg_base + (i * 4);
	}
}

int r600_ring_test(struct radeon_device *rdev, struct radeon_ring *ring)
{
	uint32_t scratch;
	uint32_t tmp = 0;
	unsigned i;
	int r;

	r = radeon_scratch_get(rdev, &scratch);
	if (r) {
		DRM_ERROR("radeon: cp failed to get scratch reg (%d).\n", r);
		return r;
	}
	WREG32(scratch, 0xCAFEDEAD);
	r = radeon_ring_lock(rdev, ring, 3);
	if (r) {
		DRM_ERROR("radeon: cp failed to lock ring %d (%d).\n", ring->idx, r);
		radeon_scratch_free(rdev, scratch);
		return r;
	}
	radeon_ring_write(ring, PACKET3(PACKET3_SET_CONFIG_REG, 1));
	radeon_ring_write(ring, ((scratch - PACKET3_SET_CONFIG_REG_OFFSET) >> 2));
	radeon_ring_write(ring, 0xDEADBEEF);
	radeon_ring_unlock_commit(rdev, ring);
	for (i = 0; i < rdev->usec_timeout; i++) {
		tmp = RREG32(scratch);
		if (tmp == 0xDEADBEEF)
			break;
		udelay(1);
	}
	if (i < rdev->usec_timeout) {
		DRM_INFO("ring test on %d succeeded in %d usecs\n", ring->idx, i);
	} else {
		DRM_ERROR("radeon: ring %d test failed (scratch(0x%04X)=0x%08X)\n",
			  ring->idx, scratch, tmp);
		r = -EINVAL;
	}
	radeon_scratch_free(rdev, scratch);
	return r;
}

/**
 * r600_dma_ring_test - simple async dma engine test
 *
 * @rdev: radeon_device pointer
 * @ring: radeon_ring structure holding ring information
 *
 * Test the DMA engine by writing using it to write an
 * value to memory. (r6xx-SI).
 * Returns 0 for success, error for failure.
 */
int r600_dma_ring_test(struct radeon_device *rdev,
		       struct radeon_ring *ring)
{
	unsigned i;
	int r;
	volatile uint32_t *ptr = rdev->vram_scratch.ptr;
	u32 tmp;

	if (!ptr) {
		DRM_ERROR("invalid vram scratch pointer\n");
		return -EINVAL;
	}

	tmp = 0xCAFEDEAD;
	*ptr = tmp;

	r = radeon_ring_lock(rdev, ring, 4);
	if (r) {
		DRM_ERROR("radeon: dma failed to lock ring %d (%d).\n", ring->idx, r);
		return r;
	}
	radeon_ring_write(ring, DMA_PACKET(DMA_PACKET_WRITE, 0, 0, 1));
	radeon_ring_write(ring, rdev->vram_scratch.gpu_addr & 0xfffffffc);
	radeon_ring_write(ring, upper_32_bits(rdev->vram_scratch.gpu_addr) & 0xff);
	radeon_ring_write(ring, 0xDEADBEEF);
	radeon_ring_unlock_commit(rdev, ring);

	for (i = 0; i < rdev->usec_timeout; i++) {
		tmp = *ptr;
		if (tmp == 0xDEADBEEF)
			break;
		udelay(1);
	}

	if (i < rdev->usec_timeout) {
		DRM_INFO("ring test on %d succeeded in %d usecs\n", ring->idx, i);
	} else {
		DRM_ERROR("radeon: ring %d test failed (0x%08X)\n",
			  ring->idx, tmp);
		r = -EINVAL;
	}
	return r;
}

/*
 * CP fences/semaphores
 */

void r600_fence_ring_emit(struct radeon_device *rdev,
			  struct radeon_fence *fence)
{
	struct radeon_ring *ring = &rdev->ring[fence->ring];
	u32 cp_coher_cntl = PACKET3_TC_ACTION_ENA | PACKET3_VC_ACTION_ENA |
		PACKET3_SH_ACTION_ENA;

	if (rdev->family >= CHIP_RV770)
		cp_coher_cntl |= PACKET3_FULL_CACHE_ENA;

	if (rdev->wb.use_event) {
		u64 addr = rdev->fence_drv[fence->ring].gpu_addr;
		/* flush read cache over gart */
		radeon_ring_write(ring, PACKET3(PACKET3_SURFACE_SYNC, 3));
		radeon_ring_write(ring, cp_coher_cntl);
		radeon_ring_write(ring, 0xFFFFFFFF);
		radeon_ring_write(ring, 0);
		radeon_ring_write(ring, 10); /* poll interval */
		/* EVENT_WRITE_EOP - flush caches, send int */
		radeon_ring_write(ring, PACKET3(PACKET3_EVENT_WRITE_EOP, 4));
		radeon_ring_write(ring, EVENT_TYPE(CACHE_FLUSH_AND_INV_EVENT_TS) | EVENT_INDEX(5));
		radeon_ring_write(ring, addr & 0xffffffff);
		radeon_ring_write(ring, (upper_32_bits(addr) & 0xff) | DATA_SEL(1) | INT_SEL(2));
		radeon_ring_write(ring, fence->seq);
		radeon_ring_write(ring, 0);
	} else {
		/* flush read cache over gart */
		radeon_ring_write(ring, PACKET3(PACKET3_SURFACE_SYNC, 3));
		radeon_ring_write(ring, cp_coher_cntl);
		radeon_ring_write(ring, 0xFFFFFFFF);
		radeon_ring_write(ring, 0);
		radeon_ring_write(ring, 10); /* poll interval */
		radeon_ring_write(ring, PACKET3(PACKET3_EVENT_WRITE, 0));
		radeon_ring_write(ring, EVENT_TYPE(CACHE_FLUSH_AND_INV_EVENT) | EVENT_INDEX(0));
		/* wait for 3D idle clean */
		radeon_ring_write(ring, PACKET3(PACKET3_SET_CONFIG_REG, 1));
		radeon_ring_write(ring, (WAIT_UNTIL - PACKET3_SET_CONFIG_REG_OFFSET) >> 2);
		radeon_ring_write(ring, WAIT_3D_IDLE_bit | WAIT_3D_IDLECLEAN_bit);
		/* Emit fence sequence & fire IRQ */
		radeon_ring_write(ring, PACKET3(PACKET3_SET_CONFIG_REG, 1));
		radeon_ring_write(ring, ((rdev->fence_drv[fence->ring].scratch_reg - PACKET3_SET_CONFIG_REG_OFFSET) >> 2));
		radeon_ring_write(ring, fence->seq);
		/* CP_INTERRUPT packet 3 no longer exists, use packet 0 */
		radeon_ring_write(ring, PACKET0(CP_INT_STATUS, 0));
		radeon_ring_write(ring, RB_INT_STAT);
	}
}

void r600_semaphore_ring_emit(struct radeon_device *rdev,
			      struct radeon_ring *ring,
			      struct radeon_semaphore *semaphore,
			      bool emit_wait)
{
	uint64_t addr = semaphore->gpu_addr;
	unsigned sel = emit_wait ? PACKET3_SEM_SEL_WAIT : PACKET3_SEM_SEL_SIGNAL;

	if (rdev->family < CHIP_CAYMAN)
		sel |= PACKET3_SEM_WAIT_ON_SIGNAL;

	radeon_ring_write(ring, PACKET3(PACKET3_MEM_SEMAPHORE, 1));
	radeon_ring_write(ring, addr & 0xffffffff);
	radeon_ring_write(ring, (upper_32_bits(addr) & 0xff) | sel);
}

/*
 * DMA fences/semaphores
 */

/**
 * r600_dma_fence_ring_emit - emit a fence on the DMA ring
 *
 * @rdev: radeon_device pointer
 * @fence: radeon fence object
 *
 * Add a DMA fence packet to the ring to write
 * the fence seq number and DMA trap packet to generate
 * an interrupt if needed (r6xx-r7xx).
 */
void r600_dma_fence_ring_emit(struct radeon_device *rdev,
			      struct radeon_fence *fence)
{
	struct radeon_ring *ring = &rdev->ring[fence->ring];
	u64 addr = rdev->fence_drv[fence->ring].gpu_addr;

	/* write the fence */
	radeon_ring_write(ring, DMA_PACKET(DMA_PACKET_FENCE, 0, 0, 0));
	radeon_ring_write(ring, addr & 0xfffffffc);
	radeon_ring_write(ring, (upper_32_bits(addr) & 0xff));
	radeon_ring_write(ring, lower_32_bits(fence->seq));
	/* generate an interrupt */
	radeon_ring_write(ring, DMA_PACKET(DMA_PACKET_TRAP, 0, 0, 0));
}

/**
 * r600_dma_semaphore_ring_emit - emit a semaphore on the dma ring
 *
 * @rdev: radeon_device pointer
 * @ring: radeon_ring structure holding ring information
 * @semaphore: radeon semaphore object
 * @emit_wait: wait or signal semaphore
 *
 * Add a DMA semaphore packet to the ring wait on or signal
 * other rings (r6xx-SI).
 */
void r600_dma_semaphore_ring_emit(struct radeon_device *rdev,
				  struct radeon_ring *ring,
				  struct radeon_semaphore *semaphore,
				  bool emit_wait)
{
	u64 addr = semaphore->gpu_addr;
	u32 s = emit_wait ? 0 : 1;

	radeon_ring_write(ring, DMA_PACKET(DMA_PACKET_SEMAPHORE, 0, s, 0));
	radeon_ring_write(ring, addr & 0xfffffffc);
	radeon_ring_write(ring, upper_32_bits(addr) & 0xff);
}

int r600_copy_blit(struct radeon_device *rdev,
		   uint64_t src_offset,
		   uint64_t dst_offset,
		   unsigned num_gpu_pages,
		   struct radeon_fence **fence)
{
	struct radeon_semaphore *sem = NULL;
	struct radeon_sa_bo *vb = NULL;
	int r;

	r = r600_blit_prepare_copy(rdev, num_gpu_pages, fence, &vb, &sem);
	if (r) {
		return r;
	}
	r600_kms_blit_copy(rdev, src_offset, dst_offset, num_gpu_pages, vb);
	r600_blit_done_copy(rdev, fence, vb, sem);
	return 0;
}

/**
 * r600_copy_dma - copy pages using the DMA engine
 *
 * @rdev: radeon_device pointer
 * @src_offset: src GPU address
 * @dst_offset: dst GPU address
 * @num_gpu_pages: number of GPU pages to xfer
 * @fence: radeon fence object
 *
 * Copy GPU paging using the DMA engine (r6xx).
 * Used by the radeon ttm implementation to move pages if
 * registered as the asic copy callback.
 */
int r600_copy_dma(struct radeon_device *rdev,
		  uint64_t src_offset, uint64_t dst_offset,
		  unsigned num_gpu_pages,
		  struct radeon_fence **fence)
{
	struct radeon_semaphore *sem = NULL;
	int ring_index = rdev->asic->copy.dma_ring_index;
	struct radeon_ring *ring = &rdev->ring[ring_index];
	u32 size_in_dw, cur_size_in_dw;
	int i, num_loops;
	int r = 0;

	r = radeon_semaphore_create(rdev, &sem);
	if (r) {
		DRM_ERROR("radeon: moving bo (%d).\n", r);
		return r;
	}

	size_in_dw = (num_gpu_pages << RADEON_GPU_PAGE_SHIFT) / 4;
	num_loops = howmany(size_in_dw, 0xFFFE);
	r = radeon_ring_lock(rdev, ring, num_loops * 4 + 8);
	if (r) {
		DRM_ERROR("radeon: moving bo (%d).\n", r);
		radeon_semaphore_free(rdev, &sem, NULL);
		return r;
	}

	if (radeon_fence_need_sync(*fence, ring->idx)) {
		radeon_semaphore_sync_rings(rdev, sem, (*fence)->ring,
					    ring->idx);
		radeon_fence_note_sync(*fence, ring->idx);
	} else {
		radeon_semaphore_free(rdev, &sem, NULL);
	}

	for (i = 0; i < num_loops; i++) {
		cur_size_in_dw = size_in_dw;
		if (cur_size_in_dw > 0xFFFE)
			cur_size_in_dw = 0xFFFE;
		size_in_dw -= cur_size_in_dw;
		radeon_ring_write(ring, DMA_PACKET(DMA_PACKET_COPY, 0, 0, cur_size_in_dw));
		radeon_ring_write(ring, dst_offset & 0xfffffffc);
		radeon_ring_write(ring, src_offset & 0xfffffffc);
		radeon_ring_write(ring, (((upper_32_bits(dst_offset) & 0xff) << 16) |
					 (upper_32_bits(src_offset) & 0xff)));
		src_offset += cur_size_in_dw * 4;
		dst_offset += cur_size_in_dw * 4;
	}

	r = radeon_fence_emit(rdev, fence, ring->idx);
	if (r) {
		radeon_ring_unlock_undo(rdev, ring);
		return r;
	}

	radeon_ring_unlock_commit(rdev, ring);
	radeon_semaphore_free(rdev, &sem, *fence);

	return r;
}

int r600_set_surface_reg(struct radeon_device *rdev, int reg,
			 uint32_t tiling_flags, uint32_t pitch,
			 uint32_t offset, uint32_t obj_size)
{
	/* FIXME: implement */
	return 0;
}

void r600_clear_surface_reg(struct radeon_device *rdev, int reg)
{
	/* FIXME: implement */
}

static int r600_startup(struct radeon_device *rdev)
{
	struct radeon_ring *ring;
	int r;

	/* enable pcie gen2 link */
	r600_pcie_gen2_enable(rdev);

	r600_mc_program(rdev);

	if (!rdev->me_fw || !rdev->pfp_fw || !rdev->rlc_fw) {
		r = r600_init_microcode(rdev);
		if (r) {
			DRM_ERROR("Failed to load firmware!\n");
			return r;
		}
	}

	r = r600_vram_scratch_init(rdev);
	if (r)
		return r;

	if (rdev->flags & RADEON_IS_AGP) {
		r600_agp_enable(rdev);
	} else {
		r = r600_pcie_gart_enable(rdev);
		if (r)
			return r;
	}
	r600_gpu_init(rdev);
	r = r600_blit_init(rdev);
	if (r) {
		r600_blit_fini(rdev);
		rdev->asic->copy.copy = NULL;
		dev_warn(rdev->dev, "failed blitter (%d) falling back to memcpy\n", r);
	}

	/* allocate wb buffer */
	r = radeon_wb_init(rdev);
	if (r)
		return r;

	r = radeon_fence_driver_start_ring(rdev, RADEON_RING_TYPE_GFX_INDEX);
	if (r) {
		dev_err(rdev->dev, "failed initializing CP fences (%d).\n", r);
		return r;
	}

	r = radeon_fence_driver_start_ring(rdev, R600_RING_TYPE_DMA_INDEX);
	if (r) {
		dev_err(rdev->dev, "failed initializing DMA fences (%d).\n", r);
		return r;
	}

	/* Enable IRQ */
	if (!rdev->irq.installed) {
		r = radeon_irq_kms_init(rdev);
		if (r)
			return r;
	}

	r = r600_irq_init(rdev);
	if (r) {
		DRM_ERROR("radeon: IH init failed (%d).\n", r);
		radeon_irq_kms_fini(rdev);
		return r;
	}
	r600_irq_set(rdev);

	ring = &rdev->ring[RADEON_RING_TYPE_GFX_INDEX];
	r = radeon_ring_init(rdev, ring, ring->ring_size, RADEON_WB_CP_RPTR_OFFSET,
			     R600_CP_RB_RPTR, R600_CP_RB_WPTR,
			     0, 0xfffff, RADEON_CP_PACKET2);
	if (r)
		return r;

	ring = &rdev->ring[R600_RING_TYPE_DMA_INDEX];
	r = radeon_ring_init(rdev, ring, ring->ring_size, R600_WB_DMA_RPTR_OFFSET,
			     DMA_RB_RPTR, DMA_RB_WPTR,
			     2, 0x3fffc, DMA_PACKET(DMA_PACKET_NOP, 0, 0, 0));
	if (r)
		return r;

	r = r600_cp_load_microcode(rdev);
	if (r)
		return r;
	r = r600_cp_resume(rdev);
	if (r)
		return r;

	r = r600_dma_resume(rdev);
	if (r)
		return r;

	r = radeon_ib_pool_init(rdev);
	if (r) {
		dev_err(rdev->dev, "IB initialization failed (%d).\n", r);
		return r;
	}

	r = r600_audio_init(rdev);
	if (r) {
		DRM_ERROR("radeon: audio init failed\n");
		return r;
	}

	return 0;
}

void r600_vga_set_state(struct radeon_device *rdev, bool state)
{
	uint32_t temp;

	temp = RREG32(CONFIG_CNTL);
	if (state == false) {
		temp &= ~(1<<0);
		temp |= (1<<1);
	} else {
		temp &= ~(1<<1);
	}
	WREG32(CONFIG_CNTL, temp);
}

int r600_resume(struct radeon_device *rdev)
{
	int r;

	/* Do not reset GPU before posting, on r600 hw unlike on r500 hw,
	 * posting will perform necessary task to bring back GPU into good
	 * shape.
	 */
	/* post card */
	atom_asic_init(rdev->mode_info.atom_context);

	rdev->accel_working = true;
	r = r600_startup(rdev);
	if (r) {
		DRM_ERROR("r600 startup failed on resume\n");
		rdev->accel_working = false;
		return r;
	}

	return r;
}

int r600_suspend(struct radeon_device *rdev)
{
	r600_audio_fini(rdev);
	r600_cp_stop(rdev);
	r600_dma_stop(rdev);
	r600_irq_suspend(rdev);
	radeon_wb_disable(rdev);
	r600_pcie_gart_disable(rdev);

	return 0;
}

/* Plan is to move initialization in that function and use
 * helper function so that radeon_device_init pretty much
 * do nothing more than calling asic specific function. This
 * should also allow to remove a bunch of callback function
 * like vram_info.
 */
int r600_init(struct radeon_device *rdev)
{
	int r;

	if (r600_debugfs_mc_info_init(rdev)) {
		DRM_ERROR("Failed to register debugfs file for mc !\n");
	}
	/* Read BIOS */
	if (!radeon_get_bios(rdev)) {
		if (ASIC_IS_AVIVO(rdev))
			return -EINVAL;
	}
	/* Must be an ATOMBIOS */
	if (!rdev->is_atom_bios) {
		dev_err(rdev->dev, "Expecting atombios for R600 GPU\n");
		return -EINVAL;
	}
	r = radeon_atombios_init(rdev);
	if (r)
		return r;
	/* Post card if necessary */
	if (!radeon_card_posted(rdev)) {
		if (!rdev->bios) {
			dev_err(rdev->dev, "Card not posted and no BIOS - ignoring\n");
			return -EINVAL;
		}
		DRM_INFO("GPU not posted. posting now...\n");
		atom_asic_init(rdev->mode_info.atom_context);
	}
	/* Initialize scratch registers */
	r600_scratch_init(rdev);
	/* Initialize surface registers */
	radeon_surface_init(rdev);
	/* Initialize clocks */
	radeon_get_clock_info(rdev->ddev);
	/* Fence driver */
	r = radeon_fence_driver_init(rdev);
	if (r)
		return r;
	if (rdev->flags & RADEON_IS_AGP) {
		r = radeon_agp_init(rdev);
		if (r)
			radeon_agp_disable(rdev);
	}
	r = r600_mc_init(rdev);
	if (r)
		return r;
	/* Memory manager */
	r = radeon_bo_init(rdev);
	if (r)
		return r;

	rdev->ring[RADEON_RING_TYPE_GFX_INDEX].ring_obj = NULL;
	r600_ring_init(rdev, &rdev->ring[RADEON_RING_TYPE_GFX_INDEX], 1024 * 1024);

	rdev->ring[R600_RING_TYPE_DMA_INDEX].ring_obj = NULL;
	r600_ring_init(rdev, &rdev->ring[R600_RING_TYPE_DMA_INDEX], 64 * 1024);

	rdev->ih.ring_obj = NULL;
	r600_ih_ring_init(rdev, 64 * 1024);

	r = r600_pcie_gart_init(rdev);
	if (r)
		return r;

	rdev->accel_working = true;
	r = r600_startup(rdev);
	if (r) {
		dev_err(rdev->dev, "disabling GPU acceleration\n");
		r600_cp_fini(rdev);
		r600_dma_fini(rdev);
		r600_irq_fini(rdev);
		radeon_wb_fini(rdev);
		radeon_ib_pool_fini(rdev);
		radeon_irq_kms_fini(rdev);
		r600_pcie_gart_fini(rdev);
		rdev->accel_working = false;
	}

	return 0;
}

void r600_fini(struct radeon_device *rdev)
{
	r600_audio_fini(rdev);
	r600_blit_fini(rdev);
	r600_cp_fini(rdev);
	r600_dma_fini(rdev);
	r600_irq_fini(rdev);
	radeon_wb_fini(rdev);
	radeon_ib_pool_fini(rdev);
	radeon_irq_kms_fini(rdev);
	r600_pcie_gart_fini(rdev);
	r600_vram_scratch_fini(rdev);
	radeon_agp_fini(rdev);
	radeon_gem_fini(rdev);
	radeon_fence_driver_fini(rdev);
	radeon_bo_fini(rdev);
	radeon_atombios_fini(rdev);
	kfree(rdev->bios);
	rdev->bios = NULL;
}


/*
 * CS stuff
 */
void r600_ring_ib_execute(struct radeon_device *rdev, struct radeon_ib *ib)
{
	struct radeon_ring *ring = &rdev->ring[ib->ring];
	u32 next_rptr;

	if (ring->rptr_save_reg) {
		next_rptr = ring->wptr + 3 + 4;
		radeon_ring_write(ring, PACKET3(PACKET3_SET_CONFIG_REG, 1));
		radeon_ring_write(ring, ((ring->rptr_save_reg -
					 PACKET3_SET_CONFIG_REG_OFFSET) >> 2));
		radeon_ring_write(ring, next_rptr);
	} else if (rdev->wb.enabled) {
		next_rptr = ring->wptr + 5 + 4;
		radeon_ring_write(ring, PACKET3(PACKET3_MEM_WRITE, 3));
		radeon_ring_write(ring, ring->next_rptr_gpu_addr & 0xfffffffc);
		radeon_ring_write(ring, (upper_32_bits(ring->next_rptr_gpu_addr) & 0xff) | (1 << 18));
		radeon_ring_write(ring, next_rptr);
		radeon_ring_write(ring, 0);
	}

	radeon_ring_write(ring, PACKET3(PACKET3_INDIRECT_BUFFER, 2));
	radeon_ring_write(ring,
#ifdef __BIG_ENDIAN
			  (2 << 0) |
#endif
			  (ib->gpu_addr & 0xFFFFFFFC));
	radeon_ring_write(ring, upper_32_bits(ib->gpu_addr) & 0xFF);
	radeon_ring_write(ring, ib->length_dw);
}

int r600_ib_test(struct radeon_device *rdev, struct radeon_ring *ring)
{
	struct radeon_ib ib;
	uint32_t scratch;
	uint32_t tmp = 0;
	unsigned i;
	int r;

	r = radeon_scratch_get(rdev, &scratch);
	if (r) {
		DRM_ERROR("radeon: failed to get scratch reg (%d).\n", r);
		return r;
	}
	WREG32(scratch, 0xCAFEDEAD);
	r = radeon_ib_get(rdev, ring->idx, &ib, NULL, 256);
	if (r) {
		DRM_ERROR("radeon: failed to get ib (%d).\n", r);
		goto free_scratch;
	}
	ib.ptr[0] = PACKET3(PACKET3_SET_CONFIG_REG, 1);
	ib.ptr[1] = ((scratch - PACKET3_SET_CONFIG_REG_OFFSET) >> 2);
	ib.ptr[2] = 0xDEADBEEF;
	ib.length_dw = 3;
	r = radeon_ib_schedule(rdev, &ib, NULL);
	if (r) {
		DRM_ERROR("radeon: failed to schedule ib (%d).\n", r);
		goto free_ib;
	}
	r = radeon_fence_wait(ib.fence, false);
	if (r) {
		DRM_ERROR("radeon: fence wait failed (%d).\n", r);
		goto free_ib;
	}
	for (i = 0; i < rdev->usec_timeout; i++) {
		tmp = RREG32(scratch);
		if (tmp == 0xDEADBEEF)
			break;
		udelay(1);
	}
	if (i < rdev->usec_timeout) {
		DRM_INFO("ib test on ring %d succeeded in %u usecs\n", ib.fence->ring, i);
	} else {
		DRM_ERROR("radeon: ib test failed (scratch(0x%04X)=0x%08X)\n",
			  scratch, tmp);
		r = -EINVAL;
	}
free_ib:
	radeon_ib_free(rdev, &ib);
free_scratch:
	radeon_scratch_free(rdev, scratch);
	return r;
}

/**
 * r600_dma_ib_test - test an IB on the DMA engine
 *
 * @rdev: radeon_device pointer
 * @ring: radeon_ring structure holding ring information
 *
 * Test a simple IB in the DMA ring (r6xx-SI).
 * Returns 0 on success, error on failure.
 */
int r600_dma_ib_test(struct radeon_device *rdev, struct radeon_ring *ring)
{
	struct radeon_ib ib;
	unsigned i;
	int r;
	volatile uint32_t *ptr = rdev->vram_scratch.ptr;
	u32 tmp = 0;

	if (!ptr) {
		DRM_ERROR("invalid vram scratch pointer\n");
		return -EINVAL;
	}

	tmp = 0xCAFEDEAD;
	*ptr = tmp;

	r = radeon_ib_get(rdev, ring->idx, &ib, NULL, 256);
	if (r) {
		DRM_ERROR("radeon: failed to get ib (%d).\n", r);
		return r;
	}

	ib.ptr[0] = DMA_PACKET(DMA_PACKET_WRITE, 0, 0, 1);
	ib.ptr[1] = rdev->vram_scratch.gpu_addr & 0xfffffffc;
	ib.ptr[2] = upper_32_bits(rdev->vram_scratch.gpu_addr) & 0xff;
	ib.ptr[3] = 0xDEADBEEF;
	ib.length_dw = 4;

	r = radeon_ib_schedule(rdev, &ib, NULL);
	if (r) {
		radeon_ib_free(rdev, &ib);
		DRM_ERROR("radeon: failed to schedule ib (%d).\n", r);
		return r;
	}
	r = radeon_fence_wait(ib.fence, false);
	if (r) {
		DRM_ERROR("radeon: fence wait failed (%d).\n", r);
		return r;
	}
	for (i = 0; i < rdev->usec_timeout; i++) {
		tmp = *ptr;
		if (tmp == 0xDEADBEEF)
			break;
		udelay(1);
	}
	if (i < rdev->usec_timeout) {
		DRM_INFO("ib test on ring %d succeeded in %u usecs\n", ib.fence->ring, i);
	} else {
		DRM_ERROR("radeon: ib test failed (0x%08X)\n", tmp);
		r = -EINVAL;
	}
	radeon_ib_free(rdev, &ib);
	return r;
}

/**
 * r600_dma_ring_ib_execute - Schedule an IB on the DMA engine
 *
 * @rdev: radeon_device pointer
 * @ib: IB object to schedule
 *
 * Schedule an IB in the DMA ring (r6xx-r7xx).
 */
void r600_dma_ring_ib_execute(struct radeon_device *rdev, struct radeon_ib *ib)
{
	struct radeon_ring *ring = &rdev->ring[ib->ring];

	if (rdev->wb.enabled) {
		u32 next_rptr = ring->wptr + 4;
		while ((next_rptr & 7) != 5)
			next_rptr++;
		next_rptr += 3;
		radeon_ring_write(ring, DMA_PACKET(DMA_PACKET_WRITE, 0, 0, 1));
		radeon_ring_write(ring, ring->next_rptr_gpu_addr & 0xfffffffc);
		radeon_ring_write(ring, upper_32_bits(ring->next_rptr_gpu_addr) & 0xff);
		radeon_ring_write(ring, next_rptr);
	}

	/* The indirect buffer packet must end on an 8 DW boundary in the DMA ring.
	 * Pad as necessary with NOPs.
	 */
	while ((ring->wptr & 7) != 5)
		radeon_ring_write(ring, DMA_PACKET(DMA_PACKET_NOP, 0, 0, 0));
	radeon_ring_write(ring, DMA_PACKET(DMA_PACKET_INDIRECT_BUFFER, 0, 0, 0));
	radeon_ring_write(ring, (ib->gpu_addr & 0xFFFFFFE0));
	radeon_ring_write(ring, (ib->length_dw << 16) | (upper_32_bits(ib->gpu_addr) & 0xFF));

}

/*
 * Interrupts
 *
 * Interrupts use a ring buffer on r6xx/r7xx hardware.  It works pretty
 * the same as the CP ring buffer, but in reverse.  Rather than the CPU
 * writing to the ring and the GPU consuming, the GPU writes to the ring
 * and host consumes.  As the host irq handler processes interrupts, it
 * increments the rptr.  When the rptr catches up with the wptr, all the
 * current interrupts have been processed.
 */

void r600_ih_ring_init(struct radeon_device *rdev, unsigned ring_size)
{
	u32 rb_bufsz;

	/* Align ring size */
	rb_bufsz = drm_order(ring_size / 4);
	ring_size = (1 << rb_bufsz) * 4;
	rdev->ih.ring_size = ring_size;
	rdev->ih.ptr_mask = rdev->ih.ring_size - 1;
	rdev->ih.rptr = 0;
}

int r600_ih_ring_alloc(struct radeon_device *rdev)
{
	int r;

	/* Allocate ring buffer */
	if (rdev->ih.ring_obj == NULL) {
		r = radeon_bo_create(rdev, rdev->ih.ring_size,
				     PAGE_SIZE, true,
				     RADEON_GEM_DOMAIN_GTT,
				     NULL, &rdev->ih.ring_obj);
		if (r) {
			DRM_ERROR("radeon: failed to create ih ring buffer (%d).\n", r);
			return r;
		}
		r = radeon_bo_reserve(rdev->ih.ring_obj, false);
		if (unlikely(r != 0))
			return r;
		r = radeon_bo_pin(rdev->ih.ring_obj,
				  RADEON_GEM_DOMAIN_GTT,
				  &rdev->ih.gpu_addr);
		if (r) {
			radeon_bo_unreserve(rdev->ih.ring_obj);
			DRM_ERROR("radeon: failed to pin ih ring buffer (%d).\n", r);
			return r;
		}
		r = radeon_bo_kmap(rdev->ih.ring_obj,
				   (void **)&rdev->ih.ring);
		radeon_bo_unreserve(rdev->ih.ring_obj);
		if (r) {
			DRM_ERROR("radeon: failed to map ih ring buffer (%d).\n", r);
			return r;
		}
	}
	return 0;
}

void r600_ih_ring_fini(struct radeon_device *rdev)
{
	int r;
	if (rdev->ih.ring_obj) {
		r = radeon_bo_reserve(rdev->ih.ring_obj, false);
		if (likely(r == 0)) {
			radeon_bo_kunmap(rdev->ih.ring_obj);
			radeon_bo_unpin(rdev->ih.ring_obj);
			radeon_bo_unreserve(rdev->ih.ring_obj);
		}
		radeon_bo_unref(&rdev->ih.ring_obj);
		rdev->ih.ring = NULL;
		rdev->ih.ring_obj = NULL;
	}
}

void r600_rlc_stop(struct radeon_device *rdev)
{

	if ((rdev->family >= CHIP_RV770) &&
	    (rdev->family <= CHIP_RV740)) {
		/* r7xx asics need to soft reset RLC before halting */
		WREG32(SRBM_SOFT_RESET, SOFT_RESET_RLC);
		RREG32(SRBM_SOFT_RESET);
		mdelay(15);
		WREG32(SRBM_SOFT_RESET, 0);
		RREG32(SRBM_SOFT_RESET);
	}

	WREG32(RLC_CNTL, 0);
}

void
r600_rlc_start(struct radeon_device *rdev)
{
	WREG32(RLC_CNTL, RLC_ENABLE);
}

static int r600_rlc_init(struct radeon_device *rdev)
{
	u32 i;
	const __be32 *fw_data;

	if (!rdev->rlc_fw)
		return -EINVAL;

	r600_rlc_stop(rdev);

	WREG32(RLC_HB_CNTL, 0);

	if (rdev->family == CHIP_ARUBA) {
		WREG32(TN_RLC_SAVE_AND_RESTORE_BASE, rdev->rlc.save_restore_gpu_addr >> 8);
		WREG32(TN_RLC_CLEAR_STATE_RESTORE_BASE, rdev->rlc.clear_state_gpu_addr >> 8);
	}
	if (rdev->family <= CHIP_CAYMAN) {
		WREG32(RLC_HB_BASE, 0);
		WREG32(RLC_HB_RPTR, 0);
		WREG32(RLC_HB_WPTR, 0);
	}
	if (rdev->family <= CHIP_CAICOS) {
		WREG32(RLC_HB_WPTR_LSB_ADDR, 0);
		WREG32(RLC_HB_WPTR_MSB_ADDR, 0);
	}
	WREG32(RLC_MC_CNTL, 0);
	WREG32(RLC_UCODE_CNTL, 0);

	fw_data = (const __be32 *)rdev->rlc_fw;
	if (rdev->family >= CHIP_ARUBA) {
		for (i = 0; i < ARUBA_RLC_UCODE_SIZE; i++) {
			WREG32(RLC_UCODE_ADDR, i);
			WREG32(RLC_UCODE_DATA, be32_to_cpup(fw_data++));
		}
	} else if (rdev->family >= CHIP_CAYMAN) {
		for (i = 0; i < CAYMAN_RLC_UCODE_SIZE; i++) {
			WREG32(RLC_UCODE_ADDR, i);
			WREG32(RLC_UCODE_DATA, be32_to_cpup(fw_data++));
		}
	} else if (rdev->family >= CHIP_CEDAR) {
		for (i = 0; i < EVERGREEN_RLC_UCODE_SIZE; i++) {
			WREG32(RLC_UCODE_ADDR, i);
			WREG32(RLC_UCODE_DATA, be32_to_cpup(fw_data++));
		}
	} else if (rdev->family >= CHIP_RV770) {
		for (i = 0; i < R700_RLC_UCODE_SIZE; i++) {
			WREG32(RLC_UCODE_ADDR, i);
			WREG32(RLC_UCODE_DATA, be32_to_cpup(fw_data++));
		}
	} else {
		for (i = 0; i < RLC_UCODE_SIZE; i++) {
			WREG32(RLC_UCODE_ADDR, i);
			WREG32(RLC_UCODE_DATA, be32_to_cpup(fw_data++));
		}
	}
	WREG32(RLC_UCODE_ADDR, 0);

	r600_rlc_start(rdev);

	return 0;
}

static void r600_enable_interrupts(struct radeon_device *rdev)
{
	u32 ih_cntl = RREG32(IH_CNTL);
	u32 ih_rb_cntl = RREG32(IH_RB_CNTL);

	ih_cntl |= ENABLE_INTR;
	ih_rb_cntl |= IH_RB_ENABLE;
	WREG32(IH_CNTL, ih_cntl);
	WREG32(IH_RB_CNTL, ih_rb_cntl);
	rdev->ih.enabled = true;
}

void r600_disable_interrupts(struct radeon_device *rdev)
{
	u32 ih_rb_cntl = RREG32(IH_RB_CNTL);
	u32 ih_cntl = RREG32(IH_CNTL);

	ih_rb_cntl &= ~IH_RB_ENABLE;
	ih_cntl &= ~ENABLE_INTR;
	WREG32(IH_RB_CNTL, ih_rb_cntl);
	WREG32(IH_CNTL, ih_cntl);
	/* set rptr, wptr to 0 */
	WREG32(IH_RB_RPTR, 0);
	WREG32(IH_RB_WPTR, 0);
	rdev->ih.enabled = false;
	rdev->ih.rptr = 0;
}

static void r600_disable_interrupt_state(struct radeon_device *rdev)
{
	u32 tmp;

	WREG32(CP_INT_CNTL, CNTX_BUSY_INT_ENABLE | CNTX_EMPTY_INT_ENABLE);
	tmp = RREG32(DMA_CNTL) & ~TRAP_ENABLE;
	WREG32(DMA_CNTL, tmp);
	WREG32(GRBM_INT_CNTL, 0);
	WREG32(DxMODE_INT_MASK, 0);
	WREG32(D1GRPH_INTERRUPT_CONTROL, 0);
	WREG32(D2GRPH_INTERRUPT_CONTROL, 0);
	if (ASIC_IS_DCE3(rdev)) {
		WREG32(DCE3_DACA_AUTODETECT_INT_CONTROL, 0);
		WREG32(DCE3_DACB_AUTODETECT_INT_CONTROL, 0);
		tmp = RREG32(DC_HPD1_INT_CONTROL) & DC_HPDx_INT_POLARITY;
		WREG32(DC_HPD1_INT_CONTROL, tmp);
		tmp = RREG32(DC_HPD2_INT_CONTROL) & DC_HPDx_INT_POLARITY;
		WREG32(DC_HPD2_INT_CONTROL, tmp);
		tmp = RREG32(DC_HPD3_INT_CONTROL) & DC_HPDx_INT_POLARITY;
		WREG32(DC_HPD3_INT_CONTROL, tmp);
		tmp = RREG32(DC_HPD4_INT_CONTROL) & DC_HPDx_INT_POLARITY;
		WREG32(DC_HPD4_INT_CONTROL, tmp);
		if (ASIC_IS_DCE32(rdev)) {
			tmp = RREG32(DC_HPD5_INT_CONTROL) & DC_HPDx_INT_POLARITY;
			WREG32(DC_HPD5_INT_CONTROL, tmp);
			tmp = RREG32(DC_HPD6_INT_CONTROL) & DC_HPDx_INT_POLARITY;
			WREG32(DC_HPD6_INT_CONTROL, tmp);
			tmp = RREG32(AFMT_AUDIO_PACKET_CONTROL + DCE3_HDMI_OFFSET0) & ~HDMI0_AZ_FORMAT_WTRIG_MASK;
			WREG32(AFMT_AUDIO_PACKET_CONTROL + DCE3_HDMI_OFFSET0, tmp);
			tmp = RREG32(AFMT_AUDIO_PACKET_CONTROL + DCE3_HDMI_OFFSET1) & ~HDMI0_AZ_FORMAT_WTRIG_MASK;
			WREG32(AFMT_AUDIO_PACKET_CONTROL + DCE3_HDMI_OFFSET1, tmp);
		} else {
			tmp = RREG32(HDMI0_AUDIO_PACKET_CONTROL) & ~HDMI0_AZ_FORMAT_WTRIG_MASK;
			WREG32(HDMI0_AUDIO_PACKET_CONTROL, tmp);
			tmp = RREG32(DCE3_HDMI1_AUDIO_PACKET_CONTROL) & ~HDMI0_AZ_FORMAT_WTRIG_MASK;
			WREG32(DCE3_HDMI1_AUDIO_PACKET_CONTROL, tmp);
		}
	} else {
		WREG32(DACA_AUTODETECT_INT_CONTROL, 0);
		WREG32(DACB_AUTODETECT_INT_CONTROL, 0);
		tmp = RREG32(DC_HOT_PLUG_DETECT1_INT_CONTROL) & DC_HOT_PLUG_DETECTx_INT_POLARITY;
		WREG32(DC_HOT_PLUG_DETECT1_INT_CONTROL, tmp);
		tmp = RREG32(DC_HOT_PLUG_DETECT2_INT_CONTROL) & DC_HOT_PLUG_DETECTx_INT_POLARITY;
		WREG32(DC_HOT_PLUG_DETECT2_INT_CONTROL, tmp);
		tmp = RREG32(DC_HOT_PLUG_DETECT3_INT_CONTROL) & DC_HOT_PLUG_DETECTx_INT_POLARITY;
		WREG32(DC_HOT_PLUG_DETECT3_INT_CONTROL, tmp);
		tmp = RREG32(HDMI0_AUDIO_PACKET_CONTROL) & ~HDMI0_AZ_FORMAT_WTRIG_MASK;
		WREG32(HDMI0_AUDIO_PACKET_CONTROL, tmp);
		tmp = RREG32(HDMI1_AUDIO_PACKET_CONTROL) & ~HDMI0_AZ_FORMAT_WTRIG_MASK;
		WREG32(HDMI1_AUDIO_PACKET_CONTROL, tmp);
	}
}

int r600_irq_init(struct radeon_device *rdev)
{
	int ret = 0;
	int rb_bufsz;
	u32 interrupt_cntl, ih_cntl, ih_rb_cntl;

	/* allocate ring */
	ret = r600_ih_ring_alloc(rdev);
	if (ret)
		return ret;

	/* disable irqs */
	r600_disable_interrupts(rdev);

	/* init rlc */
	ret = r600_rlc_init(rdev);
	if (ret) {
		r600_ih_ring_fini(rdev);
		return ret;
	}

	/* setup interrupt control */
	/* set dummy read address to ring address */
	WREG32(INTERRUPT_CNTL2, rdev->ih.gpu_addr >> 8);
	interrupt_cntl = RREG32(INTERRUPT_CNTL);
	/* IH_DUMMY_RD_OVERRIDE=0 - dummy read disabled with msi, enabled without msi
	 * IH_DUMMY_RD_OVERRIDE=1 - dummy read controlled by IH_DUMMY_RD_EN
	 */
	interrupt_cntl &= ~IH_DUMMY_RD_OVERRIDE;
	/* IH_REQ_NONSNOOP_EN=1 if ring is in non-cacheable memory, e.g., vram */
	interrupt_cntl &= ~IH_REQ_NONSNOOP_EN;
	WREG32(INTERRUPT_CNTL, interrupt_cntl);

	WREG32(IH_RB_BASE, rdev->ih.gpu_addr >> 8);
	rb_bufsz = drm_order(rdev->ih.ring_size / 4);

	ih_rb_cntl = (IH_WPTR_OVERFLOW_ENABLE |
		      IH_WPTR_OVERFLOW_CLEAR |
		      (rb_bufsz << 1));

	if (rdev->wb.enabled)
		ih_rb_cntl |= IH_WPTR_WRITEBACK_ENABLE;

	/* set the writeback address whether it's enabled or not */
	WREG32(IH_RB_WPTR_ADDR_LO, (rdev->wb.gpu_addr + R600_WB_IH_WPTR_OFFSET) & 0xFFFFFFFC);
	WREG32(IH_RB_WPTR_ADDR_HI, upper_32_bits(rdev->wb.gpu_addr + R600_WB_IH_WPTR_OFFSET) & 0xFF);

	WREG32(IH_RB_CNTL, ih_rb_cntl);

	/* set rptr, wptr to 0 */
	WREG32(IH_RB_RPTR, 0);
	WREG32(IH_RB_WPTR, 0);

	/* Default settings for IH_CNTL (disabled at first) */
	ih_cntl = MC_WRREQ_CREDIT(0x10) | MC_WR_CLEAN_CNT(0x10);
	/* RPTR_REARM only works if msi's are enabled */
	if (rdev->msi_enabled)
		ih_cntl |= RPTR_REARM;
	WREG32(IH_CNTL, ih_cntl);

	/* force the active interrupt state to all disabled */
	if (rdev->family >= CHIP_CEDAR)
		evergreen_disable_interrupt_state(rdev);
	else
		r600_disable_interrupt_state(rdev);

	/* at this point everything should be setup correctly to enable master */
#ifdef notyet
	pci_set_master(rdev->pdev);
#endif

	/* enable irqs */
	r600_enable_interrupts(rdev);

	return ret;
}

void r600_irq_suspend(struct radeon_device *rdev)
{
	r600_irq_disable(rdev);
	r600_rlc_stop(rdev);
}

void r600_irq_fini(struct radeon_device *rdev)
{
	r600_irq_suspend(rdev);
	r600_ih_ring_fini(rdev);
}

int r600_irq_set(struct radeon_device *rdev)
{
	u32 cp_int_cntl = CNTX_BUSY_INT_ENABLE | CNTX_EMPTY_INT_ENABLE;
	u32 mode_int = 0;
	u32 hpd1, hpd2, hpd3, hpd4 = 0, hpd5 = 0, hpd6 = 0;
	u32 grbm_int_cntl = 0;
	u32 hdmi0, hdmi1;
	u32 d1grph = 0, d2grph = 0;
	u32 dma_cntl;

	if (!rdev->irq.installed) {
		WARN(1, "Can't enable IRQ/MSI because no handler is installed\n");
		return -EINVAL;
	}
	/* don't enable anything if the ih is disabled */
	if (!rdev->ih.enabled) {
		r600_disable_interrupts(rdev);
		/* force the active interrupt state to all disabled */
		r600_disable_interrupt_state(rdev);
		return 0;
	}

	if (ASIC_IS_DCE3(rdev)) {
		hpd1 = RREG32(DC_HPD1_INT_CONTROL) & ~DC_HPDx_INT_EN;
		hpd2 = RREG32(DC_HPD2_INT_CONTROL) & ~DC_HPDx_INT_EN;
		hpd3 = RREG32(DC_HPD3_INT_CONTROL) & ~DC_HPDx_INT_EN;
		hpd4 = RREG32(DC_HPD4_INT_CONTROL) & ~DC_HPDx_INT_EN;
		if (ASIC_IS_DCE32(rdev)) {
			hpd5 = RREG32(DC_HPD5_INT_CONTROL) & ~DC_HPDx_INT_EN;
			hpd6 = RREG32(DC_HPD6_INT_CONTROL) & ~DC_HPDx_INT_EN;
			hdmi0 = RREG32(AFMT_AUDIO_PACKET_CONTROL + DCE3_HDMI_OFFSET0) & ~AFMT_AZ_FORMAT_WTRIG_MASK;
			hdmi1 = RREG32(AFMT_AUDIO_PACKET_CONTROL + DCE3_HDMI_OFFSET1) & ~AFMT_AZ_FORMAT_WTRIG_MASK;
		} else {
			hdmi0 = RREG32(HDMI0_AUDIO_PACKET_CONTROL) & ~HDMI0_AZ_FORMAT_WTRIG_MASK;
			hdmi1 = RREG32(DCE3_HDMI1_AUDIO_PACKET_CONTROL) & ~HDMI0_AZ_FORMAT_WTRIG_MASK;
		}
	} else {
		hpd1 = RREG32(DC_HOT_PLUG_DETECT1_INT_CONTROL) & ~DC_HPDx_INT_EN;
		hpd2 = RREG32(DC_HOT_PLUG_DETECT2_INT_CONTROL) & ~DC_HPDx_INT_EN;
		hpd3 = RREG32(DC_HOT_PLUG_DETECT3_INT_CONTROL) & ~DC_HPDx_INT_EN;
		hdmi0 = RREG32(HDMI0_AUDIO_PACKET_CONTROL) & ~HDMI0_AZ_FORMAT_WTRIG_MASK;
		hdmi1 = RREG32(HDMI1_AUDIO_PACKET_CONTROL) & ~HDMI0_AZ_FORMAT_WTRIG_MASK;
	}
	dma_cntl = RREG32(DMA_CNTL) & ~TRAP_ENABLE;

	if (atomic_read(&rdev->irq.ring_int[RADEON_RING_TYPE_GFX_INDEX])) {
		DRM_DEBUG("r600_irq_set: sw int\n");
		cp_int_cntl |= RB_INT_ENABLE;
		cp_int_cntl |= TIME_STAMP_INT_ENABLE;
	}

	if (atomic_read(&rdev->irq.ring_int[R600_RING_TYPE_DMA_INDEX])) {
		DRM_DEBUG("r600_irq_set: sw int dma\n");
		dma_cntl |= TRAP_ENABLE;
	}

	if (rdev->irq.crtc_vblank_int[0] ||
	    atomic_read(&rdev->irq.pflip[0])) {
		DRM_DEBUG("r600_irq_set: vblank 0\n");
		mode_int |= D1MODE_VBLANK_INT_MASK;
	}
	if (rdev->irq.crtc_vblank_int[1] ||
	    atomic_read(&rdev->irq.pflip[1])) {
		DRM_DEBUG("r600_irq_set: vblank 1\n");
		mode_int |= D2MODE_VBLANK_INT_MASK;
	}
	if (rdev->irq.hpd[0]) {
		DRM_DEBUG("r600_irq_set: hpd 1\n");
		hpd1 |= DC_HPDx_INT_EN;
	}
	if (rdev->irq.hpd[1]) {
		DRM_DEBUG("r600_irq_set: hpd 2\n");
		hpd2 |= DC_HPDx_INT_EN;
	}
	if (rdev->irq.hpd[2]) {
		DRM_DEBUG("r600_irq_set: hpd 3\n");
		hpd3 |= DC_HPDx_INT_EN;
	}
	if (rdev->irq.hpd[3]) {
		DRM_DEBUG("r600_irq_set: hpd 4\n");
		hpd4 |= DC_HPDx_INT_EN;
	}
	if (rdev->irq.hpd[4]) {
		DRM_DEBUG("r600_irq_set: hpd 5\n");
		hpd5 |= DC_HPDx_INT_EN;
	}
	if (rdev->irq.hpd[5]) {
		DRM_DEBUG("r600_irq_set: hpd 6\n");
		hpd6 |= DC_HPDx_INT_EN;
	}
	if (rdev->irq.afmt[0]) {
		DRM_DEBUG("r600_irq_set: hdmi 0\n");
		hdmi0 |= HDMI0_AZ_FORMAT_WTRIG_MASK;
	}
	if (rdev->irq.afmt[1]) {
		DRM_DEBUG("r600_irq_set: hdmi 0\n");
		hdmi1 |= HDMI0_AZ_FORMAT_WTRIG_MASK;
	}

	WREG32(CP_INT_CNTL, cp_int_cntl);
	WREG32(DMA_CNTL, dma_cntl);
	WREG32(DxMODE_INT_MASK, mode_int);
	WREG32(D1GRPH_INTERRUPT_CONTROL, d1grph);
	WREG32(D2GRPH_INTERRUPT_CONTROL, d2grph);
	WREG32(GRBM_INT_CNTL, grbm_int_cntl);
	if (ASIC_IS_DCE3(rdev)) {
		WREG32(DC_HPD1_INT_CONTROL, hpd1);
		WREG32(DC_HPD2_INT_CONTROL, hpd2);
		WREG32(DC_HPD3_INT_CONTROL, hpd3);
		WREG32(DC_HPD4_INT_CONTROL, hpd4);
		if (ASIC_IS_DCE32(rdev)) {
			WREG32(DC_HPD5_INT_CONTROL, hpd5);
			WREG32(DC_HPD6_INT_CONTROL, hpd6);
			WREG32(AFMT_AUDIO_PACKET_CONTROL + DCE3_HDMI_OFFSET0, hdmi0);
			WREG32(AFMT_AUDIO_PACKET_CONTROL + DCE3_HDMI_OFFSET1, hdmi1);
		} else {
			WREG32(HDMI0_AUDIO_PACKET_CONTROL, hdmi0);
			WREG32(DCE3_HDMI1_AUDIO_PACKET_CONTROL, hdmi1);
		}
	} else {
		WREG32(DC_HOT_PLUG_DETECT1_INT_CONTROL, hpd1);
		WREG32(DC_HOT_PLUG_DETECT2_INT_CONTROL, hpd2);
		WREG32(DC_HOT_PLUG_DETECT3_INT_CONTROL, hpd3);
		WREG32(HDMI0_AUDIO_PACKET_CONTROL, hdmi0);
		WREG32(HDMI1_AUDIO_PACKET_CONTROL, hdmi1);
	}

	return 0;
}

static void r600_irq_ack(struct radeon_device *rdev)
{
	u32 tmp;

	if (ASIC_IS_DCE3(rdev)) {
		rdev->irq.stat_regs.r600.disp_int = RREG32(DCE3_DISP_INTERRUPT_STATUS);
		rdev->irq.stat_regs.r600.disp_int_cont = RREG32(DCE3_DISP_INTERRUPT_STATUS_CONTINUE);
		rdev->irq.stat_regs.r600.disp_int_cont2 = RREG32(DCE3_DISP_INTERRUPT_STATUS_CONTINUE2);
		if (ASIC_IS_DCE32(rdev)) {
			rdev->irq.stat_regs.r600.hdmi0_status = RREG32(AFMT_STATUS + DCE3_HDMI_OFFSET0);
			rdev->irq.stat_regs.r600.hdmi1_status = RREG32(AFMT_STATUS + DCE3_HDMI_OFFSET1);
		} else {
			rdev->irq.stat_regs.r600.hdmi0_status = RREG32(HDMI0_STATUS);
			rdev->irq.stat_regs.r600.hdmi1_status = RREG32(DCE3_HDMI1_STATUS);
		}
	} else {
		rdev->irq.stat_regs.r600.disp_int = RREG32(DISP_INTERRUPT_STATUS);
		rdev->irq.stat_regs.r600.disp_int_cont = RREG32(DISP_INTERRUPT_STATUS_CONTINUE);
		rdev->irq.stat_regs.r600.disp_int_cont2 = 0;
		rdev->irq.stat_regs.r600.hdmi0_status = RREG32(HDMI0_STATUS);
		rdev->irq.stat_regs.r600.hdmi1_status = RREG32(HDMI1_STATUS);
	}
	rdev->irq.stat_regs.r600.d1grph_int = RREG32(D1GRPH_INTERRUPT_STATUS);
	rdev->irq.stat_regs.r600.d2grph_int = RREG32(D2GRPH_INTERRUPT_STATUS);

	if (rdev->irq.stat_regs.r600.d1grph_int & DxGRPH_PFLIP_INT_OCCURRED)
		WREG32(D1GRPH_INTERRUPT_STATUS, DxGRPH_PFLIP_INT_CLEAR);
	if (rdev->irq.stat_regs.r600.d2grph_int & DxGRPH_PFLIP_INT_OCCURRED)
		WREG32(D2GRPH_INTERRUPT_STATUS, DxGRPH_PFLIP_INT_CLEAR);
	if (rdev->irq.stat_regs.r600.disp_int & LB_D1_VBLANK_INTERRUPT)
		WREG32(D1MODE_VBLANK_STATUS, DxMODE_VBLANK_ACK);
	if (rdev->irq.stat_regs.r600.disp_int & LB_D1_VLINE_INTERRUPT)
		WREG32(D1MODE_VLINE_STATUS, DxMODE_VLINE_ACK);
	if (rdev->irq.stat_regs.r600.disp_int & LB_D2_VBLANK_INTERRUPT)
		WREG32(D2MODE_VBLANK_STATUS, DxMODE_VBLANK_ACK);
	if (rdev->irq.stat_regs.r600.disp_int & LB_D2_VLINE_INTERRUPT)
		WREG32(D2MODE_VLINE_STATUS, DxMODE_VLINE_ACK);
	if (rdev->irq.stat_regs.r600.disp_int & DC_HPD1_INTERRUPT) {
		if (ASIC_IS_DCE3(rdev)) {
			tmp = RREG32(DC_HPD1_INT_CONTROL);
			tmp |= DC_HPDx_INT_ACK;
			WREG32(DC_HPD1_INT_CONTROL, tmp);
		} else {
			tmp = RREG32(DC_HOT_PLUG_DETECT1_INT_CONTROL);
			tmp |= DC_HPDx_INT_ACK;
			WREG32(DC_HOT_PLUG_DETECT1_INT_CONTROL, tmp);
		}
	}
	if (rdev->irq.stat_regs.r600.disp_int & DC_HPD2_INTERRUPT) {
		if (ASIC_IS_DCE3(rdev)) {
			tmp = RREG32(DC_HPD2_INT_CONTROL);
			tmp |= DC_HPDx_INT_ACK;
			WREG32(DC_HPD2_INT_CONTROL, tmp);
		} else {
			tmp = RREG32(DC_HOT_PLUG_DETECT2_INT_CONTROL);
			tmp |= DC_HPDx_INT_ACK;
			WREG32(DC_HOT_PLUG_DETECT2_INT_CONTROL, tmp);
		}
	}
	if (rdev->irq.stat_regs.r600.disp_int_cont & DC_HPD3_INTERRUPT) {
		if (ASIC_IS_DCE3(rdev)) {
			tmp = RREG32(DC_HPD3_INT_CONTROL);
			tmp |= DC_HPDx_INT_ACK;
			WREG32(DC_HPD3_INT_CONTROL, tmp);
		} else {
			tmp = RREG32(DC_HOT_PLUG_DETECT3_INT_CONTROL);
			tmp |= DC_HPDx_INT_ACK;
			WREG32(DC_HOT_PLUG_DETECT3_INT_CONTROL, tmp);
		}
	}
	if (rdev->irq.stat_regs.r600.disp_int_cont & DC_HPD4_INTERRUPT) {
		tmp = RREG32(DC_HPD4_INT_CONTROL);
		tmp |= DC_HPDx_INT_ACK;
		WREG32(DC_HPD4_INT_CONTROL, tmp);
	}
	if (ASIC_IS_DCE32(rdev)) {
		if (rdev->irq.stat_regs.r600.disp_int_cont2 & DC_HPD5_INTERRUPT) {
			tmp = RREG32(DC_HPD5_INT_CONTROL);
			tmp |= DC_HPDx_INT_ACK;
			WREG32(DC_HPD5_INT_CONTROL, tmp);
		}
		if (rdev->irq.stat_regs.r600.disp_int_cont2 & DC_HPD6_INTERRUPT) {
			tmp = RREG32(DC_HPD5_INT_CONTROL);
			tmp |= DC_HPDx_INT_ACK;
			WREG32(DC_HPD6_INT_CONTROL, tmp);
		}
		if (rdev->irq.stat_regs.r600.hdmi0_status & AFMT_AZ_FORMAT_WTRIG) {
			tmp = RREG32(AFMT_AUDIO_PACKET_CONTROL + DCE3_HDMI_OFFSET0);
			tmp |= AFMT_AZ_FORMAT_WTRIG_ACK;
			WREG32(AFMT_AUDIO_PACKET_CONTROL + DCE3_HDMI_OFFSET0, tmp);
		}
		if (rdev->irq.stat_regs.r600.hdmi1_status & AFMT_AZ_FORMAT_WTRIG) {
			tmp = RREG32(AFMT_AUDIO_PACKET_CONTROL + DCE3_HDMI_OFFSET1);
			tmp |= AFMT_AZ_FORMAT_WTRIG_ACK;
			WREG32(AFMT_AUDIO_PACKET_CONTROL + DCE3_HDMI_OFFSET1, tmp);
		}
	} else {
		if (rdev->irq.stat_regs.r600.hdmi0_status & HDMI0_AZ_FORMAT_WTRIG) {
			tmp = RREG32(HDMI0_AUDIO_PACKET_CONTROL);
			tmp |= HDMI0_AZ_FORMAT_WTRIG_ACK;
			WREG32(HDMI0_AUDIO_PACKET_CONTROL, tmp);
		}
		if (rdev->irq.stat_regs.r600.hdmi1_status & HDMI0_AZ_FORMAT_WTRIG) {
			if (ASIC_IS_DCE3(rdev)) {
				tmp = RREG32(DCE3_HDMI1_AUDIO_PACKET_CONTROL);
				tmp |= HDMI0_AZ_FORMAT_WTRIG_ACK;
				WREG32(DCE3_HDMI1_AUDIO_PACKET_CONTROL, tmp);
			} else {
				tmp = RREG32(HDMI1_AUDIO_PACKET_CONTROL);
				tmp |= HDMI0_AZ_FORMAT_WTRIG_ACK;
				WREG32(HDMI1_AUDIO_PACKET_CONTROL, tmp);
			}
		}
	}
}

void r600_irq_disable(struct radeon_device *rdev)
{
	r600_disable_interrupts(rdev);
	/* Wait and acknowledge irq */
	mdelay(1);
	r600_irq_ack(rdev);
	r600_disable_interrupt_state(rdev);
}

static u32 r600_get_ih_wptr(struct radeon_device *rdev)
{
	u32 wptr, tmp;

	if (rdev->wb.enabled)
		wptr = le32_to_cpu(rdev->wb.wb[R600_WB_IH_WPTR_OFFSET/4]);
	else
		wptr = RREG32(IH_RB_WPTR);

	if (wptr & RB_OVERFLOW) {
		/* When a ring buffer overflow happen start parsing interrupt
		 * from the last not overwritten vector (wptr + 16). Hopefully
		 * this should allow us to catchup.
		 */
		dev_warn(rdev->dev, "IH ring buffer overflow (0x%08X, %d, %d)\n",
			wptr, rdev->ih.rptr, (wptr + 16) + rdev->ih.ptr_mask);
		rdev->ih.rptr = (wptr + 16) & rdev->ih.ptr_mask;
		tmp = RREG32(IH_RB_CNTL);
		tmp |= IH_WPTR_OVERFLOW_CLEAR;
		WREG32(IH_RB_CNTL, tmp);
	}
	return (wptr & rdev->ih.ptr_mask);
}

/*        r600 IV Ring
 * Each IV ring entry is 128 bits:
 * [7:0]    - interrupt source id
 * [31:8]   - reserved
 * [59:32]  - interrupt source data
 * [127:60]  - reserved
 *
 * The basic interrupt vector entries
 * are decoded as follows:
 * src_id  src_data  description
 *      1         0  D1 Vblank
 *      1         1  D1 Vline
 *      5         0  D2 Vblank
 *      5         1  D2 Vline
 *     19         0  FP Hot plug detection A
 *     19         1  FP Hot plug detection B
 *     19         2  DAC A auto-detection
 *     19         3  DAC B auto-detection
 *     21         4  HDMI block A
 *     21         5  HDMI block B
 *    176         -  CP_INT RB
 *    177         -  CP_INT IB1
 *    178         -  CP_INT IB2
 *    181         -  EOP Interrupt
 *    233         -  GUI Idle
 *
 * Note, these are based on r600 and may need to be
 * adjusted or added to on newer asics
 */

int r600_irq_process(struct radeon_device *rdev)
{
	u32 wptr;
	u32 rptr;
	u32 src_id, src_data;
	u32 ring_index;
	bool queue_hotplug = false;
	bool queue_hdmi = false;

	if (!rdev->ih.enabled || rdev->shutdown)
		return (0);

	/* No MSIs, need a dummy read to flush PCI DMAs */
	if (!rdev->msi_enabled)
		RREG32(IH_RB_WPTR);

	wptr = r600_get_ih_wptr(rdev);

	if (wptr == rdev->ih.rptr)
		return (0);
restart_ih:
	/* is somebody else already processing irqs? */
	if (atomic_xchg(&rdev->ih.lock, 1))
		return (0);

	rptr = rdev->ih.rptr;
	DRM_DEBUG("r600_irq_process start: rptr %d, wptr %d\n", rptr, wptr);

	/* Order reading of wptr vs. reading of IH ring data */
	DRM_READMEMORYBARRIER();

	/* display interrupts */
	r600_irq_ack(rdev);

	while (rptr != wptr) {
		/* wptr/rptr are in bytes! */
		ring_index = rptr / 4;
		src_id = le32_to_cpu(rdev->ih.ring[ring_index]) & 0xff;
		src_data = le32_to_cpu(rdev->ih.ring[ring_index + 1]) & 0xfffffff;

		switch (src_id) {
		case 1: /* D1 vblank/vline */
			switch (src_data) {
			case 0: /* D1 vblank */
				if (rdev->irq.stat_regs.r600.disp_int & LB_D1_VBLANK_INTERRUPT) {
					if (rdev->irq.crtc_vblank_int[0]) {
						drm_handle_vblank(rdev->ddev, 0);
						rdev->pm.vblank_sync = true;
						wakeup(&rdev->irq.vblank_queue);
					}
					if (atomic_read(&rdev->irq.pflip[0]))
						radeon_crtc_handle_flip(rdev, 0);
					rdev->irq.stat_regs.r600.disp_int &= ~LB_D1_VBLANK_INTERRUPT;
					DRM_DEBUG("IH: D1 vblank\n");
				}
				break;
			case 1: /* D1 vline */
				if (rdev->irq.stat_regs.r600.disp_int & LB_D1_VLINE_INTERRUPT) {
					rdev->irq.stat_regs.r600.disp_int &= ~LB_D1_VLINE_INTERRUPT;
					DRM_DEBUG("IH: D1 vline\n");
				}
				break;
			default:
				DRM_DEBUG("Unhandled interrupt: %d %d\n", src_id, src_data);
				break;
			}
			break;
		case 5: /* D2 vblank/vline */
			switch (src_data) {
			case 0: /* D2 vblank */
				if (rdev->irq.stat_regs.r600.disp_int & LB_D2_VBLANK_INTERRUPT) {
					if (rdev->irq.crtc_vblank_int[1]) {
						drm_handle_vblank(rdev->ddev, 1);
						rdev->pm.vblank_sync = true;
						wakeup(&rdev->irq.vblank_queue);
					}
					if (atomic_read(&rdev->irq.pflip[1]))
						radeon_crtc_handle_flip(rdev, 1);
					rdev->irq.stat_regs.r600.disp_int &= ~LB_D2_VBLANK_INTERRUPT;
					DRM_DEBUG("IH: D2 vblank\n");
				}
				break;
			case 1: /* D1 vline */
				if (rdev->irq.stat_regs.r600.disp_int & LB_D2_VLINE_INTERRUPT) {
					rdev->irq.stat_regs.r600.disp_int &= ~LB_D2_VLINE_INTERRUPT;
					DRM_DEBUG("IH: D2 vline\n");
				}
				break;
			default:
				DRM_DEBUG("Unhandled interrupt: %d %d\n", src_id, src_data);
				break;
			}
			break;
		case 19: /* HPD/DAC hotplug */
			switch (src_data) {
			case 0:
				if (rdev->irq.stat_regs.r600.disp_int & DC_HPD1_INTERRUPT) {
					rdev->irq.stat_regs.r600.disp_int &= ~DC_HPD1_INTERRUPT;
					queue_hotplug = true;
					DRM_DEBUG("IH: HPD1\n");
				}
				break;
			case 1:
				if (rdev->irq.stat_regs.r600.disp_int & DC_HPD2_INTERRUPT) {
					rdev->irq.stat_regs.r600.disp_int &= ~DC_HPD2_INTERRUPT;
					queue_hotplug = true;
					DRM_DEBUG("IH: HPD2\n");
				}
				break;
			case 4:
				if (rdev->irq.stat_regs.r600.disp_int_cont & DC_HPD3_INTERRUPT) {
					rdev->irq.stat_regs.r600.disp_int_cont &= ~DC_HPD3_INTERRUPT;
					queue_hotplug = true;
					DRM_DEBUG("IH: HPD3\n");
				}
				break;
			case 5:
				if (rdev->irq.stat_regs.r600.disp_int_cont & DC_HPD4_INTERRUPT) {
					rdev->irq.stat_regs.r600.disp_int_cont &= ~DC_HPD4_INTERRUPT;
					queue_hotplug = true;
					DRM_DEBUG("IH: HPD4\n");
				}
				break;
			case 10:
				if (rdev->irq.stat_regs.r600.disp_int_cont2 & DC_HPD5_INTERRUPT) {
					rdev->irq.stat_regs.r600.disp_int_cont2 &= ~DC_HPD5_INTERRUPT;
					queue_hotplug = true;
					DRM_DEBUG("IH: HPD5\n");
				}
				break;
			case 12:
				if (rdev->irq.stat_regs.r600.disp_int_cont2 & DC_HPD6_INTERRUPT) {
					rdev->irq.stat_regs.r600.disp_int_cont2 &= ~DC_HPD6_INTERRUPT;
					queue_hotplug = true;
					DRM_DEBUG("IH: HPD6\n");
				}
				break;
			default:
				DRM_DEBUG("Unhandled interrupt: %d %d\n", src_id, src_data);
				break;
			}
			break;
		case 21: /* hdmi */
			switch (src_data) {
			case 4:
				if (rdev->irq.stat_regs.r600.hdmi0_status & HDMI0_AZ_FORMAT_WTRIG) {
					rdev->irq.stat_regs.r600.hdmi0_status &= ~HDMI0_AZ_FORMAT_WTRIG;
					queue_hdmi = true;
					DRM_DEBUG("IH: HDMI0\n");
				}
				break;
			case 5:
				if (rdev->irq.stat_regs.r600.hdmi1_status & HDMI0_AZ_FORMAT_WTRIG) {
					rdev->irq.stat_regs.r600.hdmi1_status &= ~HDMI0_AZ_FORMAT_WTRIG;
					queue_hdmi = true;
					DRM_DEBUG("IH: HDMI1\n");
				}
				break;
			default:
				DRM_ERROR("Unhandled interrupt: %d %d\n", src_id, src_data);
				break;
			}
			break;
		case 176: /* CP_INT in ring buffer */
		case 177: /* CP_INT in IB1 */
		case 178: /* CP_INT in IB2 */
			DRM_DEBUG("IH: CP int: 0x%08x\n", src_data);
			radeon_fence_process(rdev, RADEON_RING_TYPE_GFX_INDEX);
			break;
		case 181: /* CP EOP event */
			DRM_DEBUG("IH: CP EOP\n");
			radeon_fence_process(rdev, RADEON_RING_TYPE_GFX_INDEX);
			break;
		case 224: /* DMA trap event */
			DRM_DEBUG("IH: DMA trap\n");
			radeon_fence_process(rdev, R600_RING_TYPE_DMA_INDEX);
			break;
		case 233: /* GUI IDLE */
			DRM_DEBUG("IH: GUI idle\n");
			break;
		default:
			DRM_DEBUG("Unhandled interrupt: %d %d\n", src_id, src_data);
			break;
		}

		/* wptr/rptr are in bytes! */
		rptr += 16;
		rptr &= rdev->ih.ptr_mask;
	}
	if (queue_hotplug)
		task_add(systq, &rdev->hotplug_task);
	if (queue_hdmi)
		task_add(systq, &rdev->audio_task);
	rdev->ih.rptr = rptr;
	WREG32(IH_RB_RPTR, rdev->ih.rptr);
	atomic_set(&rdev->ih.lock, 0);

	/* make sure wptr hasn't changed while processing */
	wptr = r600_get_ih_wptr(rdev);
	if (wptr != rptr)
		goto restart_ih;

	return (1);
}

/*
 * Debugfs info
 */
#if defined(CONFIG_DEBUG_FS)

static int r600_debugfs_mc_info(struct seq_file *m, void *data)
{
	struct drm_info_node *node = (struct drm_info_node *) m->private;
	struct drm_device *dev = node->minor->dev;
	struct radeon_device *rdev = dev->dev_private;

	DREG32_SYS(m, rdev, R_000E50_SRBM_STATUS);
	DREG32_SYS(m, rdev, VM_L2_STATUS);
	return 0;
}

static struct drm_info_list r600_mc_info_list[] = {
	{"r600_mc_info", r600_debugfs_mc_info, 0, NULL},
};
#endif

int r600_debugfs_mc_info_init(struct radeon_device *rdev)
{
#if defined(CONFIG_DEBUG_FS)
	return radeon_debugfs_add_files(rdev, r600_mc_info_list, ARRAY_SIZE(r600_mc_info_list));
#else
	return 0;
#endif
}

/**
 * r600_ioctl_wait_idle - flush host path cache on wait idle ioctl
 * rdev: radeon device structure
 * bo: buffer object struct which userspace is waiting for idle
 *
 * Some R6XX/R7XX doesn't seems to take into account HDP flush performed
 * through ring buffer, this leads to corruption in rendering, see
 * http://bugzilla.kernel.org/show_bug.cgi?id=15186 to avoid this we
 * directly perform HDP flush by writing register through MMIO.
 */
void r600_ioctl_wait_idle(struct radeon_device *rdev, struct radeon_bo *bo)
{
	/* r7xx hw bug.  write to HDP_DEBUG1 followed by fb read
	 * rather than write to HDP_REG_COHERENCY_FLUSH_CNTL.
	 * This seems to cause problems on some AGP cards. Just use the old
	 * method for them.
	 */
	if ((rdev->family >= CHIP_RV770) && (rdev->family <= CHIP_RV740) &&
	    rdev->vram_scratch.ptr && !(rdev->flags & RADEON_IS_AGP)) {
		volatile uint32_t *ptr = rdev->vram_scratch.ptr;
		u32 tmp;

		WREG32(HDP_DEBUG1, 0);
		tmp = *ptr;
	} else
		WREG32(R_005480_HDP_MEM_COHERENCY_FLUSH_CNTL, 0x1);
}

void r600_set_pcie_lanes(struct radeon_device *rdev, int lanes)
{
	u32 link_width_cntl, mask, target_reg;

	if (rdev->flags & RADEON_IS_IGP)
		return;

	if (!(rdev->flags & RADEON_IS_PCIE))
		return;

	/* x2 cards have a special sequence */
	if (ASIC_IS_X2(rdev))
		return;

	/* FIXME wait for idle */

	switch (lanes) {
	case 0:
		mask = RADEON_PCIE_LC_LINK_WIDTH_X0;
		break;
	case 1:
		mask = RADEON_PCIE_LC_LINK_WIDTH_X1;
		break;
	case 2:
		mask = RADEON_PCIE_LC_LINK_WIDTH_X2;
		break;
	case 4:
		mask = RADEON_PCIE_LC_LINK_WIDTH_X4;
		break;
	case 8:
		mask = RADEON_PCIE_LC_LINK_WIDTH_X8;
		break;
	case 12:
		mask = RADEON_PCIE_LC_LINK_WIDTH_X12;
		break;
	case 16:
	default:
		mask = RADEON_PCIE_LC_LINK_WIDTH_X16;
		break;
	}

	link_width_cntl = RREG32_PCIE_P(RADEON_PCIE_LC_LINK_WIDTH_CNTL);

	if ((link_width_cntl & RADEON_PCIE_LC_LINK_WIDTH_RD_MASK) ==
	    (mask << RADEON_PCIE_LC_LINK_WIDTH_RD_SHIFT))
		return;

	if (link_width_cntl & R600_PCIE_LC_UPCONFIGURE_DIS)
		return;

	link_width_cntl &= ~(RADEON_PCIE_LC_LINK_WIDTH_MASK |
			     RADEON_PCIE_LC_RECONFIG_NOW |
			     R600_PCIE_LC_RENEGOTIATE_EN |
			     R600_PCIE_LC_RECONFIG_ARC_MISSING_ESCAPE);
	link_width_cntl |= mask;

	WREG32_PCIE_P(RADEON_PCIE_LC_LINK_WIDTH_CNTL, link_width_cntl);

        /* some northbridges can renegotiate the link rather than requiring                                  
         * a complete re-config.                                                                             
         * e.g., AMD 780/790 northbridges (pci ids: 0x5956, 0x5957, 0x5958, etc.)                            
         */
        if (link_width_cntl & R600_PCIE_LC_RENEGOTIATION_SUPPORT)
		link_width_cntl |= R600_PCIE_LC_RENEGOTIATE_EN | R600_PCIE_LC_UPCONFIGURE_SUPPORT;
        else
		link_width_cntl |= R600_PCIE_LC_RECONFIG_ARC_MISSING_ESCAPE;

	WREG32_PCIE_P(RADEON_PCIE_LC_LINK_WIDTH_CNTL, (link_width_cntl |
						       RADEON_PCIE_LC_RECONFIG_NOW));

        if (rdev->family >= CHIP_RV770)
		target_reg = R700_TARGET_AND_CURRENT_PROFILE_INDEX;
        else
		target_reg = R600_TARGET_AND_CURRENT_PROFILE_INDEX;

        /* wait for lane set to complete */
        link_width_cntl = RREG32(target_reg);
        while (link_width_cntl == 0xffffffff)
		link_width_cntl = RREG32(target_reg);

}

int r600_get_pcie_lanes(struct radeon_device *rdev)
{
	u32 link_width_cntl;

	if (rdev->flags & RADEON_IS_IGP)
		return 0;

	if (!(rdev->flags & RADEON_IS_PCIE))
		return 0;

	/* x2 cards have a special sequence */
	if (ASIC_IS_X2(rdev))
		return 0;

	/* FIXME wait for idle */

	link_width_cntl = RREG32_PCIE_P(RADEON_PCIE_LC_LINK_WIDTH_CNTL);

	switch ((link_width_cntl & RADEON_PCIE_LC_LINK_WIDTH_RD_MASK) >> RADEON_PCIE_LC_LINK_WIDTH_RD_SHIFT) {
	case RADEON_PCIE_LC_LINK_WIDTH_X0:
		return 0;
	case RADEON_PCIE_LC_LINK_WIDTH_X1:
		return 1;
	case RADEON_PCIE_LC_LINK_WIDTH_X2:
		return 2;
	case RADEON_PCIE_LC_LINK_WIDTH_X4:
		return 4;
	case RADEON_PCIE_LC_LINK_WIDTH_X8:
		return 8;
	case RADEON_PCIE_LC_LINK_WIDTH_X16:
	default:
		return 16;
	}
}

static void r600_pcie_gen2_enable(struct radeon_device *rdev)
{
	u32 link_width_cntl, lanes, speed_cntl, training_cntl, tmp;
	u16 link_cntl2;
	u32 mask;
	int ret;

	if (radeon_pcie_gen2 == 0)
		return;

	if (rdev->flags & RADEON_IS_IGP)
		return;

	if (!(rdev->flags & RADEON_IS_PCIE))
		return;

	/* x2 cards have a special sequence */
	if (ASIC_IS_X2(rdev))
		return;

	/* only RV6xx+ chips are supported */
	if (rdev->family <= CHIP_R600)
		return;

	ret = drm_pcie_get_speed_cap_mask(rdev->ddev, &mask);
	if (ret != 0)
		return;

	if (!(mask & DRM_PCIE_SPEED_50))
		return;

	speed_cntl = RREG32_PCIE_P(PCIE_LC_SPEED_CNTL);
	if (speed_cntl & LC_CURRENT_DATA_RATE) {
		DRM_INFO("PCIE gen 2 link speeds already enabled\n");
		return;
	}

	DRM_INFO("enabling PCIE gen 2 link speeds, disable with radeon.pcie_gen2=0\n");

	/* 55 nm r6xx asics */
	if ((rdev->family == CHIP_RV670) ||
	    (rdev->family == CHIP_RV620) ||
	    (rdev->family == CHIP_RV635)) {
		/* advertise upconfig capability */
		link_width_cntl = RREG32_PCIE_P(PCIE_LC_LINK_WIDTH_CNTL);
		link_width_cntl &= ~LC_UPCONFIGURE_DIS;
		WREG32_PCIE_P(PCIE_LC_LINK_WIDTH_CNTL, link_width_cntl);
		link_width_cntl = RREG32_PCIE_P(PCIE_LC_LINK_WIDTH_CNTL);
		if (link_width_cntl & LC_RENEGOTIATION_SUPPORT) {
			lanes = (link_width_cntl & LC_LINK_WIDTH_RD_MASK) >> LC_LINK_WIDTH_RD_SHIFT;
			link_width_cntl &= ~(LC_LINK_WIDTH_MASK |
					     LC_RECONFIG_ARC_MISSING_ESCAPE);
			link_width_cntl |= lanes | LC_RECONFIG_NOW | LC_RENEGOTIATE_EN;
			WREG32_PCIE_P(PCIE_LC_LINK_WIDTH_CNTL, link_width_cntl);
		} else {
			link_width_cntl |= LC_UPCONFIGURE_DIS;
			WREG32_PCIE_P(PCIE_LC_LINK_WIDTH_CNTL, link_width_cntl);
		}
	}

	speed_cntl = RREG32_PCIE_P(PCIE_LC_SPEED_CNTL);
	if ((speed_cntl & LC_OTHER_SIDE_EVER_SENT_GEN2) &&
	    (speed_cntl & LC_OTHER_SIDE_SUPPORTS_GEN2)) {

		/* 55 nm r6xx asics */
		if ((rdev->family == CHIP_RV670) ||
		    (rdev->family == CHIP_RV620) ||
		    (rdev->family == CHIP_RV635)) {
			WREG32(MM_CFGREGS_CNTL, 0x8);
			link_cntl2 = RREG32(0x4088);
			WREG32(MM_CFGREGS_CNTL, 0);
			/* not supported yet */
			if (link_cntl2 & SELECTABLE_DEEMPHASIS)
				return;
		}

		speed_cntl &= ~LC_SPEED_CHANGE_ATTEMPTS_ALLOWED_MASK;
		speed_cntl |= (0x3 << LC_SPEED_CHANGE_ATTEMPTS_ALLOWED_SHIFT);
		speed_cntl &= ~LC_VOLTAGE_TIMER_SEL_MASK;
		speed_cntl &= ~LC_FORCE_DIS_HW_SPEED_CHANGE;
		speed_cntl |= LC_FORCE_EN_HW_SPEED_CHANGE;
		WREG32_PCIE_P(PCIE_LC_SPEED_CNTL, speed_cntl);

		tmp = RREG32(0x541c);
		WREG32(0x541c, tmp | 0x8);
		WREG32(MM_CFGREGS_CNTL, MM_WR_TO_CFG_EN);
		link_cntl2 = RREG16(0x4088);
		link_cntl2 &= ~TARGET_LINK_SPEED_MASK;
		link_cntl2 |= 0x2;
		WREG16(0x4088, link_cntl2);
		WREG32(MM_CFGREGS_CNTL, 0);

		if ((rdev->family == CHIP_RV670) ||
		    (rdev->family == CHIP_RV620) ||
		    (rdev->family == CHIP_RV635)) {
			training_cntl = RREG32_PCIE_P(PCIE_LC_TRAINING_CNTL);
			training_cntl &= ~LC_POINT_7_PLUS_EN;
			WREG32_PCIE_P(PCIE_LC_TRAINING_CNTL, training_cntl);
		} else {
			speed_cntl = RREG32_PCIE_P(PCIE_LC_SPEED_CNTL);
			speed_cntl &= ~LC_TARGET_LINK_SPEED_OVERRIDE_EN;
			WREG32_PCIE_P(PCIE_LC_SPEED_CNTL, speed_cntl);
		}

		speed_cntl = RREG32_PCIE_P(PCIE_LC_SPEED_CNTL);
		speed_cntl |= LC_GEN2_EN_STRAP;
		WREG32_PCIE_P(PCIE_LC_SPEED_CNTL, speed_cntl);

	} else {
		link_width_cntl = RREG32_PCIE_P(PCIE_LC_LINK_WIDTH_CNTL);
		/* XXX: only disable it if gen1 bridge vendor == 0x111d or 0x1106 */
		if (1)
			link_width_cntl |= LC_UPCONFIGURE_DIS;
		else
			link_width_cntl &= ~LC_UPCONFIGURE_DIS;
		WREG32_PCIE_P(PCIE_LC_LINK_WIDTH_CNTL, link_width_cntl);
	}
}

/**
 * r600_get_gpu_clock - return GPU clock counter snapshot
 *
 * @rdev: radeon_device pointer
 *
 * Fetches a GPU clock counter snapshot (R6xx-cayman).
 * Returns the 64 bit clock counter snapshot.
 */
uint64_t r600_get_gpu_clock(struct radeon_device *rdev)
{
	uint64_t clock;

	mutex_lock(&rdev->gpu_clock_mutex);
	WREG32(RLC_CAPTURE_GPU_CLOCK_COUNT, 1);
	clock = (uint64_t)RREG32(RLC_GPU_CLOCK_COUNT_LSB) |
	        ((uint64_t)RREG32(RLC_GPU_CLOCK_COUNT_MSB) << 32ULL);
	mutex_unlock(&rdev->gpu_clock_mutex);
	return clock;
}
