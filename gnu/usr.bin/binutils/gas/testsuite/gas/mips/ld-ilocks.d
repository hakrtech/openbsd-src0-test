#objdump: -dr --prefix-addresses
#name: MIPS ld-ilocks
#source: ld.s
#as:
# Test the ld macro.

.*: +file format .*mips.*

Disassembly of section .text:
0+0000 <.text> lw	a0,0\(zero\)
0+0004 <[^>]*> lw	a1,4\(zero\)
0+0008 <[^>]*> lw	a0,1\(zero\)
0+000c <[^>]*> lw	a1,5\(zero\)
0+0010 <[^>]*> lui	at,0x1
0+0014 <[^>]*> lw	a0,-32768\(at\)
0+0018 <[^>]*> lw	a1,-32764\(at\)
0+001c <[^>]*> lw	a0,-32768\(zero\)
0+0020 <[^>]*> lw	a1,-32764\(zero\)
0+0024 <[^>]*> lui	at,0x1
0+0028 <[^>]*> lw	a0,0\(at\)
0+002c <[^>]*> lw	a1,4\(at\)
0+0030 <[^>]*> lui	at,0x2
0+0034 <[^>]*> lw	a0,-23131\(at\)
0+0038 <[^>]*> lw	a1,-23127\(at\)
0+003c <[^>]*> lw	a0,0\(a1\)
0+0040 <[^>]*> lw	a1,4\(a1\)
0+0044 <[^>]*> lw	a0,1\(a1\)
0+0048 <[^>]*> lw	a1,5\(a1\)
0+004c <[^>]*> lui	at,0x1
0+0050 <[^>]*> addu	at,a1,at
0+0054 <[^>]*> lw	a0,-32768\(at\)
0+0058 <[^>]*> lw	a1,-32764\(at\)
0+005c <[^>]*> lw	a0,-32768\(a1\)
0+0060 <[^>]*> lw	a1,-32764\(a1\)
0+0064 <[^>]*> lui	at,0x1
0+0068 <[^>]*> addu	at,a1,at
0+006c <[^>]*> lw	a0,0\(at\)
0+0070 <[^>]*> lw	a1,4\(at\)
0+0074 <[^>]*> lui	at,0x2
0+0078 <[^>]*> addu	at,a1,at
0+007c <[^>]*> lw	a0,-23131\(at\)
0+0080 <[^>]*> lw	a1,-23127\(at\)
0+0084 <[^>]*> lui	at,0x0
[ 	]*84: [A-Z0-9_]*HI[A-Z0-9_]*	.data.*
0+0088 <[^>]*> lw	a0,0\(at\)
[ 	]*88: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
0+008c <[^>]*> lw	a1,4\(at\)
[ 	]*8c: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
0+0090 <[^>]*> lui	at,0x0
[ 	]*90: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_data_label
0+0094 <[^>]*> lw	a0,0\(at\)
[ 	]*94: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
0+0098 <[^>]*> lw	a1,4\(at\)
[ 	]*98: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
0+009c <[^>]*> lw	a0,0\(gp\)
[ 	]*9c: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_data_label
0+00a0 <[^>]*> lw	a1,4\(gp\)
[ 	]*a0: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_data_label
0+00a4 <[^>]*> lui	at,0x0
[ 	]*a4: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
0+00a8 <[^>]*> lw	a0,0\(at\)
[ 	]*a8: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
0+00ac <[^>]*> lw	a1,4\(at\)
[ 	]*ac: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
0+00b0 <[^>]*> lw	a0,0\(gp\)
[ 	]*b0: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_common
0+00b4 <[^>]*> lw	a1,4\(gp\)
[ 	]*b4: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_common
0+00b8 <[^>]*> lui	at,0x0
[ 	]*b8: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
0+00bc <[^>]*> lw	a0,0\(at\)
[ 	]*bc: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
0+00c0 <[^>]*> lw	a1,4\(at\)
[ 	]*c0: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
0+00c4 <[^>]*> lw	a0,-16384\(gp\)
[ 	]*c4: [A-Z0-9_]*GPREL[A-Z0-9_]*	.sbss.*
0+00c8 <[^>]*> lw	a1,-16380\(gp\)
[ 	]*c8: [A-Z0-9_]*GPREL[A-Z0-9_]*	.sbss.*
0+00cc <[^>]*> lui	at,0x0
[ 	]*cc: [A-Z0-9_]*HI[A-Z0-9_]*	.data.*
0+00d0 <[^>]*> lw	a0,1\(at\)
[ 	]*d0: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
0+00d4 <[^>]*> lw	a1,5\(at\)
[ 	]*d4: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
0+00d8 <[^>]*> lui	at,0x0
[ 	]*d8: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_data_label
0+00dc <[^>]*> lw	a0,1\(at\)
[ 	]*dc: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
0+00e0 <[^>]*> lw	a1,5\(at\)
[ 	]*e0: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
0+00e4 <[^>]*> lw	a0,1\(gp\)
[ 	]*e4: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_data_label
0+00e8 <[^>]*> lw	a1,5\(gp\)
[ 	]*e8: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_data_label
0+00ec <[^>]*> lui	at,0x0
[ 	]*ec: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
0+00f0 <[^>]*> lw	a0,1\(at\)
[ 	]*f0: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
0+00f4 <[^>]*> lw	a1,5\(at\)
[ 	]*f4: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
0+00f8 <[^>]*> lw	a0,1\(gp\)
[ 	]*f8: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_common
0+00fc <[^>]*> lw	a1,5\(gp\)
[ 	]*fc: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_common
0+0100 <[^>]*> lui	at,0x0
[ 	]*100: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
0+0104 <[^>]*> lw	a0,1\(at\)
[ 	]*104: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
0+0108 <[^>]*> lw	a1,5\(at\)
[ 	]*108: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
0+010c <[^>]*> lw	a0,-16383\(gp\)
[ 	]*10c: [A-Z0-9_]*GPREL[A-Z0-9_]*	.sbss.*
0+0110 <[^>]*> lw	a1,-16379\(gp\)
[ 	]*110: [A-Z0-9_]*GPREL[A-Z0-9_]*	.sbss.*
0+0114 <[^>]*> lui	at,0x1
[ 	]*114: [A-Z0-9_]*HI[A-Z0-9_]*	.data.*
0+0118 <[^>]*> lw	a0,-32768\(at\)
[ 	]*118: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
0+011c <[^>]*> lw	a1,-32764\(at\)
[ 	]*11c: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
0+0120 <[^>]*> lui	at,0x1
[ 	]*120: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_data_label
0+0124 <[^>]*> lw	a0,-32768\(at\)
[ 	]*124: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
0+0128 <[^>]*> lw	a1,-32764\(at\)
[ 	]*128: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
0+012c <[^>]*> lui	at,0x1
[ 	]*12c: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_data_label
0+0130 <[^>]*> lw	a0,-32768\(at\)
[ 	]*130: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
0+0134 <[^>]*> lw	a1,-32764\(at\)
[ 	]*134: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
0+0138 <[^>]*> lui	at,0x1
[ 	]*138: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
0+013c <[^>]*> lw	a0,-32768\(at\)
[ 	]*13c: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
0+0140 <[^>]*> lw	a1,-32764\(at\)
[ 	]*140: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
0+0144 <[^>]*> lui	at,0x1
[ 	]*144: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_common
0+0148 <[^>]*> lw	a0,-32768\(at\)
[ 	]*148: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
0+014c <[^>]*> lw	a1,-32764\(at\)
[ 	]*14c: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
0+0150 <[^>]*> lui	at,0x1
[ 	]*150: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
0+0154 <[^>]*> lw	a0,-32768\(at\)
[ 	]*154: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
0+0158 <[^>]*> lw	a1,-32764\(at\)
[ 	]*158: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
0+015c <[^>]*> lui	at,0x1
[ 	]*15c: [A-Z0-9_]*HI[A-Z0-9_]*	.sbss.*
0+0160 <[^>]*> lw	a0,-32768\(at\)
[ 	]*160: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
0+0164 <[^>]*> lw	a1,-32764\(at\)
[ 	]*164: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
0+0168 <[^>]*> lui	at,0x0
[ 	]*168: [A-Z0-9_]*HI[A-Z0-9_]*	.data.*
0+016c <[^>]*> lw	a0,-32768\(at\)
[ 	]*16c: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
0+0170 <[^>]*> lw	a1,-32764\(at\)
[ 	]*170: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
0+0174 <[^>]*> lui	at,0x0
[ 	]*174: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_data_label
0+0178 <[^>]*> lw	a0,-32768\(at\)
[ 	]*178: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
0+017c <[^>]*> lw	a1,-32764\(at\)
[ 	]*17c: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
0+0180 <[^>]*> lui	at,0x0
[ 	]*180: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_data_label
0+0184 <[^>]*> lw	a0,-32768\(at\)
[ 	]*184: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
0+0188 <[^>]*> lw	a1,-32764\(at\)
[ 	]*188: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
0+018c <[^>]*> lui	at,0x0
[ 	]*18c: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
0+0190 <[^>]*> lw	a0,-32768\(at\)
[ 	]*190: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
0+0194 <[^>]*> lw	a1,-32764\(at\)
[ 	]*194: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
0+0198 <[^>]*> lui	at,0x0
[ 	]*198: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_common
0+019c <[^>]*> lw	a0,-32768\(at\)
[ 	]*19c: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
0+01a0 <[^>]*> lw	a1,-32764\(at\)
[ 	]*1a0: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
0+01a4 <[^>]*> lui	at,0x0
[ 	]*1a4: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
0+01a8 <[^>]*> lw	a0,-32768\(at\)
[ 	]*1a8: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
0+01ac <[^>]*> lw	a1,-32764\(at\)
[ 	]*1ac: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
0+01b0 <[^>]*> lui	at,0x0
[ 	]*1b0: [A-Z0-9_]*HI[A-Z0-9_]*	.sbss.*
0+01b4 <[^>]*> lw	a0,-32768\(at\)
[ 	]*1b4: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
0+01b8 <[^>]*> lw	a1,-32764\(at\)
[ 	]*1b8: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
0+01bc <[^>]*> lui	at,0x1
[ 	]*1bc: [A-Z0-9_]*HI[A-Z0-9_]*	.data.*
0+01c0 <[^>]*> lw	a0,0\(at\)
[ 	]*1c0: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
0+01c4 <[^>]*> lw	a1,4\(at\)
[ 	]*1c4: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
0+01c8 <[^>]*> lui	at,0x1
[ 	]*1c8: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_data_label
0+01cc <[^>]*> lw	a0,0\(at\)
[ 	]*1cc: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
0+01d0 <[^>]*> lw	a1,4\(at\)
[ 	]*1d0: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
0+01d4 <[^>]*> lui	at,0x1
[ 	]*1d4: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_data_label
0+01d8 <[^>]*> lw	a0,0\(at\)
[ 	]*1d8: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
0+01dc <[^>]*> lw	a1,4\(at\)
[ 	]*1dc: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
0+01e0 <[^>]*> lui	at,0x1
[ 	]*1e0: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
0+01e4 <[^>]*> lw	a0,0\(at\)
[ 	]*1e4: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
0+01e8 <[^>]*> lw	a1,4\(at\)
[ 	]*1e8: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
0+01ec <[^>]*> lui	at,0x1
[ 	]*1ec: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_common
0+01f0 <[^>]*> lw	a0,0\(at\)
[ 	]*1f0: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
0+01f4 <[^>]*> lw	a1,4\(at\)
[ 	]*1f4: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
0+01f8 <[^>]*> lui	at,0x1
[ 	]*1f8: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
0+01fc <[^>]*> lw	a0,0\(at\)
[ 	]*1fc: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
0+0200 <[^>]*> lw	a1,4\(at\)
[ 	]*200: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
0+0204 <[^>]*> lui	at,0x1
[ 	]*204: [A-Z0-9_]*HI[A-Z0-9_]*	.sbss.*
0+0208 <[^>]*> lw	a0,0\(at\)
[ 	]*208: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
0+020c <[^>]*> lw	a1,4\(at\)
[ 	]*20c: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
0+0210 <[^>]*> lui	at,0x2
[ 	]*210: [A-Z0-9_]*HI[A-Z0-9_]*	.data.*
0+0214 <[^>]*> lw	a0,-23131\(at\)
[ 	]*214: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
0+0218 <[^>]*> lw	a1,-23127\(at\)
[ 	]*218: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
0+021c <[^>]*> lui	at,0x2
[ 	]*21c: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_data_label
0+0220 <[^>]*> lw	a0,-23131\(at\)
[ 	]*220: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
0+0224 <[^>]*> lw	a1,-23127\(at\)
[ 	]*224: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
0+0228 <[^>]*> lui	at,0x2
[ 	]*228: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_data_label
0+022c <[^>]*> lw	a0,-23131\(at\)
[ 	]*22c: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
0+0230 <[^>]*> lw	a1,-23127\(at\)
[ 	]*230: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
0+0234 <[^>]*> lui	at,0x2
[ 	]*234: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
0+0238 <[^>]*> lw	a0,-23131\(at\)
[ 	]*238: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
0+023c <[^>]*> lw	a1,-23127\(at\)
[ 	]*23c: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
0+0240 <[^>]*> lui	at,0x2
[ 	]*240: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_common
0+0244 <[^>]*> lw	a0,-23131\(at\)
[ 	]*244: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
0+0248 <[^>]*> lw	a1,-23127\(at\)
[ 	]*248: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
0+024c <[^>]*> lui	at,0x2
[ 	]*24c: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
0+0250 <[^>]*> lw	a0,-23131\(at\)
[ 	]*250: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
0+0254 <[^>]*> lw	a1,-23127\(at\)
[ 	]*254: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
0+0258 <[^>]*> lui	at,0x2
[ 	]*258: [A-Z0-9_]*HI[A-Z0-9_]*	.sbss.*
0+025c <[^>]*> lw	a0,-23131\(at\)
[ 	]*25c: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
0+0260 <[^>]*> lw	a1,-23127\(at\)
[ 	]*260: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
0+0264 <[^>]*> lui	at,0x0
[ 	]*264: [A-Z0-9_]*HI[A-Z0-9_]*	.data.*
0+0268 <[^>]*> addu	at,a1,at
0+026c <[^>]*> lw	a0,0\(at\)
[ 	]*26c: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
0+0270 <[^>]*> lw	a1,4\(at\)
[ 	]*270: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
0+0274 <[^>]*> lui	at,0x0
[ 	]*274: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_data_label
0+0278 <[^>]*> addu	at,a1,at
0+027c <[^>]*> lw	a0,0\(at\)
[ 	]*27c: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
0+0280 <[^>]*> lw	a1,4\(at\)
[ 	]*280: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
0+0284 <[^>]*> addu	at,a1,gp
0+0288 <[^>]*> lw	a0,0\(at\)
[ 	]*288: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_data_label
0+028c <[^>]*> lw	a1,4\(at\)
[ 	]*28c: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_data_label
0+0290 <[^>]*> lui	at,0x0
[ 	]*290: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
0+0294 <[^>]*> addu	at,a1,at
0+0298 <[^>]*> lw	a0,0\(at\)
[ 	]*298: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
0+029c <[^>]*> lw	a1,4\(at\)
[ 	]*29c: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
0+02a0 <[^>]*> addu	at,a1,gp
0+02a4 <[^>]*> lw	a0,0\(at\)
[ 	]*2a4: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_common
0+02a8 <[^>]*> lw	a1,4\(at\)
[ 	]*2a8: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_common
0+02ac <[^>]*> lui	at,0x0
[ 	]*2ac: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
0+02b0 <[^>]*> addu	at,a1,at
0+02b4 <[^>]*> lw	a0,0\(at\)
[ 	]*2b4: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
0+02b8 <[^>]*> lw	a1,4\(at\)
[ 	]*2b8: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
0+02bc <[^>]*> addu	at,a1,gp
0+02c0 <[^>]*> lw	a0,-16384\(at\)
[ 	]*2c0: [A-Z0-9_]*GPREL[A-Z0-9_]*	.sbss.*
0+02c4 <[^>]*> lw	a1,-16380\(at\)
[ 	]*2c4: [A-Z0-9_]*GPREL[A-Z0-9_]*	.sbss.*
0+02c8 <[^>]*> lui	at,0x0
[ 	]*2c8: [A-Z0-9_]*HI[A-Z0-9_]*	.data.*
0+02cc <[^>]*> addu	at,a1,at
0+02d0 <[^>]*> lw	a0,1\(at\)
[ 	]*2d0: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
0+02d4 <[^>]*> lw	a1,5\(at\)
[ 	]*2d4: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
0+02d8 <[^>]*> lui	at,0x0
[ 	]*2d8: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_data_label
0+02dc <[^>]*> addu	at,a1,at
0+02e0 <[^>]*> lw	a0,1\(at\)
[ 	]*2e0: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
0+02e4 <[^>]*> lw	a1,5\(at\)
[ 	]*2e4: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
0+02e8 <[^>]*> addu	at,a1,gp
0+02ec <[^>]*> lw	a0,1\(at\)
[ 	]*2ec: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_data_label
0+02f0 <[^>]*> lw	a1,5\(at\)
[ 	]*2f0: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_data_label
0+02f4 <[^>]*> lui	at,0x0
[ 	]*2f4: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
0+02f8 <[^>]*> addu	at,a1,at
0+02fc <[^>]*> lw	a0,1\(at\)
[ 	]*2fc: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
0+0300 <[^>]*> lw	a1,5\(at\)
[ 	]*300: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
0+0304 <[^>]*> addu	at,a1,gp
0+0308 <[^>]*> lw	a0,1\(at\)
[ 	]*308: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_common
0+030c <[^>]*> lw	a1,5\(at\)
[ 	]*30c: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_common
0+0310 <[^>]*> lui	at,0x0
[ 	]*310: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
0+0314 <[^>]*> addu	at,a1,at
0+0318 <[^>]*> lw	a0,1\(at\)
[ 	]*318: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
0+031c <[^>]*> lw	a1,5\(at\)
[ 	]*31c: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
0+0320 <[^>]*> addu	at,a1,gp
0+0324 <[^>]*> lw	a0,-16383\(at\)
[ 	]*324: [A-Z0-9_]*GPREL[A-Z0-9_]*	.sbss.*
0+0328 <[^>]*> lw	a1,-16379\(at\)
[ 	]*328: [A-Z0-9_]*GPREL[A-Z0-9_]*	.sbss.*
0+032c <[^>]*> lui	at,0x1
[ 	]*32c: [A-Z0-9_]*HI[A-Z0-9_]*	.data.*
0+0330 <[^>]*> addu	at,a1,at
0+0334 <[^>]*> lw	a0,-32768\(at\)
[ 	]*334: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
0+0338 <[^>]*> lw	a1,-32764\(at\)
[ 	]*338: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
0+033c <[^>]*> lui	at,0x1
[ 	]*33c: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_data_label
0+0340 <[^>]*> addu	at,a1,at
0+0344 <[^>]*> lw	a0,-32768\(at\)
[ 	]*344: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
0+0348 <[^>]*> lw	a1,-32764\(at\)
[ 	]*348: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
0+034c <[^>]*> lui	at,0x1
[ 	]*34c: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_data_label
0+0350 <[^>]*> addu	at,a1,at
0+0354 <[^>]*> lw	a0,-32768\(at\)
[ 	]*354: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
0+0358 <[^>]*> lw	a1,-32764\(at\)
[ 	]*358: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
0+035c <[^>]*> lui	at,0x1
[ 	]*35c: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
0+0360 <[^>]*> addu	at,a1,at
0+0364 <[^>]*> lw	a0,-32768\(at\)
[ 	]*364: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
0+0368 <[^>]*> lw	a1,-32764\(at\)
[ 	]*368: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
0+036c <[^>]*> lui	at,0x1
[ 	]*36c: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_common
0+0370 <[^>]*> addu	at,a1,at
0+0374 <[^>]*> lw	a0,-32768\(at\)
[ 	]*374: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
0+0378 <[^>]*> lw	a1,-32764\(at\)
[ 	]*378: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
0+037c <[^>]*> lui	at,0x1
[ 	]*37c: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
0+0380 <[^>]*> addu	at,a1,at
0+0384 <[^>]*> lw	a0,-32768\(at\)
[ 	]*384: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
0+0388 <[^>]*> lw	a1,-32764\(at\)
[ 	]*388: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
0+038c <[^>]*> lui	at,0x1
[ 	]*38c: [A-Z0-9_]*HI[A-Z0-9_]*	.sbss.*
0+0390 <[^>]*> addu	at,a1,at
0+0394 <[^>]*> lw	a0,-32768\(at\)
[ 	]*394: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
0+0398 <[^>]*> lw	a1,-32764\(at\)
[ 	]*398: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
0+039c <[^>]*> lui	at,0x0
[ 	]*39c: [A-Z0-9_]*HI[A-Z0-9_]*	.data.*
0+03a0 <[^>]*> addu	at,a1,at
0+03a4 <[^>]*> lw	a0,-32768\(at\)
[ 	]*3a4: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
0+03a8 <[^>]*> lw	a1,-32764\(at\)
[ 	]*3a8: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
0+03ac <[^>]*> lui	at,0x0
[ 	]*3ac: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_data_label
0+03b0 <[^>]*> addu	at,a1,at
0+03b4 <[^>]*> lw	a0,-32768\(at\)
[ 	]*3b4: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
0+03b8 <[^>]*> lw	a1,-32764\(at\)
[ 	]*3b8: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
0+03bc <[^>]*> lui	at,0x0
[ 	]*3bc: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_data_label
0+03c0 <[^>]*> addu	at,a1,at
0+03c4 <[^>]*> lw	a0,-32768\(at\)
[ 	]*3c4: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
0+03c8 <[^>]*> lw	a1,-32764\(at\)
[ 	]*3c8: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
0+03cc <[^>]*> lui	at,0x0
[ 	]*3cc: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
0+03d0 <[^>]*> addu	at,a1,at
0+03d4 <[^>]*> lw	a0,-32768\(at\)
[ 	]*3d4: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
0+03d8 <[^>]*> lw	a1,-32764\(at\)
[ 	]*3d8: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
0+03dc <[^>]*> lui	at,0x0
[ 	]*3dc: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_common
0+03e0 <[^>]*> addu	at,a1,at
0+03e4 <[^>]*> lw	a0,-32768\(at\)
[ 	]*3e4: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
0+03e8 <[^>]*> lw	a1,-32764\(at\)
[ 	]*3e8: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
0+03ec <[^>]*> lui	at,0x0
[ 	]*3ec: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
0+03f0 <[^>]*> addu	at,a1,at
0+03f4 <[^>]*> lw	a0,-32768\(at\)
[ 	]*3f4: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
0+03f8 <[^>]*> lw	a1,-32764\(at\)
[ 	]*3f8: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
0+03fc <[^>]*> lui	at,0x0
[ 	]*3fc: [A-Z0-9_]*HI[A-Z0-9_]*	.sbss.*
0+0400 <[^>]*> addu	at,a1,at
0+0404 <[^>]*> lw	a0,-32768\(at\)
[ 	]*404: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
0+0408 <[^>]*> lw	a1,-32764\(at\)
[ 	]*408: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
0+040c <[^>]*> lui	at,0x1
[ 	]*40c: [A-Z0-9_]*HI[A-Z0-9_]*	.data.*
0+0410 <[^>]*> addu	at,a1,at
0+0414 <[^>]*> lw	a0,0\(at\)
[ 	]*414: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
0+0418 <[^>]*> lw	a1,4\(at\)
[ 	]*418: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
0+041c <[^>]*> lui	at,0x1
[ 	]*41c: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_data_label
0+0420 <[^>]*> addu	at,a1,at
0+0424 <[^>]*> lw	a0,0\(at\)
[ 	]*424: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
0+0428 <[^>]*> lw	a1,4\(at\)
[ 	]*428: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
0+042c <[^>]*> lui	at,0x1
[ 	]*42c: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_data_label
0+0430 <[^>]*> addu	at,a1,at
0+0434 <[^>]*> lw	a0,0\(at\)
[ 	]*434: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
0+0438 <[^>]*> lw	a1,4\(at\)
[ 	]*438: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
0+043c <[^>]*> lui	at,0x1
[ 	]*43c: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
0+0440 <[^>]*> addu	at,a1,at
0+0444 <[^>]*> lw	a0,0\(at\)
[ 	]*444: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
0+0448 <[^>]*> lw	a1,4\(at\)
[ 	]*448: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
0+044c <[^>]*> lui	at,0x1
[ 	]*44c: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_common
0+0450 <[^>]*> addu	at,a1,at
0+0454 <[^>]*> lw	a0,0\(at\)
[ 	]*454: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
0+0458 <[^>]*> lw	a1,4\(at\)
[ 	]*458: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
0+045c <[^>]*> lui	at,0x1
[ 	]*45c: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
0+0460 <[^>]*> addu	at,a1,at
0+0464 <[^>]*> lw	a0,0\(at\)
[ 	]*464: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
0+0468 <[^>]*> lw	a1,4\(at\)
[ 	]*468: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
0+046c <[^>]*> lui	at,0x1
[ 	]*46c: [A-Z0-9_]*HI[A-Z0-9_]*	.sbss.*
0+0470 <[^>]*> addu	at,a1,at
0+0474 <[^>]*> lw	a0,0\(at\)
[ 	]*474: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
0+0478 <[^>]*> lw	a1,4\(at\)
[ 	]*478: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
0+047c <[^>]*> lui	at,0x2
[ 	]*47c: [A-Z0-9_]*HI[A-Z0-9_]*	.data.*
0+0480 <[^>]*> addu	at,a1,at
0+0484 <[^>]*> lw	a0,-23131\(at\)
[ 	]*484: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
0+0488 <[^>]*> lw	a1,-23127\(at\)
[ 	]*488: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
0+048c <[^>]*> lui	at,0x2
[ 	]*48c: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_data_label
0+0490 <[^>]*> addu	at,a1,at
0+0494 <[^>]*> lw	a0,-23131\(at\)
[ 	]*494: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
0+0498 <[^>]*> lw	a1,-23127\(at\)
[ 	]*498: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
0+049c <[^>]*> lui	at,0x2
[ 	]*49c: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_data_label
0+04a0 <[^>]*> addu	at,a1,at
0+04a4 <[^>]*> lw	a0,-23131\(at\)
[ 	]*4a4: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
0+04a8 <[^>]*> lw	a1,-23127\(at\)
[ 	]*4a8: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
0+04ac <[^>]*> lui	at,0x2
[ 	]*4ac: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
0+04b0 <[^>]*> addu	at,a1,at
0+04b4 <[^>]*> lw	a0,-23131\(at\)
[ 	]*4b4: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
0+04b8 <[^>]*> lw	a1,-23127\(at\)
[ 	]*4b8: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
0+04bc <[^>]*> lui	at,0x2
[ 	]*4bc: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_common
0+04c0 <[^>]*> addu	at,a1,at
0+04c4 <[^>]*> lw	a0,-23131\(at\)
[ 	]*4c4: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
0+04c8 <[^>]*> lw	a1,-23127\(at\)
[ 	]*4c8: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
0+04cc <[^>]*> lui	at,0x2
[ 	]*4cc: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
0+04d0 <[^>]*> addu	at,a1,at
0+04d4 <[^>]*> lw	a0,-23131\(at\)
[ 	]*4d4: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
0+04d8 <[^>]*> lw	a1,-23127\(at\)
[ 	]*4d8: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
0+04dc <[^>]*> lui	at,0x2
[ 	]*4dc: [A-Z0-9_]*HI[A-Z0-9_]*	.sbss.*
0+04e0 <[^>]*> addu	at,a1,at
0+04e4 <[^>]*> lw	a0,-23131\(at\)
[ 	]*4e4: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
0+04e8 <[^>]*> lw	a1,-23127\(at\)
[ 	]*4e8: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
0+04ec <[^>]*> lwc1	f[45],0\(zero\)
0+04f0 <[^>]*> lwc1	f[45],4\(zero\)
0+04f4 <[^>]*> lwc1	f[45],1\(zero\)
0+04f8 <[^>]*> lwc1	f[45],5\(zero\)
0+04fc <[^>]*> lui	at,0x1
0+0500 <[^>]*> lwc1	f[45],-32768\(at\)
0+0504 <[^>]*> lwc1	f[45],-32764\(at\)
0+0508 <[^>]*> lwc1	f[45],-32768\(zero\)
0+050c <[^>]*> lwc1	f[45],-32764\(zero\)
0+0510 <[^>]*> lwc1	f[45],0\(a1\)
0+0514 <[^>]*> lwc1	f[45],4\(a1\)
0+0518 <[^>]*> lwc1	f[45],1\(a1\)
0+051c <[^>]*> lwc1	f[45],5\(a1\)
0+0520 <[^>]*> lui	at,0x1
0+0524 <[^>]*> addu	at,a1,at
0+0528 <[^>]*> lwc1	f[45],-32768\(at\)
0+052c <[^>]*> lwc1	f[45],-32764\(at\)
0+0530 <[^>]*> lwc1	f[45],-32768\(a1\)
0+0534 <[^>]*> lwc1	f[45],-32764\(a1\)
0+0538 <[^>]*> lui	at,0x2
[ 	]*538: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_common
0+053c <[^>]*> addu	at,a1,at
0+0540 <[^>]*> lwc1	f[45],-23131\(at\)
[ 	]*540: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
0+0544 <[^>]*> lwc1	f[45],-23127\(at\)
[ 	]*544: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
0+0548 <[^>]*> nop
0+054c <[^>]*> swc1	f[45],0\(zero\)
0+0550 <[^>]*> swc1	f[45],4\(zero\)
0+0554 <[^>]*> swc1	f[45],1\(zero\)
0+0558 <[^>]*> swc1	f[45],5\(zero\)
0+055c <[^>]*> lui	at,0x1
0+0560 <[^>]*> swc1	f[45],-32768\(at\)
0+0564 <[^>]*> swc1	f[45],-32764\(at\)
0+0568 <[^>]*> swc1	f[45],-32768\(zero\)
0+056c <[^>]*> swc1	f[45],-32764\(zero\)
0+0570 <[^>]*> swc1	f[45],0\(a1\)
0+0574 <[^>]*> swc1	f[45],4\(a1\)
0+0578 <[^>]*> swc1	f[45],1\(a1\)
0+057c <[^>]*> swc1	f[45],5\(a1\)
0+0580 <[^>]*> lui	at,0x1
0+0584 <[^>]*> addu	at,a1,at
0+0588 <[^>]*> swc1	f[45],-32768\(at\)
0+058c <[^>]*> swc1	f[45],-32764\(at\)
0+0590 <[^>]*> swc1	f[45],-32768\(a1\)
0+0594 <[^>]*> swc1	f[45],-32764\(a1\)
0+0598 <[^>]*> lui	at,0x2
[ 	]*598: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
0+059c <[^>]*> addu	at,a1,at
0+05a0 <[^>]*> swc1	f[45],-23131\(at\)
[ 	]*5a0: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
0+05a4 <[^>]*> swc1	f[45],-23127\(at\)
[ 	]*5a4: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
0+05a8 <[^>]*> sw	a0,0\(zero\)
0+05ac <[^>]*> sw	a1,4\(zero\)
0+05b0 <[^>]*> lui	a0,0x2
[ 	]*5b0: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
0+05b4 <[^>]*> (d|)addu	a0,a0,a1
0+05b8 <[^>]*> ld	a0,-23131\(a0\)
[ 	]*5b8: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
0+05bc <[^>]*> lui	at,0x2
[ 	]*5bc: [A-Z0-9_]*HI[A-Z0-9_]*	.sbss.*
0+05c0 <[^>]*> (d|)addu	at,at,a1
0+05c4 <[^>]*> sd	a0,-23131\(at\)
[ 	]*5c4: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
0+05c8 <[^>]*> nop

