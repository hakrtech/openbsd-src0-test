/*	$NetBSD: db_sym.h,v 1.13 2000/05/25 19:57:36 jhawk Exp $	*/

/*
 * Mach Operating System
 * Copyright (c) 1993,1992,1991,1990 Carnegie Mellon University
 * All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 * Carnegie Mellon requests users of this software to return to
 *
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 *
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 *
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	8/90
 */

#ifndef _DDB_DB_SYM_H_
#define _DDB_DB_SYM_H_

#include <sys/stdint.h>

/*
 * Symbol representation is specific to the symtab style:
 * BSD compilers use dbx' nlist, other compilers might use
 * a different one
 */
typedef	char *		db_sym_t;	/* opaque handle on symbols */

/*
 * Non-stripped symbol tables will have duplicates, for instance
 * the same string could match a parameter name, a local var, a
 * global var, etc.
 * We are most concern with the following matches.
 */
typedef int		db_strategy_t;	/* search strategy */

#define	DB_STGY_ANY	0			/* anything goes */
#define DB_STGY_XTRN	1			/* only external symbols */
#define DB_STGY_PROC	2			/* only procedures */


/*
 * Internal db_forall function calling convention:
 *
 * (*db_forall_func)(stab, sym, name, suffix, prefix, arg);
 *
 * stab is the symbol table, symbol the (opaque) symbol pointer,
 * name the name of the symbol, suffix a string representing
 * the type, prefix an initial ignorable function prefix (e.g. "_"
 * in a.out), and arg an opaque argument to be passed in.
 */
typedef void (db_forall_func_t)(db_sym_t, char *, char *, int, void *);

extern unsigned int db_maxoff;		/* like gdb's "max-symbolic-offset" */

int db_eqname(char *, char *, int);
					/* strcmp, modulo leading char */

int db_value_of_name(char *, db_expr_t *);
					/* find symbol value given name */

db_sym_t db_search_symbol(db_addr_t, db_strategy_t, db_expr_t *);
					/* find symbol given value */

void db_symbol_values(db_sym_t, char **, db_expr_t *);
					/* return name and value of symbol */

#define db_find_sym_and_offset(val,namep,offp)	\
	db_symbol_values(db_search_symbol(val,DB_STGY_ANY,offp),namep,0)
					/* find name&value given approx val */

#define db_find_xtrn_sym_and_offset(val,namep,offp)	\
	db_symbol_values(db_search_symbol(val,DB_STGY_XTRN,offp),namep,0)
					/* ditto, but no locals */

void db_printsym(db_expr_t, db_strategy_t, int (*)(const char *, ...));
					/* print closest symbol to a value */

int db_elf_sym_init(int, void *, void *, const char *);
db_sym_t db_elf_sym_search(db_addr_t, db_strategy_t, db_expr_t *);
int db_elf_line_at_pc(db_sym_t, char **, int *, db_expr_t);
void db_elf_sym_forall(db_forall_func_t db_forall_func, void *);

bool db_dwarf_line_at_pc(const char *, size_t, uintptr_t,
    const char **, const char **, int *);

#ifdef DDBCTF
int	db_ctf_func_numargs(db_sym_t);
#endif

#endif /* _DDB_DB_SYM_H_ */
