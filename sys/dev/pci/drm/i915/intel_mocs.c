/*
 * Copyright (c) 2015 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions: *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "intel_mocs.h"
#include "intel_lrc.h"
#include "intel_ringbuffer.h"

/* structures required */
struct drm_i915_mocs_entry {
	u32 control_value;
	u16 l3cc_value;
};

struct drm_i915_mocs_table {
	u32 size;
	const struct drm_i915_mocs_entry *table;
};

/* Defines for the tables (XXX_MOCS_0 - XXX_MOCS_63) */
#define LE_CACHEABILITY(value)	((value) << 0)
#define LE_TGT_CACHE(value)	((value) << 2)
#define LE_LRUM(value)		((value) << 4)
#define LE_AOM(value)		((value) << 6)
#define LE_RSC(value)		((value) << 7)
#define LE_SCC(value)		((value) << 8)
#define LE_PFM(value)		((value) << 11)
#define LE_SCF(value)		((value) << 14)

/* Defines for the tables (LNCFMOCS0 - LNCFMOCS31) - two entries per word */
#define L3_ESC(value)		((value) << 0)
#define L3_SCC(value)		((value) << 1)
#define L3_CACHEABILITY(value)	((value) << 4)

/* Helper defines */
#define GEN9_NUM_MOCS_ENTRIES	62  /* 62 out of 64 - 63 & 64 are reserved. */

/* (e)LLC caching options */
#define LE_PAGETABLE		0
#define LE_UC			1
#define LE_WT			2
#define LE_WB			3

/* L3 caching options */
#define L3_DIRECT		0
#define L3_UC			1
#define L3_RESERVED		2
#define L3_WB			3

/* Target cache */
#define ELLC			0
#define LLC			1
#define LLC_ELLC		2

/*
 * MOCS tables
 *
 * These are the MOCS tables that are programmed across all the rings.
 * The control value is programmed to all the rings that support the
 * MOCS registers. While the l3cc_values are only programmed to the
 * LNCFCMOCS0 - LNCFCMOCS32 registers.
 *
 * These tables are intended to be kept reasonably consistent across
 * platforms. However some of the fields are not applicable to all of
 * them.
 *
 * Entries not part of the following tables are undefined as far as
 * userspace is concerned and shouldn't be relied upon.  For the time
 * being they will be implicitly initialized to the strictest caching
 * configuration (uncached) to guarantee forwards compatibility with
 * userspace programs written against more recent kernels providing
 * additional MOCS entries.
 *
 * NOTE: These tables MUST start with being uncached and the length
 *       MUST be less than 63 as the last two registers are reserved
 *       by the hardware.  These tables are part of the kernel ABI and
 *       may only be updated incrementally by adding entries at the
 *       end.
 */
static const struct drm_i915_mocs_entry skylake_mocs_table[] = {
	/* { 0x00000009, 0x0010 } */
	{ (LE_CACHEABILITY(LE_UC) | LE_TGT_CACHE(LLC_ELLC) | LE_LRUM(0) |
	   LE_AOM(0) | LE_RSC(0) | LE_SCC(0) | LE_PFM(0) | LE_SCF(0)),
	  (L3_ESC(0) | L3_SCC(0) | L3_CACHEABILITY(L3_UC)) },
	/* { 0x00000038, 0x0030 } */
	{ (LE_CACHEABILITY(LE_PAGETABLE) | LE_TGT_CACHE(LLC_ELLC) | LE_LRUM(3) |
	   LE_AOM(0) | LE_RSC(0) | LE_SCC(0) | LE_PFM(0) | LE_SCF(0)),
	  (L3_ESC(0) | L3_SCC(0) | L3_CACHEABILITY(L3_WB)) },
	/* { 0x0000003b, 0x0030 } */
	{ (LE_CACHEABILITY(LE_WB) | LE_TGT_CACHE(LLC_ELLC) | LE_LRUM(3) |
	   LE_AOM(0) | LE_RSC(0) | LE_SCC(0) | LE_PFM(0) | LE_SCF(0)),
	  (L3_ESC(0) | L3_SCC(0) | L3_CACHEABILITY(L3_WB)) }
};

/* NOTE: the LE_TGT_CACHE is not used on Broxton */
static const struct drm_i915_mocs_entry broxton_mocs_table[] = {
	/* { 0x00000009, 0x0010 } */
	{ (LE_CACHEABILITY(LE_UC) | LE_TGT_CACHE(LLC_ELLC) | LE_LRUM(0) |
	   LE_AOM(0) | LE_RSC(0) | LE_SCC(0) | LE_PFM(0) | LE_SCF(0)),
	  (L3_ESC(0) | L3_SCC(0) | L3_CACHEABILITY(L3_UC)) },
	/* { 0x00000038, 0x0030 } */
	{ (LE_CACHEABILITY(LE_PAGETABLE) | LE_TGT_CACHE(LLC_ELLC) | LE_LRUM(3) |
	   LE_AOM(0) | LE_RSC(0) | LE_SCC(0) | LE_PFM(0) | LE_SCF(0)),
	  (L3_ESC(0) | L3_SCC(0) | L3_CACHEABILITY(L3_WB)) },
	/* { 0x0000003b, 0x0030 } */
	{ (LE_CACHEABILITY(LE_WB) | LE_TGT_CACHE(LLC_ELLC) | LE_LRUM(3) |
	   LE_AOM(0) | LE_RSC(0) | LE_SCC(0) | LE_PFM(0) | LE_SCF(0)),
	  (L3_ESC(0) | L3_SCC(0) | L3_CACHEABILITY(L3_WB)) }
};

/**
 * get_mocs_settings()
 * @dev:        DRM device.
 * @table:      Output table that will be made to point at appropriate
 *              MOCS values for the device.
 *
 * This function will return the values of the MOCS table that needs to
 * be programmed for the platform. It will return the values that need
 * to be programmed and if they need to be programmed.
 *
 * Return: true if there are applicable MOCS settings for the device.
 */
static bool get_mocs_settings(struct drm_device *dev,
			      struct drm_i915_mocs_table *table)
{
	bool result = false;

	if (IS_SKYLAKE(dev) || IS_KABYLAKE(dev)) {
		table->size  = ARRAY_SIZE(skylake_mocs_table);
		table->table = skylake_mocs_table;
		result = true;
	} else if (IS_BROXTON(dev)) {
		table->size  = ARRAY_SIZE(broxton_mocs_table);
		table->table = broxton_mocs_table;
		result = true;
	} else {
		WARN_ONCE(INTEL_INFO(dev)->gen >= 9,
			  "Platform that should have a MOCS table does not.\n");
	}

	/* WaDisableSkipCaching:skl,bxt,kbl */
	if (IS_GEN9(dev)) {
		int i;

		for (i = 0; i < table->size; i++)
			if (WARN_ON(table->table[i].l3cc_value &
				    (L3_ESC(1) || L3_SCC(0x7))))
			return false;
	}

	return result;
}

/**
 * emit_mocs_control_table() - emit the mocs control table
 * @req:	Request to set up the MOCS table for.
 * @table:	The values to program into the control regs.
 * @reg_base:	The base for the engine that needs to be programmed.
 *
 * This function simply emits a MI_LOAD_REGISTER_IMM command for the
 * given table starting at the given address.
 *
 * Return: 0 on success, otherwise the error status.
 */
static int emit_mocs_control_table(struct drm_i915_gem_request *req,
				   const struct drm_i915_mocs_table *table,
				   u32 reg_base)
{
	struct intel_ringbuffer *ringbuf = req->ringbuf;
	unsigned int index;
	int ret;

	if (WARN_ON(table->size > GEN9_NUM_MOCS_ENTRIES))
		return -ENODEV;

	ret = intel_logical_ring_begin(req, 2 + 2 * GEN9_NUM_MOCS_ENTRIES);
	if (ret) {
		DRM_DEBUG("intel_logical_ring_begin failed %d\n", ret);
		return ret;
	}

	intel_logical_ring_emit(ringbuf,
				MI_LOAD_REGISTER_IMM(GEN9_NUM_MOCS_ENTRIES));

	for (index = 0; index < table->size; index++) {
		intel_logical_ring_emit(ringbuf, reg_base + index * 4);
		intel_logical_ring_emit(ringbuf,
					table->table[index].control_value);
	}

	/*
	 * Ok, now set the unused entries to uncached. These entries
	 * are officially undefined and no contract for the contents
	 * and settings is given for these entries.
	 *
	 * Entry 0 in the table is uncached - so we are just writing
	 * that value to all the used entries.
	 */
	for (; index < GEN9_NUM_MOCS_ENTRIES; index++) {
		intel_logical_ring_emit(ringbuf, reg_base + index * 4);
		intel_logical_ring_emit(ringbuf, table->table[0].control_value);
	}

	intel_logical_ring_emit(ringbuf, MI_NOOP);
	intel_logical_ring_advance(ringbuf);

	return 0;
}

/**
 * emit_mocs_l3cc_table() - emit the mocs control table
 * @req:	Request to set up the MOCS table for.
 * @table:	The values to program into the control regs.
 *
 * This function simply emits a MI_LOAD_REGISTER_IMM command for the
 * given table starting at the given address. This register set is
 * programmed in pairs.
 *
 * Return: 0 on success, otherwise the error status.
 */
static int emit_mocs_l3cc_table(struct drm_i915_gem_request *req,
				const struct drm_i915_mocs_table *table)
{
	struct intel_ringbuffer *ringbuf = req->ringbuf;
	unsigned int count;
	unsigned int i;
	u32 value;
	u32 filler = (table->table[0].l3cc_value & 0xffff) |
			((table->table[0].l3cc_value & 0xffff) << 16);
	int ret;

	if (WARN_ON(table->size > GEN9_NUM_MOCS_ENTRIES))
		return -ENODEV;

	ret = intel_logical_ring_begin(req, 2 + GEN9_NUM_MOCS_ENTRIES);
	if (ret) {
		DRM_DEBUG("intel_logical_ring_begin failed %d\n", ret);
		return ret;
	}

	intel_logical_ring_emit(ringbuf,
			MI_LOAD_REGISTER_IMM(GEN9_NUM_MOCS_ENTRIES / 2));

	for (i = 0, count = 0; i < table->size / 2; i++, count += 2) {
		value = (table->table[count].l3cc_value & 0xffff) |
			((table->table[count + 1].l3cc_value & 0xffff) << 16);

		intel_logical_ring_emit(ringbuf, GEN9_LNCFCMOCS0 + i * 4);
		intel_logical_ring_emit(ringbuf, value);
	}

	if (table->size & 0x01) {
		/* Odd table size - 1 left over */
		value = (table->table[count].l3cc_value & 0xffff) |
			((table->table[0].l3cc_value & 0xffff) << 16);
	} else
		value = filler;

	/*
	 * Now set the rest of the table to uncached - use entry 0 as
	 * this will be uncached. Leave the last pair uninitialised as
	 * they are reserved by the hardware.
	 */
	for (; i < GEN9_NUM_MOCS_ENTRIES / 2; i++) {
		intel_logical_ring_emit(ringbuf, GEN9_LNCFCMOCS0 + i * 4);
		intel_logical_ring_emit(ringbuf, value);

		value = filler;
	}

	intel_logical_ring_emit(ringbuf, MI_NOOP);
	intel_logical_ring_advance(ringbuf);

	return 0;
}

/**
 * intel_rcs_context_init_mocs() - program the MOCS register.
 * @req:	Request to set up the MOCS tables for.
 *
 * This function will emit a batch buffer with the values required for
 * programming the MOCS register values for all the currently supported
 * rings.
 *
 * These registers are partially stored in the RCS context, so they are
 * emitted at the same time so that when a context is created these registers
 * are set up. These registers have to be emitted into the start of the
 * context as setting the ELSP will re-init some of these registers back
 * to the hw values.
 *
 * Return: 0 on success, otherwise the error status.
 */
int intel_rcs_context_init_mocs(struct drm_i915_gem_request *req)
{
	struct drm_i915_mocs_table t;
	int ret;

	if (get_mocs_settings(req->ring->dev, &t)) {
		/* Program the control registers */
		ret = emit_mocs_control_table(req, &t, GEN9_GFX_MOCS_0);
		if (ret)
			return ret;

		ret = emit_mocs_control_table(req, &t, GEN9_MFX0_MOCS_0);
		if (ret)
			return ret;

		ret = emit_mocs_control_table(req, &t, GEN9_MFX1_MOCS_0);
		if (ret)
			return ret;

		ret = emit_mocs_control_table(req, &t, GEN9_VEBOX_MOCS_0);
		if (ret)
			return ret;

		ret = emit_mocs_control_table(req, &t, GEN9_BLT_MOCS_0);
		if (ret)
			return ret;

		/* Now program the l3cc registers */
		ret = emit_mocs_l3cc_table(req, &t);
		if (ret)
			return ret;
	}

	return 0;
}
