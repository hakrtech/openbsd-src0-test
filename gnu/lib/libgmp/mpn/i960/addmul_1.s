# I960 __mpn_addmul_1 -- Multiply a limb vector with a limb and add
# the result to a second limb vector.

# Copyright (C) 1995 Free Software Foundation, Inc.

# This file is part of the GNU MP Library.

# The GNU MP Library is free software; you can redistribute it and/or modify
# it under the terms of the GNU Library General Public License as published by
# the Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.

# The GNU MP Library is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
# License for more details.

# You should have received a copy of the GNU Library General Public License
# along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
# MA 02111-1307, USA.

.text
	.align	4
	.globl	___mpn_mul_1
___mpn_mul_1:
	subo	g2,0,g2
	shlo	2,g2,g4
	subo	g4,g1,g1
	subo	g4,g0,g13
	mov	0,g0

	cmpo	1,0		# clear C bit on AC.cc

Loop:	ld	(g1)[g2*4],g5
	emul	g3,g5,g6
	ld	(g13)[g2*4],g5

	addc	g0,g6,g6	# relies on that C bit is clear
	addc	0,g7,g7
	addc	g5,g6,g6	# relies on that C bit is clear
	st	g6,(g13)[g2*4]
	addc	0,g7,g0

	addo	g2,1,g2
	cmpobne	0,g2,Loop	# when branch is taken, clears C bit

	ret
