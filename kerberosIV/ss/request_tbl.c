/*	$Id: request_tbl.c,v 1.1.1.1 1995/12/14 06:52:47 tholo Exp $	*/

/*-
 * Copyright 1987, 1988 by the Student Information Processing Board
 *	of the Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is
 * hereby granted, provided that the above copyright notice
 * appear in all copies and that both that copyright notice and
 * this permission notice appear in supporting documentation,
 * and that the names of M.I.T. and the M.I.T. S.I.P.B. not be
 * used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.
 * M.I.T. and the M.I.T. S.I.P.B. make no representations about
 * the suitability of this software for any purpose.  It is
 * provided "as is" without express or implied warranty.
 */

#include <stdlib.h>
#include <errno.h>

#include "ss_internal.h"

#define ssrt ss_request_table	/* for some readable code... */

void
ss_add_request_table(sci_idx, rqtbl_ptr, position, code_ptr)
	int sci_idx;
	ssrt *rqtbl_ptr;
	int position;		/* 1 -> becomes second... */
	int *code_ptr;
{
	register ss_data *info;
	register int i, size;

	info = ss_info(sci_idx);
	for (size=0; info->rqt_tables[size] != (ssrt *)NULL; size++)
		;
	/* size == C subscript of NULL == #elements */
	size += 2;		/* new element, and NULL */
	info->rqt_tables = (ssrt **)realloc((char *)info->rqt_tables,
					    (unsigned)size*sizeof(ssrt));
	if (info->rqt_tables == (ssrt **)NULL) {
		*code_ptr = errno;
		return;
	}
	if (position > size - 2)
		position = size - 2;

	if (size > 1)
		for (i = size - 2; i >= position; i--)
			info->rqt_tables[i+1] = info->rqt_tables[i];

	info->rqt_tables[position] = rqtbl_ptr;
	info->rqt_tables[size-1] = (ssrt *)NULL;
	*code_ptr = 0;
}

void
ss_delete_request_table(sci_idx, rqtbl_ptr, code_ptr)
     int sci_idx;
     ssrt *rqtbl_ptr;
     int *code_ptr;
{
     register ss_data *info;
     register ssrt **rt1, **rt2;

     *code_ptr = SS_ET_TABLE_NOT_FOUND;
     info = ss_info(sci_idx);
     rt1 = info->rqt_tables;
     for (rt2 = rt1; *rt1; rt1++) {
	  if (*rt1 != rqtbl_ptr) {
	       *rt2++ = *rt1;
	       *code_ptr = 0;
	  }
     }
     *rt2 = (ssrt *)NULL;
     return;
}
