/* Remote debugging interface for ABug Rom monitor for GDB, the GNU debugger.
   Copyright 1995, 1996 Free Software Foundation, Inc.

   Written by Rob Savoye of Cygnus Support

This file is part of GDB.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "defs.h"
#include "gdbcore.h"
#include "target.h"
#include "monitor.h"
#include "serial.h"

static void abug_open PARAMS ((char *args, int from_tty));

static void
abug_supply_register (regname, regnamelen, val, vallen)
     char *regname;
     int regnamelen;
     char *val;
     int vallen;
{
  int regno;

  if (regnamelen != 2)
    return;

  switch (regname[0])
    {
    case 'S':
      if (regname[1] != 'R')
	return;
      regno = PS_REGNUM;
      break;
    case 'P':
      if (regname[1] != 'C')
	return;
      regno = PC_REGNUM;
      break;
    case 'D':
      if (regname[1] < '0' || regname[1] > '7')
	return;
      regno = regname[1] - '0' + D0_REGNUM;
      break;
    case 'A':
      if (regname[1] < '0' || regname[1] > '7')
	return;
      regno = regname[1] - '0' + A0_REGNUM;
      break;
    default:
      return;
    }

  monitor_supply_register (regno, val);
}

/*
 * This array of registers needs to match the indexes used by GDB. The
 * whole reason this exists is because the various ROM monitors use
 * different names than GDB does, and don't support all the
 * registers either. So, typing "info reg sp" becomes an "A7".
 */

static char *abug_regnames[NUM_REGS] =
{
  "D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7",
  "A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7",
  "PC",
};

/*
 * Define the monitor command strings. Since these are passed directly
 * through to a printf style function, we need can include formatting
 * strings. We also need a CR or LF on the end.
 */

static struct target_ops abug_ops;

static char *abug_inits[] = {"\r", NULL};

static struct monitor_ops abug_cmds =
{
  MO_CLR_BREAK_USES_ADDR,
  abug_inits,			/* Init strings */
  "g\r",			/* continue command */
  "t\r",			/* single step */
  NULL,				/* interrupt command */
  "br %x\r",			/* set a breakpoint */
  "nobr %x\r",			/* clear a breakpoint */
  "nobr\r",			/* clear all breakpoints */
  "bf %x:%x %x;b\r",		/* fill (start count val) */
  {
    "ms %x %02x\r",		/* setmem.cmdb (addr, value) */
    "ms %x %04x\r",		/* setmem.cmdw (addr, value) */
    "ms %x %08x\r",		/* setmem.cmdl (addr, value) */
    NULL,			/* setmem.cmdll (addr, value) */
    NULL,			/* setreg.resp_delim */
    NULL,			/* setreg.term */
    NULL,			/* setreg.term_cmd */
  },
  {
    "md %x:%x;b\r",		/* getmem.cmdb (addr, len) */
    "md %x:%x;b\r",		/* getmem.cmdw (addr, len) */
    "md %x:%x;b\r",		/* getmem.cmdl (addr, len) */
    NULL,			/* getmem.cmdll (addr, len) */
    " ",			/* getmem.resp_delim */
    NULL,			/* getmem.term */
    NULL,			/* getmem.term_cmd */
  },
  {
    "rm %s %x\r",		/* setreg.cmd (name, value) */
    "=",			/* setreg.resp_delim */
    "? ",			/* setreg.term */
    ".\r"			/* setreg.term_cmd */
  },
  {
    "rm %s\r",			/* getreg.cmd (name) */
    "=",			/* getreg.resp_delim */
    "? ",			/* getreg.term */
    ".\r"			/* getreg.term_cmd */
  },
  "rd\r",			/* dump_registers */
  "\\(\\w+\\) +=\\([0-9a-fA-F]+\\b\\)", /* register_pattern */
  abug_supply_register,	/* supply_register */
  NULL,				/* load_routine (defaults to SRECs) */
  "lo 0\r",			/* download command */
  "\n",				/* load response */
  "135Bug>",			/* monitor command prompt */
  "\r",				/* end-of-line terminator */
  NULL,				/* optional command terminator */
  &abug_ops,			/* target operations */
  SERIAL_1_STOPBITS,		/* number of stop bits */
  abug_regnames,		/* registers names */
  MONITOR_OPS_MAGIC		/* magic */
  };

static void
abug_open(args, from_tty)
     char *args;
     int from_tty;
{
  monitor_open (args, &abug_cmds, from_tty);
}

void
_initialize_abug_rom ()
{
  init_monitor_ops (&abug_ops);

  abug_ops.to_shortname = "abug";
  abug_ops.to_longname = "ABug monitor";
  abug_ops.to_doc = "Debug via the ABug monitor.\n\
Specify the serial device it is connected to (e.g. /dev/ttya).";
  abug_ops.to_open = abug_open;

  add_target (&abug_ops);
}
