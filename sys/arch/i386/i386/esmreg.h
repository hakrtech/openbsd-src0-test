/*	$OpenBSD: esmreg.h,v 1.7 2005/11/28 22:11:07 jordan Exp $ */

/*
 * Copyright (c) 2005 Jordan Hargrave <jordan@openbsd.org>
 * Copyright (c) 2005 David Gwynne <dlg@openbsd.org>
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

#define ESM2_BASE_PORT		0xe0

#define ESM2_CTRL_REG		4
#define ESM2_DATA_REG		5
#define ESM2_INTMASK_REG	6

#define ESM2_TC_CLR_WPTR	(1L << 0)
#define ESM2_TC_CLR_RPTR	(1L << 1)
#define ESM2_TC_H2ECDB		(1L << 2)
#define ESM2_TC_EC2HDB		(1L << 3)
#define ESM2_TC_EVENTDB		(1L << 4)
#define ESM2_TC_HBDB		(1L << 5)
#define ESM2_TC_HOSTBUSY	(1L << 6)
#define ESM2_TC_ECBUSY		(1L << 7)
#define ESM2_TC_READY		(ESM2_TC_EC2HDB | ESM2_TC_H2ECDB | \
    ESM2_TC_ECBUSY)
#define ESM2_TC_POWER_UP_BITS	(ESM2_TC_CLR_WPTR | ESM2_TC_CLR_RPTR | \
    ESM2_TC_EC2HDB | ESM2_TC_EVENTDB)

#define ESM2_TIM_HIRQ_PEND	(1L << 1)
#define ESM2_TIM_SCI_EN		(1L << 2)
#define ESM2_TIM_SMI_EN		(1L << 3)
#define ESM2_TIM_NMI2SMI	(1L << 4)
#define ESM2_TIM_POWER_UP_BITS	(ESM2_TIM_HIRQ_PEND)

#define ESM2_CMD_NOOP			0x00
#define ESM2_CMD_ECHO			0x01
#define ESM2_CMD_DEVICEMAP		0x03
#define  ESM2_DEVICEMAP_READ			0x00

#define ESM2_CMD_HWDC			0x05 /* Host Watch Dog Control */
#define  ESM2_HWDC_WRITE_STATE			0x01
#define  ESM2_HWDC_READ_PROPERTY		0x02
#define  ESM2_HWDC_WRITE_PROPERTY		0x03

#define ESM2_CMD_SMB_BUF	   	0x20
#define ESM2_CMD_SMB_BUF_XMIT_RECV 	0x21
#define ESM2_CMD_SMB_XMIT_RECV	   	0x22
#define  ESM2_SMB_SENSOR_VALUE			0x04
#define  ESM2_SMB_SENSOR_THRESHOLDS		0x19

#define ESM2_MAX_CMD_LEN	0x20
#define ESM2_UUID_LEN		0x08

#define DELL_SYSSTR_ADDR        0xFE076L
#define DELL_SYSID_ADDR         0xFE840L

#define DELL_SYSID_2300         0x81
#define DELL_SYSID_4300         0x7C
#define DELL_SYSID_4350         0x84
#define DELL_SYSID_6300         0x7F
#define DELL_SYSID_6350         0x83
#define DELL_SYSID_2400         0x9B
#define DELL_SYSID_2450         0xA6
#define DELL_SYSID_4400         0x9A
#define DELL_SYSID_6400         0x9C
#define DELL_SYSID_6450         0xA2
#define DELL_SYSID_2500         0xD9
#define DELL_SYSID_2550         0xD1
#define DELL_SYSID_PV530F       0xCD
#define DELL_SYSID_PV735N       0xE2
#define DELL_SYSID_PV750N       0xEE
#define DELL_SYSID_PV755N       0xEF
#define DELL_SYSID_PA200        0xCB
#define DELL_SYSID_EXT          0xFE

struct dell_sysid {
	u_int16_t    		ext_id;
	u_int8_t     		bios_ver[3];
	u_int8_t     		sys_id;
} __packed;

struct esm_wdog_prop {
	u_int8_t		cmd;
	u_int8_t		reserved;
	u_int8_t		subcmd;
	u_int8_t		action;
	u_int32_t		time;
} __packed;

#define ESM_WDOG_DISABLE	0x00
#define ESM_WDOG_PWROFF		(1L << 1)
#define ESM_WDOG_PWRCYCLE	(1L << 2)
#define ESM_WDOG_RESET		(1L << 3)
#define ESM_WDOG_NOTIFY		(1L << 4)

struct esm_wdog_state {
	u_int8_t		cmd;
	u_int8_t		reserved;
	u_int8_t		subcmd;
	u_int8_t		state;
} __packed;

struct esm_devmap {
	u_int8_t		index;
	u_int8_t		dev_major;
	u_int8_t		dev_minor;
	u_int8_t		rev_major;
	u_int8_t		rev_minor;
	u_int8_t		rev_rom;
	u_int8_t		smb_addr;
	u_int8_t		status;
	u_int8_t		monitor_type;
	u_int8_t		pollcycle;
	u_int8_t		uniqueid[ESM2_UUID_LEN];
} __packed;

struct esm_devmap_req {
	u_int8_t		cmd;
	u_int8_t		initiator;
	u_int8_t		action;
	u_int8_t		index;
	u_int8_t		ndev;
} __packed;

struct esm_devmap_resp {
	u_int8_t		status;
	u_int8_t		ndev;
	struct esm_devmap	devmap[1]; /* request one map at a time */
} __packed;


/* ESM SMB requests */

struct esm_smb_req_hdr {
} __packed;

struct esm_smb_req_val {
	u_int8_t		v_cmd;
	u_int8_t		v_initiator;
	u_int8_t		v_sensor;
} __packed;

struct esm_smb_req {
	struct {
		u_int8_t		_cmd;
		u_int8_t		_dev;
		u_int8_t		_txlen;
		u_int8_t		_rxlen;
	} __packed hdr;
#define h_cmd		hdr._cmd
#define h_dev		hdr._dev
#define h_txlen		hdr._txlen
#define h_rxlen		hdr._rxlen

	union {
		struct esm_smb_req_val	_val;
	} __packed _;
#define req_val		_._val

} __packed;

/* ESM SMB responses */

struct esm_smb_resp_val {
	u_int16_t		v_reading;
	u_int8_t		v_status;
	u_int8_t		v_checksum;
} __packed;

struct esm_smb_resp {
	struct {
		u_int8_t		_status;
		u_int8_t		_i2csts;
		u_int8_t		_procsts;
		u_int8_t		_tx;
		u_int8_t		_rx;
	} __packed hdr;
#define h_status	hdr._status
#define h_i2csts	hdr._i2csts
#define h_procsts	hdr._procsts
#define h_tx		hdr._tx
#define h_rx		hdr._rx

	union {
		struct esm_smb_resp_val	_val;
	} __packed _;
#define resp_val _._val
} __packed;

#define ESM2_VS_VALID		0x07
#define ESM2_VS_SLOT		0x04

#define isValidSensor(state) (((state) & ESM_STATE_MASK) == ESM_VALID_SENSOR)
#define isValidSlot(state)   (((state) & ESM_VALID_SLOT) == ESM_VALID_SLOT)

enum esm_dev_type {
	ESM2_DEV_ESM2 = 1,
	ESM2_DEV_DRACII,
	ESM2_DEV_FRONT_PANEL,
	ESM2_DEV_BACKPLANE2,
	ESM2_DEV_POWERUNIT2,
	ESM2_DEV_ENCL2_BACKPLANE,
	ESM2_DEV_ENCL2_POWERUNIT,
	ESM2_DEV_ENCL1_BACKPLANE,
	ESM2_DEV_ENCL1_POWERUNIT,
	ESM2_DEV_HPPCI,
	ESM2_DEV_BACKPLANE3
};

enum esm_dev_esm2_type {
	ESM2_DEV_ESM2_2300 = 0,
	ESM2_DEV_ESM2_4300,
	ESM2_DEV_ESM2_6300,
	ESM2_DEV_ESM2_6400,
	ESM2_DEV_ESM2_2550,
	ESM2_DEV_ESM2_4350,
	ESM2_DEV_ESM2_6350,
	ESM2_DEV_ESM2_6450,
	ESM2_DEV_ESM2_2400,
	ESM2_DEV_ESM2_4400,
	ESM2_DEV_ESM2_R0, /* reserved */
	ESM2_DEV_ESM2_2500,
	ESM2_DEV_ESM2_2450,
	ESM2_DEV_ESM2_R1, /* reserved */
	ESM2_DEV_ESM2_R2, /* reserved */
	ESM2_DEV_ESM2_2400EX,
	ESM2_DEV_ESM2_2450EX
};

