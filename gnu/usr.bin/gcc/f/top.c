/* top.c -- Implementation File (module.c template V1.0)
   Copyright (C) 1995 Free Software Foundation, Inc.
   Contributed by James Craig Burley (burley@gnu.ai.mit.edu).

This file is part of GNU Fortran.

GNU Fortran is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Fortran is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Fortran; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.

   Related Modules:
      None.

   Description:
      The GNU Fortran Front End.

   Modifications:
*/

/* Include files. */

#include <ctype.h>
#include "proj.h"
#include "top.h"
#include "bad.h"
#include "bit.h"
#include "bld.h"
#include "com.h"
#include "data.h"
#include "equiv.h"
#include "expr.h"
#include "global.h"
#include "implic.h"
#include "info.h"
#include "intrin.h"
#include "lab.h"
#include "lex.h"
#include "malloc.h"
#include "name.h"
#include "src.h"
#include "st.h"
#include "storag.h"
#include "symbol.h"
#include "target.h"
#include "where.h"
#if FFECOM_targetCURRENT == FFECOM_targetGCC
#include "flags.j"
#endif

/* Externals defined here. */

int flag_traditional;		/* Shouldn't need this (C front end only)! */
bool ffe_is_do_internal_checks_ = TRUE;
bool ffe_is_90_ = FFETARGET_defaultIS_90;
bool ffe_is_automatic_ = FFETARGET_defaultIS_AUTOMATIC;
bool ffe_is_backslash_ = FFETARGET_defaultIS_BACKSLASH;
bool ffe_is_underscoring_ = FFETARGET_defaultEXTERNAL_UNDERSCORED
  || FFETARGET_defaultUNDERSCORED_EXTERNAL_UNDERSCORED;
bool ffe_is_second_underscore_ = FFETARGET_defaultUNDERSCORED_EXTERNAL_UNDERSCORED;
bool ffe_is_dollar_ok_ = FFETARGET_defaultIS_DOLLAR_OK;
bool ffe_is_f2c_ = FFETARGET_defaultIS_F2C;
bool ffe_is_f2c_library_ = FFETARGET_defaultIS_F2C_LIBRARY;
bool ffe_is_ffedebug_ = FALSE;
bool ffe_is_free_form_ = FFETARGET_defaultIS_FREE_FORM;
bool ffe_is_ident_ = TRUE;
bool ffe_is_init_local_zero_ = FFETARGET_defaultIS_INIT_LOCAL_ZERO;
bool ffe_is_mainprog_;		/* TRUE if current prog unit known to be
				   main. */
bool ffe_is_pedantic_ = FFETARGET_defaultIS_PEDANTIC;
bool ffe_is_saveall_;		/* TRUE if mainprog or SAVE (no args) seen. */
bool ffe_is_ugly_ = FFETARGET_defaultIS_UGLY;
bool ffe_is_ugly_args_ = FFETARGET_defaultIS_UGLY_ARGS;
bool ffe_is_ugly_init_ = FFETARGET_defaultIS_UGLY_INIT;
bool ffe_is_version_ = FALSE;
bool ffe_is_vxt_not_90_ = FFETARGET_defaultIS_VXT_NOT_90;
bool ffe_is_warn_implicit_ = FALSE;
bool ffe_is_warn_surprising_ = FALSE;
bool ffe_is_zeros_ = FALSE;
ffeCase ffe_case_intrin_ = FFETARGET_defaultCASE_INTRIN;
ffeCase ffe_case_match_ = FFETARGET_defaultCASE_MATCH;
ffeCase ffe_case_source_ = FFETARGET_defaultCASE_SOURCE;
ffeCase ffe_case_symbol_ = FFETARGET_defaultCASE_SYMBOL;
ffeIntrinsicState ffe_intrinsic_state_dcp_ = FFETARGET_defaultSTATE_DCP;
ffeIntrinsicState ffe_intrinsic_state_f2c_ = FFETARGET_defaultSTATE_F2C;
ffeIntrinsicState ffe_intrinsic_state_f90_ = FFETARGET_defaultSTATE_F90;
ffeIntrinsicState ffe_intrinsic_state_mil_ = FFETARGET_defaultSTATE_MIL;
ffeIntrinsicState ffe_intrinsic_state_unix_ = FFETARGET_defaultSTATE_UNIX;
ffeIntrinsicState ffe_intrinsic_state_vxt_ = FFETARGET_defaultSTATE_VXT;
int ffe_fixed_line_length_ = FFETARGET_defaultFIXED_LINE_LENGTH;
mallocPool ffe_file_pool_ = NULL;
mallocPool ffe_any_unit_pool_ = NULL;
mallocPool ffe_program_unit_pool_ = NULL;
ffeCounter ffe_count_0 = 0;
ffeCounter ffe_count_1 = 0;
ffeCounter ffe_count_2 = 0;
ffeCounter ffe_count_3 = 0;
ffeCounter ffe_count_4 = 0;
bool ffe_in_0 = FALSE;
bool ffe_in_1 = FALSE;
bool ffe_in_2 = FALSE;
bool ffe_in_3 = FALSE;
bool ffe_in_4 = FALSE;

/* Simple definitions and enumerations. */


/* Internal typedefs. */


/* Private include files. */


/* Internal structure definitions. */


/* Static objects accessed by functions in this module. */


/* Static functions (internal). */

static bool ffe_is_digit_string_ (char *s);

/* Internal macros. */

static bool
ffe_is_digit_string_ (char *s)
{
  char *p;

  for (p = s; isdigit (*p); ++p)
    ;

  return (p != s) && (*p == '\0');
}

/* Handle command-line options.	 Returns 0 if unrecognized, 1 if
   recognized and handled.  */

int
ffe_decode_option (char *opt)
{
  if (opt[0] != '-')
    return 0;
  if (opt[1] == 'f')
    {
      if (strcmp (&opt[2], "version") == 0)
	ffe_set_is_version (TRUE);
      else if (strcmp (&opt[2], "null-version") == 0)
	;			/* Someday generate program to print version
				   info.  */
      else if (strcmp (&opt[2], "set-g77-defaults") == 0)
	{
	  ffe_is_do_internal_checks_ = 0;
	  flag_move_all_movables = 1;
	  flag_reduce_all_givs = 1;
	  flag_rerun_loop_opt = 1;
	}
      else if (strcmp (&opt[2], "ident") == 0)
	ffe_set_is_ident (TRUE);
      else if (strcmp (&opt[2], "no-ident") == 0)
	ffe_set_is_ident (FALSE);
      else if (strcmp (&opt[2], "f90") == 0)
	ffe_set_is_90 (TRUE);
      else if (strcmp (&opt[2], "no-f90") == 0)
	ffe_set_is_90 (FALSE);
      else if (strcmp (&opt[2], "automatic") == 0)
	ffe_set_is_automatic (TRUE);
      else if (strcmp (&opt[2], "no-automatic") == 0)
	ffe_set_is_automatic (FALSE);
      else if (strcmp (&opt[2], "dollar-ok") == 0)
	ffe_set_is_dollar_ok (TRUE);
      else if (strcmp (&opt[2], "no-dollar-ok") == 0)
	ffe_set_is_dollar_ok (FALSE);
      else if (strcmp (&opt[2], "f2c") == 0)
	ffe_set_is_f2c (TRUE);
      else if (strcmp (&opt[2], "no-f2c") == 0)
	ffe_set_is_f2c (FALSE);
      else if (strcmp (&opt[2], "f2c-library") == 0)
	ffe_set_is_f2c_library (TRUE);
      else if (strcmp (&opt[2], "no-f2c-library") == 0)
	ffe_set_is_f2c_library (FALSE);
      else if (strcmp (&opt[2], "free-form") == 0)
	ffe_set_is_free_form (TRUE);
      else if (strcmp (&opt[2], "no-free-form") == 0)
	ffe_set_is_free_form (FALSE);
      else if (strcmp (&opt[2], "fixed-form") == 0)
	ffe_set_is_free_form (FALSE);
      else if (strcmp (&opt[2], "no-fixed-form") == 0)
	ffe_set_is_free_form (TRUE);
      else if (strcmp (&opt[2], "pedantic") == 0)
	ffe_set_is_pedantic (TRUE);
      else if (strcmp (&opt[2], "no-pedantic") == 0)
	ffe_set_is_pedantic (FALSE);
      else if (strcmp (&opt[2], "vxt-not-f90") == 0)
	ffe_set_is_vxt_not_90 (TRUE);
      else if (strcmp (&opt[2], "f90-not-vxt") == 0)
	ffe_set_is_vxt_not_90 (FALSE);
      else if (strcmp (&opt[2], "ugly") == 0)
	{
	  ffe_set_is_ugly (TRUE);
	  ffe_set_is_ugly_args (TRUE);
	  ffe_set_is_ugly_init (TRUE);
	}
      else if (strcmp (&opt[2], "no-ugly") == 0)
	{
	  ffe_set_is_ugly (FALSE);
	  ffe_set_is_ugly_args (FALSE);
	  ffe_set_is_ugly_init (FALSE);
	}
      else if (strcmp (&opt[2], "ugly-args") == 0)
	ffe_set_is_ugly_args (TRUE);
      else if (strcmp (&opt[2], "no-ugly-args") == 0)
	ffe_set_is_ugly_args (FALSE);
      else if (strcmp (&opt[2], "ugly-init") == 0)
	ffe_set_is_ugly_init (TRUE);
      else if (strcmp (&opt[2], "no-ugly-init") == 0)
	ffe_set_is_ugly_init (FALSE);
      else if (strcmp (&opt[2], "debug") == 0)
	ffe_set_is_ffedebug (TRUE);
      else if (strcmp (&opt[2], "no-debug") == 0)
	ffe_set_is_ffedebug (FALSE);
      else if (strcmp (&opt[2], "init-local-zero") == 0)
	ffe_set_is_init_local_zero (TRUE);
      else if (strcmp (&opt[2], "no-init-local-zero") == 0)
	ffe_set_is_init_local_zero (FALSE);
      else if (strcmp (&opt[2], "backslash") == 0)
	ffe_set_is_backslash (TRUE);
      else if (strcmp (&opt[2], "no-backslash") == 0)
	ffe_set_is_backslash (FALSE);
      else if (strcmp (&opt[2], "underscoring") == 0)
	ffe_set_is_underscoring (TRUE);
      else if (strcmp (&opt[2], "no-underscoring") == 0)
	ffe_set_is_underscoring (FALSE);
      else if (strcmp (&opt[2], "second-underscore") == 0)
	ffe_set_is_second_underscore (TRUE);
      else if (strcmp (&opt[2], "no-second-underscore") == 0)
	ffe_set_is_second_underscore (FALSE);
      else if (strcmp (&opt[2], "intrin-case-initcap") == 0)
	ffe_set_case_intrin (FFE_caseINITCAP);
      else if (strcmp (&opt[2], "intrin-case-upper") == 0)
	ffe_set_case_intrin (FFE_caseUPPER);
      else if (strcmp (&opt[2], "intrin-case-lower") == 0)
	ffe_set_case_intrin (FFE_caseLOWER);
      else if (strcmp (&opt[2], "intrin-case-any") == 0)
	ffe_set_case_intrin (FFE_caseNONE);
      else if (strcmp (&opt[2], "match-case-initcap") == 0)
	ffe_set_case_match (FFE_caseINITCAP);
      else if (strcmp (&opt[2], "match-case-upper") == 0)
	ffe_set_case_match (FFE_caseUPPER);
      else if (strcmp (&opt[2], "match-case-lower") == 0)
	ffe_set_case_match (FFE_caseLOWER);
      else if (strcmp (&opt[2], "match-case-any") == 0)
	ffe_set_case_match (FFE_caseNONE);
      else if (strcmp (&opt[2], "source-case-upper") == 0)
	ffe_set_case_source (FFE_caseUPPER);
      else if (strcmp (&opt[2], "source-case-lower") == 0)
	ffe_set_case_source (FFE_caseLOWER);
      else if (strcmp (&opt[2], "source-case-preserve") == 0)
	ffe_set_case_source (FFE_caseNONE);
      else if (strcmp (&opt[2], "symbol-case-initcap") == 0)
	ffe_set_case_symbol (FFE_caseINITCAP);
      else if (strcmp (&opt[2], "symbol-case-upper") == 0)
	ffe_set_case_symbol (FFE_caseUPPER);
      else if (strcmp (&opt[2], "symbol-case-lower") == 0)
	ffe_set_case_symbol (FFE_caseLOWER);
      else if (strcmp (&opt[2], "symbol-case-any") == 0)
	ffe_set_case_symbol (FFE_caseNONE);
      else if (strcmp (&opt[2], "case-strict-upper") == 0)
	{
	  ffe_set_case_intrin (FFE_caseUPPER);
	  ffe_set_case_match (FFE_caseUPPER);
	  ffe_set_case_source (FFE_caseNONE);
	  ffe_set_case_symbol (FFE_caseUPPER);
	}
      else if (strcmp (&opt[2], "case-strict-lower") == 0)
	{
	  ffe_set_case_intrin (FFE_caseLOWER);
	  ffe_set_case_match (FFE_caseLOWER);
	  ffe_set_case_source (FFE_caseNONE);
	  ffe_set_case_symbol (FFE_caseLOWER);
	}
      else if (strcmp (&opt[2], "case-initcap") == 0)
	{
	  ffe_set_case_intrin (FFE_caseINITCAP);
	  ffe_set_case_match (FFE_caseINITCAP);
	  ffe_set_case_source (FFE_caseNONE);
	  ffe_set_case_symbol (FFE_caseINITCAP);
	}
      else if (strcmp (&opt[2], "case-upper") == 0)
	{
	  ffe_set_case_intrin (FFE_caseNONE);
	  ffe_set_case_match (FFE_caseNONE);
	  ffe_set_case_source (FFE_caseUPPER);
	  ffe_set_case_symbol (FFE_caseNONE);
	}
      else if (strcmp (&opt[2], "case-lower") == 0)
	{
	  ffe_set_case_intrin (FFE_caseNONE);
	  ffe_set_case_match (FFE_caseNONE);
	  ffe_set_case_source (FFE_caseLOWER);
	  ffe_set_case_symbol (FFE_caseNONE);
	}
      else if (strcmp (&opt[2], "case-preserve") == 0)
	{
	  ffe_set_case_intrin (FFE_caseNONE);
	  ffe_set_case_match (FFE_caseNONE);
	  ffe_set_case_source (FFE_caseNONE);
	  ffe_set_case_symbol (FFE_caseNONE);
	}
      else if (strcmp (&opt[2], "dcp-intrinsics-delete") == 0)
	ffe_set_intrinsic_state_dcp (FFE_intrinsicstateDELETED);
      else if (strcmp (&opt[2], "dcp-intrinsics-hide") == 0)
	ffe_set_intrinsic_state_dcp (FFE_intrinsicstateHIDDEN);
      else if (strcmp (&opt[2], "dcp-intrinsics-disable") == 0)
	ffe_set_intrinsic_state_dcp (FFE_intrinsicstateDISABLED);
      else if (strcmp (&opt[2], "dcp-intrinsics-enable") == 0)
	ffe_set_intrinsic_state_dcp (FFE_intrinsicstateENABLED);
      else if (strcmp (&opt[2], "f2c-intrinsics-delete") == 0)
	ffe_set_intrinsic_state_f2c (FFE_intrinsicstateDELETED);
      else if (strcmp (&opt[2], "f2c-intrinsics-hide") == 0)
	ffe_set_intrinsic_state_f2c (FFE_intrinsicstateHIDDEN);
      else if (strcmp (&opt[2], "f2c-intrinsics-disable") == 0)
	ffe_set_intrinsic_state_f2c (FFE_intrinsicstateDISABLED);
      else if (strcmp (&opt[2], "f2c-intrinsics-enable") == 0)
	ffe_set_intrinsic_state_f2c (FFE_intrinsicstateENABLED);
      else if (strcmp (&opt[2], "f90-intrinsics-delete") == 0)
	ffe_set_intrinsic_state_f90 (FFE_intrinsicstateDELETED);
      else if (strcmp (&opt[2], "f90-intrinsics-hide") == 0)
	ffe_set_intrinsic_state_f90 (FFE_intrinsicstateHIDDEN);
      else if (strcmp (&opt[2], "f90-intrinsics-disable") == 0)
	ffe_set_intrinsic_state_f90 (FFE_intrinsicstateDISABLED);
      else if (strcmp (&opt[2], "f90-intrinsics-enable") == 0)
	ffe_set_intrinsic_state_f90 (FFE_intrinsicstateENABLED);
      else if (strcmp (&opt[2], "mil-intrinsics-delete") == 0)
	ffe_set_intrinsic_state_mil (FFE_intrinsicstateDELETED);
      else if (strcmp (&opt[2], "mil-intrinsics-hide") == 0)
	ffe_set_intrinsic_state_mil (FFE_intrinsicstateHIDDEN);
      else if (strcmp (&opt[2], "mil-intrinsics-disable") == 0)
	ffe_set_intrinsic_state_mil (FFE_intrinsicstateDISABLED);
      else if (strcmp (&opt[2], "mil-intrinsics-enable") == 0)
	ffe_set_intrinsic_state_mil (FFE_intrinsicstateENABLED);
      else if (strcmp (&opt[2], "unix-intrinsics-delete") == 0)
	ffe_set_intrinsic_state_unix (FFE_intrinsicstateDELETED);
      else if (strcmp (&opt[2], "unix-intrinsics-hide") == 0)
	ffe_set_intrinsic_state_unix (FFE_intrinsicstateHIDDEN);
      else if (strcmp (&opt[2], "unix-intrinsics-disable") == 0)
	ffe_set_intrinsic_state_unix (FFE_intrinsicstateDISABLED);
      else if (strcmp (&opt[2], "unix-intrinsics-enable") == 0)
	ffe_set_intrinsic_state_unix (FFE_intrinsicstateENABLED);
      else if (strcmp (&opt[2], "vxt-intrinsics-delete") == 0)
	ffe_set_intrinsic_state_vxt (FFE_intrinsicstateDELETED);
      else if (strcmp (&opt[2], "vxt-intrinsics-hide") == 0)
	ffe_set_intrinsic_state_vxt (FFE_intrinsicstateHIDDEN);
      else if (strcmp (&opt[2], "vxt-intrinsics-disable") == 0)
	ffe_set_intrinsic_state_vxt (FFE_intrinsicstateDISABLED);
      else if (strcmp (&opt[2], "vxt-intrinsics-enable") == 0)
	ffe_set_intrinsic_state_vxt (FFE_intrinsicstateENABLED);
      else if (strcmp (&opt[2], "zeros") == 0)
	ffe_set_is_zeros (TRUE);
      else if (strcmp (&opt[2], "no-zeros") == 0)
	ffe_set_is_zeros (FALSE);
      else if (strncmp (&opt[2], "fixed-line-length-",
			strlen ("fixed-line-length-")) == 0)
	{
	  char *len = &opt[2] + strlen ("fixed-line-length-");

	  if (strcmp (len, "none") == 0)
	    ffe_set_fixed_line_length (0);
	  else if (ffe_is_digit_string_ (len))
	    ffe_set_fixed_line_length (atol (len));
	  else
	    return 0;
	}
      else
	return 0;
    }
  else if (opt[1] == 'W')
    {
      if (!strcmp (&opt[2], "comment"))
	; /* cpp handles this one.  */
      else if (!strcmp (&opt[2], "no-comment"))
	; /* cpp handles this one.  */
      else if (!strcmp (&opt[2], "comments"))
	; /* cpp handles this one.  */
      else if (!strcmp (&opt[2], "no-comments"))
	; /* cpp handles this one.  */
      else if (!strcmp (&opt[2], "trigraphs"))
	; /* cpp handles this one.  */
      else if (!strcmp (&opt[2], "no-trigraphs"))
	; /* cpp handles this one.  */
      else if (!strcmp (&opt[2], "import"))
	; /* cpp handles this one.  */
      else if (!strcmp (&opt[2], "no-import"))
	; /* cpp handles this one.  */
      else if (!strcmp (&opt[2], "implicit"))
	ffe_set_is_warn_implicit (TRUE);
      else if (!strcmp (&opt[2], "no-implicit"))
	ffe_set_is_warn_implicit (FALSE);
      else if (!strcmp (&opt[2], "surprising"))
	ffe_set_is_warn_surprising (TRUE);
      else if (!strcmp (&opt[2], "no-surprising"))
	ffe_set_is_warn_surprising (FALSE);
      else if (!strcmp (&opt[2], "all"))
	{
	  /* We save the value of warn_uninitialized, since if they put
	     -Wuninitialized on the command line, we need to generate a
	     warning about not using it without also specifying -O.  */
	  if (warn_uninitialized != 1)
	    warn_uninitialized = 2;
	  warn_unused = 1;
	  ffe_set_is_warn_surprising (TRUE);
	}
      else
	return 0;
    }
  else if (opt[1] == 'I')
    return ffecom_decode_include_option (&opt[2]);
  else
    return 0;

  return 1;
}

/* Run the FFE on a source file (not an INCLUDEd file).

   Runs the whole shebang.

   Prepare and invoke the appropriate lexer.  */

void
ffe_file (ffewhereFile wf, FILE *f)
{
  ffe_init_1 ();
  ffelex_set_handler ((ffelexHandler) ffest_first);
  ffewhere_file_set (wf, TRUE, 0);
  if (ffe_is_free_form_)
    ffelex_file_free (wf, f);
  else
    ffelex_file_fixed (wf, f);
  ffest_eof ();
  ffe_terminate_1 ();
}

/* ffe_init_0 -- Initialize the FFE per image invocation

   ffe_init_0();

   Performs per-image invocation.  */

void
ffe_init_0 ()
{
  ++ffe_count_0;
  ffe_in_0 = TRUE;

  ffebad_init_0 ();
  ffebit_init_0 ();
  ffebld_init_0 ();
  ffecom_init_0 ();
  ffedata_init_0 ();
  ffeequiv_init_0 ();
  ffeexpr_init_0 ();
  ffeglobal_init_0 ();
  ffeimplic_init_0 ();
  ffeinfo_init_0 ();
  ffeintrin_init_0 ();
  ffelab_init_0 ();
  ffelex_init_0 ();
  ffename_init_0 ();
  ffesrc_init_0 ();
  ffest_init_0 ();
  ffestorag_init_0 ();
  ffesymbol_init_0 ();
  ffetarget_init_0 ();
  ffetype_init_0 ();
  ffewhere_init_0 ();
}

/* ffe_init_1 -- Initialize the FFE per source file

   ffe_init_1();

   Performs per-source-file invocation (not including INCLUDEd files).	*/

void
ffe_init_1 ()
{
  ++ffe_count_1;
  ffe_in_1 = TRUE;

  assert (ffe_file_pool_ == NULL);
  ffe_file_pool_ = malloc_pool_new ("File", malloc_pool_image (), 1024);

  ffebad_init_1 ();
  ffebit_init_1 ();
  ffebld_init_1 ();
  ffecom_init_1 ();
  ffedata_init_1 ();
  ffeequiv_init_1 ();
  ffeexpr_init_1 ();
  ffeglobal_init_1 ();
  ffeimplic_init_1 ();
  ffeinfo_init_1 ();
  ffeintrin_init_1 ();
  ffelab_init_1 ();
  ffelex_init_1 ();
  ffename_init_1 ();
  ffesrc_init_1 ();
  ffest_init_1 ();
  ffestorag_init_1 ();
  ffesymbol_init_1 ();
  ffetarget_init_1 ();
  ffetype_init_1 ();
  ffewhere_init_1 ();

  ffe_init_2 ();
}

/* ffe_init_2 -- Initialize the FFE per outer program unit

   ffe_init_2();

   Performs per-program-unit invocation.  */

void
ffe_init_2 ()
{
  ++ffe_count_2;
  ffe_in_2 = TRUE;

  assert (ffe_program_unit_pool_ == NULL);
  ffe_program_unit_pool_ = malloc_pool_new ("Program unit", ffe_file_pool_, 1024);
  ffe_is_mainprog_ = FALSE;
  ffe_is_saveall_ = !ffe_is_automatic_;

  ffebad_init_2 ();
  ffebit_init_2 ();
  ffebld_init_2 ();
  ffecom_init_2 ();
  ffedata_init_2 ();
  ffeequiv_init_2 ();
  ffeexpr_init_2 ();
  ffeglobal_init_2 ();
  ffeimplic_init_2 ();
  ffeinfo_init_2 ();
  ffeintrin_init_2 ();
  ffelab_init_2 ();
  ffelex_init_2 ();
  ffename_init_2 ();
  ffesrc_init_2 ();
  ffest_init_2 ();
  ffestorag_init_2 ();
  ffesymbol_init_2 ();
  ffetarget_init_2 ();
  ffetype_init_2 ();
  ffewhere_init_2 ();

  ffe_init_3 ();
}

/* ffe_init_3 -- Initialize the FFE per any program unit

   ffe_init_3();

   Performs per-any-unit initialization; does NOT do
   per-statement-function-definition initialization (i.e. the chain
   of inits, from 0-3, breaks here; level 4 must be invoked independently).  */

void
ffe_init_3 ()
{
  ++ffe_count_3;
  ffe_in_3 = TRUE;

  assert (ffe_any_unit_pool_ == NULL);
  ffe_any_unit_pool_ = malloc_pool_new ("Any unit", ffe_program_unit_pool_, 1024);

  ffebad_init_3 ();
  ffebit_init_3 ();
  ffebld_init_3 ();
  ffecom_init_3 ();
  ffedata_init_3 ();
  ffeequiv_init_3 ();
  ffeexpr_init_3 ();
  ffeglobal_init_3 ();
  ffeimplic_init_3 ();
  ffeinfo_init_3 ();
  ffeintrin_init_3 ();
  ffelab_init_3 ();
  ffelex_init_3 ();
  ffename_init_3 ();
  ffesrc_init_3 ();
  ffest_init_3 ();
  ffestorag_init_3 ();
  ffesymbol_init_3 ();
  ffetarget_init_3 ();
  ffetype_init_3 ();
  ffewhere_init_3 ();
}

/* ffe_init_4 -- Initialize the FFE per statement function definition

   ffe_init_4();  */

void
ffe_init_4 ()
{
  ++ffe_count_4;
  ffe_in_4 = TRUE;

  ffebad_init_4 ();
  ffebit_init_4 ();
  ffebld_init_4 ();
  ffecom_init_4 ();
  ffedata_init_4 ();
  ffeequiv_init_4 ();
  ffeexpr_init_4 ();
  ffeglobal_init_4 ();
  ffeimplic_init_4 ();
  ffeinfo_init_4 ();
  ffeintrin_init_4 ();
  ffelab_init_4 ();
  ffelex_init_4 ();
  ffename_init_4 ();
  ffesrc_init_4 ();
  ffest_init_4 ();
  ffestorag_init_4 ();
  ffesymbol_init_4 ();
  ffetarget_init_4 ();
  ffetype_init_4 ();
  ffewhere_init_4 ();
}

/* ffe_terminate_0 -- Terminate the FFE prior to image termination

   ffe_terminate_0();  */

void
ffe_terminate_0 ()
{
  ffe_count_1 = 0;
  ffe_in_0 = FALSE;

  ffebad_terminate_0 ();
  ffebit_terminate_0 ();
  ffebld_terminate_0 ();
  ffecom_terminate_0 ();
  ffedata_terminate_0 ();
  ffeequiv_terminate_0 ();
  ffeexpr_terminate_0 ();
  ffeglobal_terminate_0 ();
  ffeimplic_terminate_0 ();
  ffeinfo_terminate_0 ();
  ffeintrin_terminate_0 ();
  ffelab_terminate_0 ();
  ffelex_terminate_0 ();
  ffename_terminate_0 ();
  ffesrc_terminate_0 ();
  ffest_terminate_0 ();
  ffestorag_terminate_0 ();
  ffesymbol_terminate_0 ();
  ffetarget_terminate_0 ();
  ffetype_terminate_0 ();
  ffewhere_terminate_0 ();
}

/* ffe_terminate_1 -- Terminate the FFE after seeing source file EOF

   ffe_terminate_1();  */

void
ffe_terminate_1 ()
{
  ffe_count_2 = 0;
  ffe_in_1 = FALSE;

  ffe_terminate_2 ();

  ffebad_terminate_1 ();
  ffebit_terminate_1 ();
  ffebld_terminate_1 ();
  ffecom_terminate_1 ();
  ffedata_terminate_1 ();
  ffeequiv_terminate_1 ();
  ffeexpr_terminate_1 ();
  ffeglobal_terminate_1 ();
  ffeimplic_terminate_1 ();
  ffeinfo_terminate_1 ();
  ffeintrin_terminate_1 ();
  ffelab_terminate_1 ();
  ffelex_terminate_1 ();
  ffename_terminate_1 ();
  ffesrc_terminate_1 ();
  ffest_terminate_1 ();
  ffestorag_terminate_1 ();
  ffesymbol_terminate_1 ();
  ffetarget_terminate_1 ();
  ffetype_terminate_1 ();
  ffewhere_terminate_1 ();

  assert (ffe_file_pool_ != NULL);
  malloc_pool_kill (ffe_file_pool_);
  ffe_file_pool_ = NULL;
}

/* ffe_terminate_2 -- Terminate the FFE after seeing outer program unit END

   ffe_terminate_2();  */

void
ffe_terminate_2 ()
{
  ffe_count_3 = 0;
  ffe_in_2 = FALSE;

  ffe_terminate_3 ();

  ffebad_terminate_2 ();
  ffebit_terminate_2 ();
  ffebld_terminate_2 ();
  ffecom_terminate_2 ();
  ffedata_terminate_2 ();
  ffeequiv_terminate_2 ();
  ffeexpr_terminate_2 ();
  ffeglobal_terminate_2 ();
  ffeimplic_terminate_2 ();
  ffeinfo_terminate_2 ();
  ffeintrin_terminate_2 ();
  ffelab_terminate_2 ();
  ffelex_terminate_2 ();
  ffename_terminate_2 ();
  ffesrc_terminate_2 ();
  ffest_terminate_2 ();
  ffestorag_terminate_2 ();
  ffesymbol_terminate_2 ();
  ffetarget_terminate_2 ();
  ffetype_terminate_2 ();
  ffewhere_terminate_2 ();

  assert (ffe_program_unit_pool_ != NULL);
  malloc_pool_kill (ffe_program_unit_pool_);
  ffe_program_unit_pool_ = NULL;
}

/* ffe_terminate_3 -- Terminate the FFE after seeing any program unit END

   ffe_terminate_3();  */

void
ffe_terminate_3 ()
{
  ffe_count_4 = 0;
  ffe_in_3 = FALSE;

  ffebad_terminate_3 ();
  ffebit_terminate_3 ();
  ffebld_terminate_3 ();
  ffecom_terminate_3 ();
  ffedata_terminate_3 ();
  ffeequiv_terminate_3 ();
  ffeexpr_terminate_3 ();
  ffeglobal_terminate_3 ();
  ffeimplic_terminate_3 ();
  ffeinfo_terminate_3 ();
  ffeintrin_terminate_3 ();
  ffelab_terminate_3 ();
  ffelex_terminate_3 ();
  ffename_terminate_3 ();
  ffesrc_terminate_3 ();
  ffest_terminate_3 ();
  ffestorag_terminate_3 ();
  ffesymbol_terminate_3 ();
  ffetarget_terminate_3 ();
  ffetype_terminate_3 ();
  ffewhere_terminate_3 ();

  assert (ffe_any_unit_pool_ != NULL);
  malloc_pool_kill (ffe_any_unit_pool_);
  ffe_any_unit_pool_ = NULL;
}

/* ffe_terminate_4 -- Terminate the FFE after seeing sfunc def expression

   ffe_terminate_4();  */

void
ffe_terminate_4 ()
{
  ffe_in_4 = FALSE;

  ffebad_terminate_4 ();
  ffebit_terminate_4 ();
  ffebld_terminate_4 ();
  ffecom_terminate_4 ();
  ffedata_terminate_4 ();
  ffeequiv_terminate_4 ();
  ffeexpr_terminate_4 ();
  ffeglobal_terminate_4 ();
  ffeimplic_terminate_4 ();
  ffeinfo_terminate_4 ();
  ffeintrin_terminate_4 ();
  ffelab_terminate_4 ();
  ffelex_terminate_4 ();
  ffename_terminate_4 ();
  ffesrc_terminate_4 ();
  ffest_terminate_4 ();
  ffestorag_terminate_4 ();
  ffesymbol_terminate_4 ();
  ffetarget_terminate_4 ();
  ffetype_terminate_4 ();
  ffewhere_terminate_4 ();
}
