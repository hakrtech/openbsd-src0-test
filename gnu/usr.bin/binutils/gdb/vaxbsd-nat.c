/* Native-dependent code for VAX BSD's.

   Copyright 2004 Free Software Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include "defs.h"
#include "inferior.h"
#include "regcache.h"

#include <sys/types.h>
#include <sys/ptrace.h>
#include <machine/reg.h>

#include "vax-tdep.h"

/* Supply the general-purpose registers stored in GREGS to REGCACHE.  */

static void
vaxbsd_supply_gregset (struct regcache *regcache, const void *gregs)
{
  const char *regs = gregs;
  int regnum;

  for (regnum = 0; regnum < VAX_NUM_REGS; regnum++)
    regcache_raw_supply (regcache, regnum, regs + regnum * 4);
}

/* Collect the general-purpose registers from REGCACHE and store them
   in GREGS.  */

static void
vaxbsd_collect_gregset (const struct regcache *regcache,
			void *gregs, int regnum)
{
  char *regs = gregs;
  int i;

  for (i = 0; i <= VAX_NUM_REGS; i++)
    {
      if (regnum == -1 || regnum == i)
	regcache_raw_collect (regcache, i, regs + i * 4);
    }
}


/* Fetch register REGNUM from the inferior.  If REGNUM is -1, do this
   for all registers.  */

void
fetch_inferior_registers (int regnum)
{
  struct reg regs;

  if (ptrace (PT_GETREGS, PIDGET (inferior_ptid),
	      (PTRACE_ARG3_TYPE) &regs, 0) == -1)
    perror_with_name ("Couldn't get registers");

  vaxbsd_supply_gregset (current_regcache, &regs);
}

/* Store register REGNUM back into the inferior.  If REGNUM is -1, do
   this for all registers.  */

void
store_inferior_registers (int regnum)
{
  struct reg regs;

  if (ptrace (PT_GETREGS, PIDGET (inferior_ptid),
	      (PTRACE_ARG3_TYPE) &regs, 0) == -1)
    perror_with_name ("Couldn't get registers");

  vaxbsd_collect_gregset (current_regcache, &regs, regnum);

  if (ptrace (PT_SETREGS, PIDGET (inferior_ptid),
	      (PTRACE_ARG3_TYPE) &regs, 0) == -1)
    perror_with_name ("Couldn't write registers");
}


/* Support for debugging kernel virtual memory images.  */

#include <sys/types.h>
#include <machine/pcb.h>

#include "bsd-kvm.h"

static int
vaxbsd_supply_pcb (struct regcache *regcache, struct pcb *pcb)
{
  int regnum;

  /* The following is true for OpenBSD 3.5:

     The pcb contains the register state at the context switch inside
     cpu_switch().  */

  /* The stack pointer shouldn't be zero.  */
  if (pcb->KSP == 0)
    return 0;

  for (regnum = VAX_R0_REGNUM; regnum < VAX_AP_REGNUM; regnum++)
    regcache_raw_supply (regcache, regnum, &pcb->R[regnum - VAX_R0_REGNUM]);
  regcache_raw_supply (regcache, VAX_AP_REGNUM, &pcb->AP);
  regcache_raw_supply (regcache, VAX_FP_REGNUM, &pcb->FP);
  regcache_raw_supply (regcache, VAX_SP_REGNUM, &pcb->KSP);
  regcache_raw_supply (regcache, VAX_PC_REGNUM, &pcb->PC);
  regcache_raw_supply (regcache, VAX_PS_REGNUM, &pcb->PSL);

  return 1;
}


/* Provide a prototype to silence -Wmissing-prototypes.  */
void _initialize_vaxbsd_nat (void);

void
_initialize_vaxbsd_nat (void)
{
  /* Support debugging kernel virtual memory images.  */
  bsd_kvm_add_target (vaxbsd_supply_pcb);
}
