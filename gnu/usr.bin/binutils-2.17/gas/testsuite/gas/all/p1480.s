start:	.long 0, 1, 2, 3, 4, 5, 6, 7
	.space 0x80 - (. - start)
foo:	.long 42
