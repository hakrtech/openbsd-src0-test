/* mn10300.h -- Header file for Matsushita 10300 opcode table
   Copyright 1996 Free Software Foundation, Inc.
   Written by Jeff Law, Cygnus Support

This file is part of GDB, GAS, and the GNU binutils.

GDB, GAS, and the GNU binutils are free software; you can redistribute
them and/or modify them under the terms of the GNU General Public
License as published by the Free Software Foundation; either version
1, or (at your option) any later version.

GDB, GAS, and the GNU binutils are distributed in the hope that they
will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this file; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef MN10300_H
#define MN10300_H

/* The opcode table is an array of struct mn10300_opcode.  */

struct mn10300_opcode
{
  /* The opcode name.  */
  const char *name;

  /* The opcode itself.  Those bits which will be filled in with
     operands are zeroes.  */
  unsigned long opcode;

  /* The opcode mask.  This is used by the disassembler.  This is a
     mask containing ones indicating those bits which must match the
     opcode field, and zeroes indicating those bits which need not
     match (and are presumably filled in by operands).  */
  unsigned long mask;

  /* The format of this opcode.  */
  unsigned char format;

  /* An array of operand codes.  Each code is an index into the
     operand table.  They appear in the order which the operands must
     appear in assembly code, and are terminated by a zero.  */
  unsigned char operands[8];
};

/* The table itself is sorted by major opcode number, and is otherwise
   in the order in which the disassembler should consider
   instructions.  */
extern const struct mn10300_opcode mn10300_opcodes[];
extern const int mn10300_num_opcodes;


/* The operands table is an array of struct powerpc_operand.  */

struct mn10300_operand
{
  /* The number of bits in the operand.  */
  int bits;

  /* How far the operand is left shifted in the instruction.  */
  int shift;

  /* One bit syntax flags.  */
  int flags;
};

/* Elements in the table are retrieved by indexing with values from
   the operands field of the mn10300_opcodes table.  */

extern const struct mn10300_operand mn10300_operands[];

/* Values defined for the flags field of a struct mn10300_operand.  */
#define MN10300_OPERAND_DREG 0x1

#define MN10300_OPERAND_AREG 0x2

#define MN10300_OPERAND_SP 0x4

#define MN10300_OPERAND_PSW 0x8

#define MN10300_OPERAND_MDR 0x10

#define MN10300_OPERAND_SIGNED 0x20

#define MN10300_OPERAND_PROMOTE 0x40

#define MN10300_OPERAND_PAREN 0x80

#define MN10300_OPERAND_REPEATED 0x100

#define MN10300_OPERAND_EXTENDED 0x200

#define MN10300_OPERAND_SPLIT 0x400

/* Opcode Formats.  */
#define FMT_S0 1
#define FMT_S1 2
#define FMT_S2 3
#define FMT_S4 4
#define FMT_S6 5
#define FMT_D0 6
#define FMT_D1 7
#define FMT_D2 8
#define FMT_D4 9
#define FMT_D5 10

#endif /* MN10300_H */
