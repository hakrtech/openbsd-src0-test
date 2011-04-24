# source file to test objdump's disassembly using various styles of
# CP0 (w/ non-zero select code) register names.

	.set noreorder
	.set noat

	.globl text_label .text
text_label:

	mtc0	$0, $0, 1
	mtc0	$0, $0, 2
	mtc0	$0, $0, 3
	mtc0	$0, $0, 4
	mtc0	$0, $0, 5
	mtc0	$0, $0, 6
	mtc0	$0, $0, 7
	mtc0	$0, $1, 1
	mtc0	$0, $1, 2
	mtc0	$0, $1, 3
	mtc0	$0, $1, 4
	mtc0	$0, $1, 5
	mtc0	$0, $1, 6
	mtc0	$0, $1, 7
	mtc0	$0, $2, 1
	mtc0	$0, $2, 2
	mtc0	$0, $2, 3
	mtc0	$0, $2, 4
	mtc0	$0, $2, 5
	mtc0	$0, $2, 6
	mtc0	$0, $2, 7
	mtc0	$0, $3, 1
	mtc0	$0, $3, 2
	mtc0	$0, $3, 3
	mtc0	$0, $3, 4
	mtc0	$0, $3, 5
	mtc0	$0, $3, 6
	mtc0	$0, $3, 7
	mtc0	$0, $4, 1
	mtc0	$0, $4, 2
	mtc0	$0, $4, 3
	mtc0	$0, $4, 4
	mtc0	$0, $4, 5
	mtc0	$0, $4, 6
	mtc0	$0, $4, 7
	mtc0	$0, $5, 1
	mtc0	$0, $5, 2
	mtc0	$0, $5, 3
	mtc0	$0, $5, 4
	mtc0	$0, $5, 5
	mtc0	$0, $5, 6
	mtc0	$0, $5, 7
	mtc0	$0, $6, 1
	mtc0	$0, $6, 2
	mtc0	$0, $6, 3
	mtc0	$0, $6, 4
	mtc0	$0, $6, 5
	mtc0	$0, $6, 6
	mtc0	$0, $6, 7
	mtc0	$0, $7, 1
	mtc0	$0, $7, 2
	mtc0	$0, $7, 3
	mtc0	$0, $7, 4
	mtc0	$0, $7, 5
	mtc0	$0, $7, 6
	mtc0	$0, $7, 7
	mtc0	$0, $8, 1
	mtc0	$0, $8, 2
	mtc0	$0, $8, 3
	mtc0	$0, $8, 4
	mtc0	$0, $8, 5
	mtc0	$0, $8, 6
	mtc0	$0, $8, 7
	mtc0	$0, $9, 1
	mtc0	$0, $9, 2
	mtc0	$0, $9, 3
	mtc0	$0, $9, 4
	mtc0	$0, $9, 5
	mtc0	$0, $9, 6
	mtc0	$0, $9, 7
	mtc0	$0, $10, 1
	mtc0	$0, $10, 2
	mtc0	$0, $10, 3
	mtc0	$0, $10, 4
	mtc0	$0, $10, 5
	mtc0	$0, $10, 6
	mtc0	$0, $10, 7
	mtc0	$0, $11, 1
	mtc0	$0, $11, 2
	mtc0	$0, $11, 3
	mtc0	$0, $11, 4
	mtc0	$0, $11, 5
	mtc0	$0, $11, 6
	mtc0	$0, $11, 7
	mtc0	$0, $12, 1
	mtc0	$0, $12, 2
	mtc0	$0, $12, 3
	mtc0	$0, $12, 4
	mtc0	$0, $12, 5
	mtc0	$0, $12, 6
	mtc0	$0, $12, 7
	mtc0	$0, $13, 1
	mtc0	$0, $13, 2
	mtc0	$0, $13, 3
	mtc0	$0, $13, 4
	mtc0	$0, $13, 5
	mtc0	$0, $13, 6
	mtc0	$0, $13, 7
	mtc0	$0, $14, 1
	mtc0	$0, $14, 2
	mtc0	$0, $14, 3
	mtc0	$0, $14, 4
	mtc0	$0, $14, 5
	mtc0	$0, $14, 6
	mtc0	$0, $14, 7
	mtc0	$0, $15, 1
	mtc0	$0, $15, 2
	mtc0	$0, $15, 3
	mtc0	$0, $15, 4
	mtc0	$0, $15, 5
	mtc0	$0, $15, 6
	mtc0	$0, $15, 7
	mtc0	$0, $16, 1
	mtc0	$0, $16, 2
	mtc0	$0, $16, 3
	mtc0	$0, $16, 4
	mtc0	$0, $16, 5
	mtc0	$0, $16, 6
	mtc0	$0, $16, 7
	mtc0	$0, $17, 1
	mtc0	$0, $17, 2
	mtc0	$0, $17, 3
	mtc0	$0, $17, 4
	mtc0	$0, $17, 5
	mtc0	$0, $17, 6
	mtc0	$0, $17, 7
	mtc0	$0, $18, 1
	mtc0	$0, $18, 2
	mtc0	$0, $18, 3
	mtc0	$0, $18, 4
	mtc0	$0, $18, 5
	mtc0	$0, $18, 6
	mtc0	$0, $18, 7
	mtc0	$0, $19, 1
	mtc0	$0, $19, 2
	mtc0	$0, $19, 3
	mtc0	$0, $19, 4
	mtc0	$0, $19, 5
	mtc0	$0, $19, 6
	mtc0	$0, $19, 7
	mtc0	$0, $20, 1
	mtc0	$0, $20, 2
	mtc0	$0, $20, 3
	mtc0	$0, $20, 4
	mtc0	$0, $20, 5
	mtc0	$0, $20, 6
	mtc0	$0, $20, 7
	mtc0	$0, $21, 1
	mtc0	$0, $21, 2
	mtc0	$0, $21, 3
	mtc0	$0, $21, 4
	mtc0	$0, $21, 5
	mtc0	$0, $21, 6
	mtc0	$0, $21, 7
	mtc0	$0, $22, 1
	mtc0	$0, $22, 2
	mtc0	$0, $22, 3
	mtc0	$0, $22, 4
	mtc0	$0, $22, 5
	mtc0	$0, $22, 6
	mtc0	$0, $22, 7
	mtc0	$0, $23, 1
	mtc0	$0, $23, 2
	mtc0	$0, $23, 3
	mtc0	$0, $23, 4
	mtc0	$0, $23, 5
	mtc0	$0, $23, 6
	mtc0	$0, $23, 7
	mtc0	$0, $24, 1
	mtc0	$0, $24, 2
	mtc0	$0, $24, 3
	mtc0	$0, $24, 4
	mtc0	$0, $24, 5
	mtc0	$0, $24, 6
	mtc0	$0, $24, 7
	mtc0	$0, $25, 1
	mtc0	$0, $25, 2
	mtc0	$0, $25, 3
	mtc0	$0, $25, 4
	mtc0	$0, $25, 5
	mtc0	$0, $25, 6
	mtc0	$0, $25, 7
	mtc0	$0, $26, 1
	mtc0	$0, $26, 2
	mtc0	$0, $26, 3
	mtc0	$0, $26, 4
	mtc0	$0, $26, 5
	mtc0	$0, $26, 6
	mtc0	$0, $26, 7
	mtc0	$0, $27, 1
	mtc0	$0, $27, 2
	mtc0	$0, $27, 3
	mtc0	$0, $27, 4
	mtc0	$0, $27, 5
	mtc0	$0, $27, 6
	mtc0	$0, $27, 7
	mtc0	$0, $28, 1
	mtc0	$0, $28, 2
	mtc0	$0, $28, 3
	mtc0	$0, $28, 4
	mtc0	$0, $28, 5
	mtc0	$0, $28, 6
	mtc0	$0, $28, 7
	mtc0	$0, $29, 1
	mtc0	$0, $29, 2
	mtc0	$0, $29, 3
	mtc0	$0, $29, 4
	mtc0	$0, $29, 5
	mtc0	$0, $29, 6
	mtc0	$0, $29, 7
	mtc0	$0, $30, 1
	mtc0	$0, $30, 2
	mtc0	$0, $30, 3
	mtc0	$0, $30, 4
	mtc0	$0, $30, 5
	mtc0	$0, $30, 6
	mtc0	$0, $30, 7
	mtc0	$0, $31, 1
	mtc0	$0, $31, 2
	mtc0	$0, $31, 3
	mtc0	$0, $31, 4
	mtc0	$0, $31, 5
	mtc0	$0, $31, 6
	mtc0	$0, $31, 7

# Force at least 8 (non-delay-slot) zero bytes, to make 'objdump' print ...
      .space  8
