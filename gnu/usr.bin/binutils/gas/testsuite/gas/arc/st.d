#as: -EL
#objdump: -dr -EL

.*: +file format elf32-.*arc

Disassembly of section .text:

00000000 <.text>:
   0:	00 02 01 10 	10010200     st         r1,\[r2\]
   4:	0e 02 01 10 	1001020e     st         r1,\[r2,14\]
   8:	00 02 41 10 	10410200     stb        r1,\[r2\]
   c:	0e 82 01 11 	1101820e     st.a       r1,\[r3,14\]
  10:	02 02 81 11 	11810202     stw.a      r1,\[r2,2\]
  14:	00 02 1f 10 	101f0200     st         r1,\[0x384\]
  18:	84 03 00 00 
  1c:	00 7e 41 10 	10417e00     stb        0,\[r2\]
  20:	f8 7f 01 10 	10017ff8     st         -8,\[r2,-8\]
  24:	50 7e 1f 10 	101f7e50     st         80,\[0x2ee\]
  28:	9e 02 00 00 
  2c:	00 04 1f 10 	101f0400     st         r2,\[0\]
  30:	00 00 00 00 
			30: R_ARC_32	foo
  34:	02 02 01 14 	14010202     st.di      r1,\[r2,2\]
  38:	03 02 01 15 	15010203     st.a.di    r1,\[r2,3\]
  3c:	04 02 81 15 	15810204     stw.a.di   r1,\[r2,4\]
  40:	00 02 01 12 	12010200     sr         r1,\[r2\]
  44:	0e 82 1f 12 	121f820e     sr         r1,\[0xe\]
