; HP-PA  __mpn_rshift --
; This is optimized for the PA7100, where is runs at 3.25 cycles/limb

; Copyright (C) 1992, 1994 Free Software Foundation, Inc.

; This file is part of the GNU MP Library.

; The GNU MP Library is free software; you can redistribute it and/or modify
; it under the terms of the GNU Library General Public License as published by
; the Free Software Foundation; either version 2 of the License, or (at your
; option) any later version.

; The GNU MP Library is distributed in the hope that it will be useful, but
; WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
; or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
; License for more details.

; You should have received a copy of the GNU Library General Public License
; along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
; the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
; MA 02111-1307, USA.


; INPUT PARAMETERS
; res_ptr	gr26
; s_ptr		gr25
; size		gr24
; cnt		gr23

	.code
	.export		__mpn_rshift
__mpn_rshift
	.proc
	.callinfo	frame=64,no_calls
	.entry

	ldws,ma		4(0,%r25),%r22
	mtsar		%r23
	addib,=		-1,%r24,L$0004
	vshd		%r22,%r0,%r28		; compute carry out limb
	ldws,ma		4(0,%r25),%r29
	addib,<=	-5,%r24,L$rest
	vshd		%r29,%r22,%r20

L$loop	ldws,ma		4(0,%r25),%r22
	stws,ma		%r20,4(0,%r26)
	vshd		%r22,%r29,%r20
	ldws,ma		4(0,%r25),%r29
	stws,ma		%r20,4(0,%r26)
	vshd		%r29,%r22,%r20
	ldws,ma		4(0,%r25),%r22
	stws,ma		%r20,4(0,%r26)
	vshd		%r22,%r29,%r20
	ldws,ma		4(0,%r25),%r29
	stws,ma		%r20,4(0,%r26)
	addib,>		-4,%r24,L$loop
	vshd		%r29,%r22,%r20

L$rest	addib,=		4,%r24,L$end1
	nop
L$eloop	ldws,ma		4(0,%r25),%r22
	stws,ma		%r20,4(0,%r26)
	addib,<=	-1,%r24,L$end2
	vshd		%r22,%r29,%r20
	ldws,ma		4(0,%r25),%r29
	stws,ma		%r20,4(0,%r26)
	addib,>		-1,%r24,L$eloop
	vshd		%r29,%r22,%r20

L$end1	stws,ma		%r20,4(0,%r26)
	vshd		%r0,%r29,%r20
	bv		0(%r2)
	stw		%r20,0(0,%r26)
L$end2	stws,ma		%r20,4(0,%r26)
L$0004	vshd		%r0,%r22,%r20
	bv		0(%r2)
	stw		%r20,0(0,%r26)

	.exit
	.procend
