/*
 *    perlapi.h
 *
 *    Copyright (C) 1999, 2000, 2001, 2002, 2003, 2004, by Larry Wall and others
 *
 *    You may distribute under the terms of either the GNU General Public
 *    License or the Artistic License, as specified in the README file.
 *
 * !!!!!!!   DO NOT EDIT THIS FILE   !!!!!!!
 * This file is built by embed.pl from data in embed.fnc, embed.pl,
 * pp.sym, intrpvar.h, perlvars.h and thrdvar.h.
 * Any changes made here will be lost!
 *
 * Edit those files and run 'make regen_headers' to effect changes.
 */

/* declare accessor functions for Perl variables */
#ifndef __perlapi_h__
#define __perlapi_h__

#if defined (MULTIPLICITY)

START_EXTERN_C

#undef PERLVAR
#undef PERLVARA
#undef PERLVARI
#undef PERLVARIC
#define PERLVAR(v,t)	EXTERN_C t* Perl_##v##_ptr(pTHX);
#define PERLVARA(v,n,t)	typedef t PL_##v##_t[n];			\
			EXTERN_C PL_##v##_t* Perl_##v##_ptr(pTHX);
#define PERLVARI(v,t,i)	PERLVAR(v,t)
#define PERLVARIC(v,t,i) PERLVAR(v, const t)

#include "thrdvar.h"
#include "intrpvar.h"
#include "perlvars.h"

#undef PERLVAR
#undef PERLVARA
#undef PERLVARI
#undef PERLVARIC

END_EXTERN_C

#if defined(PERL_CORE)

/* accessor functions for Perl variables (provide binary compatibility) */

/* these need to be mentioned here, or most linkers won't put them in
   the perl executable */

#ifndef PERL_NO_FORCE_LINK

START_EXTERN_C

#ifndef DOINIT
EXT void *PL_force_link_funcs[];
#else
EXT void *PL_force_link_funcs[] = {
#undef PERLVAR
#undef PERLVARA
#undef PERLVARI
#undef PERLVARIC
#define PERLVAR(v,t)	(void*)Perl_##v##_ptr,
#define PERLVARA(v,n,t)	PERLVAR(v,t)
#define PERLVARI(v,t,i)	PERLVAR(v,t)
#define PERLVARIC(v,t,i) PERLVAR(v,t)

#include "thrdvar.h"
#include "intrpvar.h"
#include "perlvars.h"

#undef PERLVAR
#undef PERLVARA
#undef PERLVARI
#undef PERLVARIC
};
#endif	/* DOINIT */

END_EXTERN_C

#endif	/* PERL_NO_FORCE_LINK */

#else	/* !PERL_CORE */

#undef  PL_Argv
#define PL_Argv			(*Perl_IArgv_ptr(aTHX))
#undef  PL_BINCOMPAT0
#define PL_BINCOMPAT0		(*Perl_IBINCOMPAT0_ptr(aTHX))
#undef  PL_Cmd
#define PL_Cmd			(*Perl_ICmd_ptr(aTHX))
#undef  PL_DBcv
#define PL_DBcv			(*Perl_IDBcv_ptr(aTHX))
#undef  PL_DBgv
#define PL_DBgv			(*Perl_IDBgv_ptr(aTHX))
#undef  PL_DBline
#define PL_DBline		(*Perl_IDBline_ptr(aTHX))
#undef  PL_DBsignal
#define PL_DBsignal		(*Perl_IDBsignal_ptr(aTHX))
#undef  PL_DBsingle
#define PL_DBsingle		(*Perl_IDBsingle_ptr(aTHX))
#undef  PL_DBsub
#define PL_DBsub		(*Perl_IDBsub_ptr(aTHX))
#undef  PL_DBtrace
#define PL_DBtrace		(*Perl_IDBtrace_ptr(aTHX))
#undef  PL_Dir
#define PL_Dir			(*Perl_IDir_ptr(aTHX))
#undef  PL_Env
#define PL_Env			(*Perl_IEnv_ptr(aTHX))
#undef  PL_LIO
#define PL_LIO			(*Perl_ILIO_ptr(aTHX))
#undef  PL_Mem
#define PL_Mem			(*Perl_IMem_ptr(aTHX))
#undef  PL_MemParse
#define PL_MemParse		(*Perl_IMemParse_ptr(aTHX))
#undef  PL_MemShared
#define PL_MemShared		(*Perl_IMemShared_ptr(aTHX))
#undef  PL_OpPtr
#define PL_OpPtr		(*Perl_IOpPtr_ptr(aTHX))
#undef  PL_OpSlab
#define PL_OpSlab		(*Perl_IOpSlab_ptr(aTHX))
#undef  PL_OpSpace
#define PL_OpSpace		(*Perl_IOpSpace_ptr(aTHX))
#undef  PL_Proc
#define PL_Proc			(*Perl_IProc_ptr(aTHX))
#undef  PL_Sock
#define PL_Sock			(*Perl_ISock_ptr(aTHX))
#undef  PL_StdIO
#define PL_StdIO		(*Perl_IStdIO_ptr(aTHX))
#undef  PL_amagic_generation
#define PL_amagic_generation	(*Perl_Iamagic_generation_ptr(aTHX))
#undef  PL_an
#define PL_an			(*Perl_Ian_ptr(aTHX))
#undef  PL_argvgv
#define PL_argvgv		(*Perl_Iargvgv_ptr(aTHX))
#undef  PL_argvout_stack
#define PL_argvout_stack	(*Perl_Iargvout_stack_ptr(aTHX))
#undef  PL_argvoutgv
#define PL_argvoutgv		(*Perl_Iargvoutgv_ptr(aTHX))
#undef  PL_basetime
#define PL_basetime		(*Perl_Ibasetime_ptr(aTHX))
#undef  PL_beginav
#define PL_beginav		(*Perl_Ibeginav_ptr(aTHX))
#undef  PL_beginav_save
#define PL_beginav_save		(*Perl_Ibeginav_save_ptr(aTHX))
#undef  PL_bitcount
#define PL_bitcount		(*Perl_Ibitcount_ptr(aTHX))
#undef  PL_bufend
#define PL_bufend		(*Perl_Ibufend_ptr(aTHX))
#undef  PL_bufptr
#define PL_bufptr		(*Perl_Ibufptr_ptr(aTHX))
#undef  PL_checkav
#define PL_checkav		(*Perl_Icheckav_ptr(aTHX))
#undef  PL_checkav_save
#define PL_checkav_save		(*Perl_Icheckav_save_ptr(aTHX))
#undef  PL_clocktick
#define PL_clocktick		(*Perl_Iclocktick_ptr(aTHX))
#undef  PL_collation_ix
#define PL_collation_ix		(*Perl_Icollation_ix_ptr(aTHX))
#undef  PL_collation_name
#define PL_collation_name	(*Perl_Icollation_name_ptr(aTHX))
#undef  PL_collation_standard
#define PL_collation_standard	(*Perl_Icollation_standard_ptr(aTHX))
#undef  PL_collxfrm_base
#define PL_collxfrm_base	(*Perl_Icollxfrm_base_ptr(aTHX))
#undef  PL_collxfrm_mult
#define PL_collxfrm_mult	(*Perl_Icollxfrm_mult_ptr(aTHX))
#undef  PL_compcv
#define PL_compcv		(*Perl_Icompcv_ptr(aTHX))
#undef  PL_compiling
#define PL_compiling		(*Perl_Icompiling_ptr(aTHX))
#undef  PL_comppad_name
#define PL_comppad_name		(*Perl_Icomppad_name_ptr(aTHX))
#undef  PL_comppad_name_fill
#define PL_comppad_name_fill	(*Perl_Icomppad_name_fill_ptr(aTHX))
#undef  PL_comppad_name_floor
#define PL_comppad_name_floor	(*Perl_Icomppad_name_floor_ptr(aTHX))
#undef  PL_cop_seqmax
#define PL_cop_seqmax		(*Perl_Icop_seqmax_ptr(aTHX))
#undef  PL_copline
#define PL_copline		(*Perl_Icopline_ptr(aTHX))
#undef  PL_cred_mutex
#define PL_cred_mutex		(*Perl_Icred_mutex_ptr(aTHX))
#undef  PL_cryptseen
#define PL_cryptseen		(*Perl_Icryptseen_ptr(aTHX))
#undef  PL_cshlen
#define PL_cshlen		(*Perl_Icshlen_ptr(aTHX))
#undef  PL_cshname
#define PL_cshname		(*Perl_Icshname_ptr(aTHX))
#undef  PL_curcopdb
#define PL_curcopdb		(*Perl_Icurcopdb_ptr(aTHX))
#undef  PL_curstname
#define PL_curstname		(*Perl_Icurstname_ptr(aTHX))
#undef  PL_curthr
#define PL_curthr		(*Perl_Icurthr_ptr(aTHX))
#undef  PL_custom_op_descs
#define PL_custom_op_descs	(*Perl_Icustom_op_descs_ptr(aTHX))
#undef  PL_custom_op_names
#define PL_custom_op_names	(*Perl_Icustom_op_names_ptr(aTHX))
#undef  PL_dbargs
#define PL_dbargs		(*Perl_Idbargs_ptr(aTHX))
#undef  PL_debstash
#define PL_debstash		(*Perl_Idebstash_ptr(aTHX))
#undef  PL_debug
#define PL_debug		(*Perl_Idebug_ptr(aTHX))
#undef  PL_debug_pad
#define PL_debug_pad		(*Perl_Idebug_pad_ptr(aTHX))
#undef  PL_def_layerlist
#define PL_def_layerlist	(*Perl_Idef_layerlist_ptr(aTHX))
#undef  PL_defgv
#define PL_defgv		(*Perl_Idefgv_ptr(aTHX))
#undef  PL_diehook
#define PL_diehook		(*Perl_Idiehook_ptr(aTHX))
#undef  PL_doextract
#define PL_doextract		(*Perl_Idoextract_ptr(aTHX))
#undef  PL_doswitches
#define PL_doswitches		(*Perl_Idoswitches_ptr(aTHX))
#undef  PL_dowarn
#define PL_dowarn		(*Perl_Idowarn_ptr(aTHX))
#undef  PL_e_script
#define PL_e_script		(*Perl_Ie_script_ptr(aTHX))
#undef  PL_egid
#define PL_egid			(*Perl_Iegid_ptr(aTHX))
#undef  PL_encoding
#define PL_encoding		(*Perl_Iencoding_ptr(aTHX))
#undef  PL_endav
#define PL_endav		(*Perl_Iendav_ptr(aTHX))
#undef  PL_envgv
#define PL_envgv		(*Perl_Ienvgv_ptr(aTHX))
#undef  PL_errgv
#define PL_errgv		(*Perl_Ierrgv_ptr(aTHX))
#undef  PL_error_count
#define PL_error_count		(*Perl_Ierror_count_ptr(aTHX))
#undef  PL_euid
#define PL_euid			(*Perl_Ieuid_ptr(aTHX))
#undef  PL_eval_cond
#define PL_eval_cond		(*Perl_Ieval_cond_ptr(aTHX))
#undef  PL_eval_mutex
#define PL_eval_mutex		(*Perl_Ieval_mutex_ptr(aTHX))
#undef  PL_eval_owner
#define PL_eval_owner		(*Perl_Ieval_owner_ptr(aTHX))
#undef  PL_eval_root
#define PL_eval_root		(*Perl_Ieval_root_ptr(aTHX))
#undef  PL_eval_start
#define PL_eval_start		(*Perl_Ieval_start_ptr(aTHX))
#undef  PL_evalseq
#define PL_evalseq		(*Perl_Ievalseq_ptr(aTHX))
#undef  PL_exit_flags
#define PL_exit_flags		(*Perl_Iexit_flags_ptr(aTHX))
#undef  PL_exitlist
#define PL_exitlist		(*Perl_Iexitlist_ptr(aTHX))
#undef  PL_exitlistlen
#define PL_exitlistlen		(*Perl_Iexitlistlen_ptr(aTHX))
#undef  PL_expect
#define PL_expect		(*Perl_Iexpect_ptr(aTHX))
#undef  PL_fdpid
#define PL_fdpid		(*Perl_Ifdpid_ptr(aTHX))
#undef  PL_fdpid_mutex
#define PL_fdpid_mutex		(*Perl_Ifdpid_mutex_ptr(aTHX))
#undef  PL_fdscript
#define PL_fdscript		(*Perl_Ifdscript_ptr(aTHX))
#undef  PL_filemode
#define PL_filemode		(*Perl_Ifilemode_ptr(aTHX))
#undef  PL_forkprocess
#define PL_forkprocess		(*Perl_Iforkprocess_ptr(aTHX))
#undef  PL_formfeed
#define PL_formfeed		(*Perl_Iformfeed_ptr(aTHX))
#undef  PL_generation
#define PL_generation		(*Perl_Igeneration_ptr(aTHX))
#undef  PL_gensym
#define PL_gensym		(*Perl_Igensym_ptr(aTHX))
#undef  PL_gid
#define PL_gid			(*Perl_Igid_ptr(aTHX))
#undef  PL_glob_index
#define PL_glob_index		(*Perl_Iglob_index_ptr(aTHX))
#undef  PL_globalstash
#define PL_globalstash		(*Perl_Iglobalstash_ptr(aTHX))
#undef  PL_hash_seed
#define PL_hash_seed		(*Perl_Ihash_seed_ptr(aTHX))
#undef  PL_hash_seed_set
#define PL_hash_seed_set	(*Perl_Ihash_seed_set_ptr(aTHX))
#undef  PL_he_arenaroot
#define PL_he_arenaroot		(*Perl_Ihe_arenaroot_ptr(aTHX))
#undef  PL_he_root
#define PL_he_root		(*Perl_Ihe_root_ptr(aTHX))
#undef  PL_hintgv
#define PL_hintgv		(*Perl_Ihintgv_ptr(aTHX))
#undef  PL_hints
#define PL_hints		(*Perl_Ihints_ptr(aTHX))
#undef  PL_in_clean_all
#define PL_in_clean_all		(*Perl_Iin_clean_all_ptr(aTHX))
#undef  PL_in_clean_objs
#define PL_in_clean_objs	(*Perl_Iin_clean_objs_ptr(aTHX))
#undef  PL_in_load_module
#define PL_in_load_module	(*Perl_Iin_load_module_ptr(aTHX))
#undef  PL_in_my
#define PL_in_my		(*Perl_Iin_my_ptr(aTHX))
#undef  PL_in_my_stash
#define PL_in_my_stash		(*Perl_Iin_my_stash_ptr(aTHX))
#undef  PL_incgv
#define PL_incgv		(*Perl_Iincgv_ptr(aTHX))
#undef  PL_initav
#define PL_initav		(*Perl_Iinitav_ptr(aTHX))
#undef  PL_inplace
#define PL_inplace		(*Perl_Iinplace_ptr(aTHX))
#undef  PL_known_layers
#define PL_known_layers		(*Perl_Iknown_layers_ptr(aTHX))
#undef  PL_last_lop
#define PL_last_lop		(*Perl_Ilast_lop_ptr(aTHX))
#undef  PL_last_lop_op
#define PL_last_lop_op		(*Perl_Ilast_lop_op_ptr(aTHX))
#undef  PL_last_swash_hv
#define PL_last_swash_hv	(*Perl_Ilast_swash_hv_ptr(aTHX))
#undef  PL_last_swash_key
#define PL_last_swash_key	(*Perl_Ilast_swash_key_ptr(aTHX))
#undef  PL_last_swash_klen
#define PL_last_swash_klen	(*Perl_Ilast_swash_klen_ptr(aTHX))
#undef  PL_last_swash_slen
#define PL_last_swash_slen	(*Perl_Ilast_swash_slen_ptr(aTHX))
#undef  PL_last_swash_tmps
#define PL_last_swash_tmps	(*Perl_Ilast_swash_tmps_ptr(aTHX))
#undef  PL_last_uni
#define PL_last_uni		(*Perl_Ilast_uni_ptr(aTHX))
#undef  PL_lastfd
#define PL_lastfd		(*Perl_Ilastfd_ptr(aTHX))
#undef  PL_laststatval
#define PL_laststatval		(*Perl_Ilaststatval_ptr(aTHX))
#undef  PL_laststype
#define PL_laststype		(*Perl_Ilaststype_ptr(aTHX))
#undef  PL_lex_brackets
#define PL_lex_brackets		(*Perl_Ilex_brackets_ptr(aTHX))
#undef  PL_lex_brackstack
#define PL_lex_brackstack	(*Perl_Ilex_brackstack_ptr(aTHX))
#undef  PL_lex_casemods
#define PL_lex_casemods		(*Perl_Ilex_casemods_ptr(aTHX))
#undef  PL_lex_casestack
#define PL_lex_casestack	(*Perl_Ilex_casestack_ptr(aTHX))
#undef  PL_lex_defer
#define PL_lex_defer		(*Perl_Ilex_defer_ptr(aTHX))
#undef  PL_lex_dojoin
#define PL_lex_dojoin		(*Perl_Ilex_dojoin_ptr(aTHX))
#undef  PL_lex_expect
#define PL_lex_expect		(*Perl_Ilex_expect_ptr(aTHX))
#undef  PL_lex_formbrack
#define PL_lex_formbrack	(*Perl_Ilex_formbrack_ptr(aTHX))
#undef  PL_lex_inpat
#define PL_lex_inpat		(*Perl_Ilex_inpat_ptr(aTHX))
#undef  PL_lex_inwhat
#define PL_lex_inwhat		(*Perl_Ilex_inwhat_ptr(aTHX))
#undef  PL_lex_op
#define PL_lex_op		(*Perl_Ilex_op_ptr(aTHX))
#undef  PL_lex_repl
#define PL_lex_repl		(*Perl_Ilex_repl_ptr(aTHX))
#undef  PL_lex_starts
#define PL_lex_starts		(*Perl_Ilex_starts_ptr(aTHX))
#undef  PL_lex_state
#define PL_lex_state		(*Perl_Ilex_state_ptr(aTHX))
#undef  PL_lex_stuff
#define PL_lex_stuff		(*Perl_Ilex_stuff_ptr(aTHX))
#undef  PL_lineary
#define PL_lineary		(*Perl_Ilineary_ptr(aTHX))
#undef  PL_linestart
#define PL_linestart		(*Perl_Ilinestart_ptr(aTHX))
#undef  PL_linestr
#define PL_linestr		(*Perl_Ilinestr_ptr(aTHX))
#undef  PL_localpatches
#define PL_localpatches		(*Perl_Ilocalpatches_ptr(aTHX))
#undef  PL_lockhook
#define PL_lockhook		(*Perl_Ilockhook_ptr(aTHX))
#undef  PL_main_cv
#define PL_main_cv		(*Perl_Imain_cv_ptr(aTHX))
#undef  PL_main_root
#define PL_main_root		(*Perl_Imain_root_ptr(aTHX))
#undef  PL_main_start
#define PL_main_start		(*Perl_Imain_start_ptr(aTHX))
#undef  PL_max_intro_pending
#define PL_max_intro_pending	(*Perl_Imax_intro_pending_ptr(aTHX))
#undef  PL_maxo
#define PL_maxo			(*Perl_Imaxo_ptr(aTHX))
#undef  PL_maxsysfd
#define PL_maxsysfd		(*Perl_Imaxsysfd_ptr(aTHX))
#undef  PL_mess_sv
#define PL_mess_sv		(*Perl_Imess_sv_ptr(aTHX))
#undef  PL_min_intro_pending
#define PL_min_intro_pending	(*Perl_Imin_intro_pending_ptr(aTHX))
#undef  PL_minus_F
#define PL_minus_F		(*Perl_Iminus_F_ptr(aTHX))
#undef  PL_minus_a
#define PL_minus_a		(*Perl_Iminus_a_ptr(aTHX))
#undef  PL_minus_c
#define PL_minus_c		(*Perl_Iminus_c_ptr(aTHX))
#undef  PL_minus_l
#define PL_minus_l		(*Perl_Iminus_l_ptr(aTHX))
#undef  PL_minus_n
#define PL_minus_n		(*Perl_Iminus_n_ptr(aTHX))
#undef  PL_minus_p
#define PL_minus_p		(*Perl_Iminus_p_ptr(aTHX))
#undef  PL_modglobal
#define PL_modglobal		(*Perl_Imodglobal_ptr(aTHX))
#undef  PL_multi_close
#define PL_multi_close		(*Perl_Imulti_close_ptr(aTHX))
#undef  PL_multi_end
#define PL_multi_end		(*Perl_Imulti_end_ptr(aTHX))
#undef  PL_multi_open
#define PL_multi_open		(*Perl_Imulti_open_ptr(aTHX))
#undef  PL_multi_start
#define PL_multi_start		(*Perl_Imulti_start_ptr(aTHX))
#undef  PL_multiline
#define PL_multiline		(*Perl_Imultiline_ptr(aTHX))
#undef  PL_nexttoke
#define PL_nexttoke		(*Perl_Inexttoke_ptr(aTHX))
#undef  PL_nexttype
#define PL_nexttype		(*Perl_Inexttype_ptr(aTHX))
#undef  PL_nextval
#define PL_nextval		(*Perl_Inextval_ptr(aTHX))
#undef  PL_nice_chunk
#define PL_nice_chunk		(*Perl_Inice_chunk_ptr(aTHX))
#undef  PL_nice_chunk_size
#define PL_nice_chunk_size	(*Perl_Inice_chunk_size_ptr(aTHX))
#undef  PL_nomemok
#define PL_nomemok		(*Perl_Inomemok_ptr(aTHX))
#undef  PL_nthreads
#define PL_nthreads		(*Perl_Inthreads_ptr(aTHX))
#undef  PL_nthreads_cond
#define PL_nthreads_cond	(*Perl_Inthreads_cond_ptr(aTHX))
#undef  PL_nullstash
#define PL_nullstash		(*Perl_Inullstash_ptr(aTHX))
#undef  PL_numeric_compat1
#define PL_numeric_compat1	(*Perl_Inumeric_compat1_ptr(aTHX))
#undef  PL_numeric_local
#define PL_numeric_local	(*Perl_Inumeric_local_ptr(aTHX))
#undef  PL_numeric_name
#define PL_numeric_name		(*Perl_Inumeric_name_ptr(aTHX))
#undef  PL_numeric_radix_sv
#define PL_numeric_radix_sv	(*Perl_Inumeric_radix_sv_ptr(aTHX))
#undef  PL_numeric_standard
#define PL_numeric_standard	(*Perl_Inumeric_standard_ptr(aTHX))
#undef  PL_ofmt
#define PL_ofmt			(*Perl_Iofmt_ptr(aTHX))
#undef  PL_oldbufptr
#define PL_oldbufptr		(*Perl_Ioldbufptr_ptr(aTHX))
#undef  PL_oldname
#define PL_oldname		(*Perl_Ioldname_ptr(aTHX))
#undef  PL_oldoldbufptr
#define PL_oldoldbufptr		(*Perl_Ioldoldbufptr_ptr(aTHX))
#undef  PL_op_mask
#define PL_op_mask		(*Perl_Iop_mask_ptr(aTHX))
#undef  PL_op_seqmax
#define PL_op_seqmax		(*Perl_Iop_seqmax_ptr(aTHX))
#undef  PL_origalen
#define PL_origalen		(*Perl_Iorigalen_ptr(aTHX))
#undef  PL_origargc
#define PL_origargc		(*Perl_Iorigargc_ptr(aTHX))
#undef  PL_origargv
#define PL_origargv		(*Perl_Iorigargv_ptr(aTHX))
#undef  PL_origenviron
#define PL_origenviron		(*Perl_Iorigenviron_ptr(aTHX))
#undef  PL_origfilename
#define PL_origfilename		(*Perl_Iorigfilename_ptr(aTHX))
#undef  PL_ors_sv
#define PL_ors_sv		(*Perl_Iors_sv_ptr(aTHX))
#undef  PL_osname
#define PL_osname		(*Perl_Iosname_ptr(aTHX))
#undef  PL_pad_reset_pending
#define PL_pad_reset_pending	(*Perl_Ipad_reset_pending_ptr(aTHX))
#undef  PL_padix
#define PL_padix		(*Perl_Ipadix_ptr(aTHX))
#undef  PL_padix_floor
#define PL_padix_floor		(*Perl_Ipadix_floor_ptr(aTHX))
#undef  PL_patchlevel
#define PL_patchlevel		(*Perl_Ipatchlevel_ptr(aTHX))
#undef  PL_pending_ident
#define PL_pending_ident	(*Perl_Ipending_ident_ptr(aTHX))
#undef  PL_perl_destruct_level
#define PL_perl_destruct_level	(*Perl_Iperl_destruct_level_ptr(aTHX))
#undef  PL_perldb
#define PL_perldb		(*Perl_Iperldb_ptr(aTHX))
#undef  PL_perlio
#define PL_perlio		(*Perl_Iperlio_ptr(aTHX))
#undef  PL_pidstatus
#define PL_pidstatus		(*Perl_Ipidstatus_ptr(aTHX))
#undef  PL_ppid
#define PL_ppid			(*Perl_Ippid_ptr(aTHX))
#undef  PL_preambleav
#define PL_preambleav		(*Perl_Ipreambleav_ptr(aTHX))
#undef  PL_preambled
#define PL_preambled		(*Perl_Ipreambled_ptr(aTHX))
#undef  PL_preprocess
#define PL_preprocess		(*Perl_Ipreprocess_ptr(aTHX))
#undef  PL_profiledata
#define PL_profiledata		(*Perl_Iprofiledata_ptr(aTHX))
#undef  PL_psig_name
#define PL_psig_name		(*Perl_Ipsig_name_ptr(aTHX))
#undef  PL_psig_pend
#define PL_psig_pend		(*Perl_Ipsig_pend_ptr(aTHX))
#undef  PL_psig_ptr
#define PL_psig_ptr		(*Perl_Ipsig_ptr_ptr(aTHX))
#undef  PL_ptr_table
#define PL_ptr_table		(*Perl_Iptr_table_ptr(aTHX))
#undef  PL_reentrant_buffer
#define PL_reentrant_buffer	(*Perl_Ireentrant_buffer_ptr(aTHX))
#undef  PL_reentrant_retint
#define PL_reentrant_retint	(*Perl_Ireentrant_retint_ptr(aTHX))
#undef  PL_regex_pad
#define PL_regex_pad		(*Perl_Iregex_pad_ptr(aTHX))
#undef  PL_regex_padav
#define PL_regex_padav		(*Perl_Iregex_padav_ptr(aTHX))
#undef  PL_rehash_seed
#define PL_rehash_seed		(*Perl_Irehash_seed_ptr(aTHX))
#undef  PL_rehash_seed_set
#define PL_rehash_seed_set	(*Perl_Irehash_seed_set_ptr(aTHX))
#undef  PL_replgv
#define PL_replgv		(*Perl_Ireplgv_ptr(aTHX))
#undef  PL_rsfp
#define PL_rsfp			(*Perl_Irsfp_ptr(aTHX))
#undef  PL_rsfp_filters
#define PL_rsfp_filters		(*Perl_Irsfp_filters_ptr(aTHX))
#undef  PL_runops
#define PL_runops		(*Perl_Irunops_ptr(aTHX))
#undef  PL_runops_dbg
#define PL_runops_dbg		(*Perl_Irunops_dbg_ptr(aTHX))
#undef  PL_runops_std
#define PL_runops_std		(*Perl_Irunops_std_ptr(aTHX))
#undef  PL_savebegin
#define PL_savebegin		(*Perl_Isavebegin_ptr(aTHX))
#undef  PL_sawampersand
#define PL_sawampersand		(*Perl_Isawampersand_ptr(aTHX))
#undef  PL_sh_path_compat
#define PL_sh_path_compat	(*Perl_Ish_path_compat_ptr(aTHX))
#undef  PL_sharehook
#define PL_sharehook		(*Perl_Isharehook_ptr(aTHX))
#undef  PL_sig_pending
#define PL_sig_pending		(*Perl_Isig_pending_ptr(aTHX))
#undef  PL_sighandlerp
#define PL_sighandlerp		(*Perl_Isighandlerp_ptr(aTHX))
#undef  PL_signals
#define PL_signals		(*Perl_Isignals_ptr(aTHX))
#undef  PL_sort_RealCmp
#define PL_sort_RealCmp		(*Perl_Isort_RealCmp_ptr(aTHX))
#undef  PL_splitstr
#define PL_splitstr		(*Perl_Isplitstr_ptr(aTHX))
#undef  PL_srand_called
#define PL_srand_called		(*Perl_Isrand_called_ptr(aTHX))
#undef  PL_stashcache
#define PL_stashcache		(*Perl_Istashcache_ptr(aTHX))
#undef  PL_statusvalue
#define PL_statusvalue		(*Perl_Istatusvalue_ptr(aTHX))
#undef  PL_statusvalue_vms
#define PL_statusvalue_vms	(*Perl_Istatusvalue_vms_ptr(aTHX))
#undef  PL_stderrgv
#define PL_stderrgv		(*Perl_Istderrgv_ptr(aTHX))
#undef  PL_stdingv
#define PL_stdingv		(*Perl_Istdingv_ptr(aTHX))
#undef  PL_strtab
#define PL_strtab		(*Perl_Istrtab_ptr(aTHX))
#undef  PL_strtab_mutex
#define PL_strtab_mutex		(*Perl_Istrtab_mutex_ptr(aTHX))
#undef  PL_sub_generation
#define PL_sub_generation	(*Perl_Isub_generation_ptr(aTHX))
#undef  PL_sublex_info
#define PL_sublex_info		(*Perl_Isublex_info_ptr(aTHX))
#undef  PL_subline
#define PL_subline		(*Perl_Isubline_ptr(aTHX))
#undef  PL_subname
#define PL_subname		(*Perl_Isubname_ptr(aTHX))
#undef  PL_suidscript
#define PL_suidscript		(*Perl_Isuidscript_ptr(aTHX))
#undef  PL_sv_arenaroot
#define PL_sv_arenaroot		(*Perl_Isv_arenaroot_ptr(aTHX))
#undef  PL_sv_count
#define PL_sv_count		(*Perl_Isv_count_ptr(aTHX))
#undef  PL_sv_lock_mutex
#define PL_sv_lock_mutex	(*Perl_Isv_lock_mutex_ptr(aTHX))
#undef  PL_sv_mutex
#define PL_sv_mutex		(*Perl_Isv_mutex_ptr(aTHX))
#undef  PL_sv_no
#define PL_sv_no		(*Perl_Isv_no_ptr(aTHX))
#undef  PL_sv_objcount
#define PL_sv_objcount		(*Perl_Isv_objcount_ptr(aTHX))
#undef  PL_sv_root
#define PL_sv_root		(*Perl_Isv_root_ptr(aTHX))
#undef  PL_sv_undef
#define PL_sv_undef		(*Perl_Isv_undef_ptr(aTHX))
#undef  PL_sv_yes
#define PL_sv_yes		(*Perl_Isv_yes_ptr(aTHX))
#undef  PL_svref_mutex
#define PL_svref_mutex		(*Perl_Isvref_mutex_ptr(aTHX))
#undef  PL_sys_intern
#define PL_sys_intern		(*Perl_Isys_intern_ptr(aTHX))
#undef  PL_taint_warn
#define PL_taint_warn		(*Perl_Itaint_warn_ptr(aTHX))
#undef  PL_tainting
#define PL_tainting		(*Perl_Itainting_ptr(aTHX))
#undef  PL_threadhook
#define PL_threadhook		(*Perl_Ithreadhook_ptr(aTHX))
#undef  PL_threadnum
#define PL_threadnum		(*Perl_Ithreadnum_ptr(aTHX))
#undef  PL_threads_mutex
#define PL_threads_mutex	(*Perl_Ithreads_mutex_ptr(aTHX))
#undef  PL_threadsv_names
#define PL_threadsv_names	(*Perl_Ithreadsv_names_ptr(aTHX))
#undef  PL_thrsv
#define PL_thrsv		(*Perl_Ithrsv_ptr(aTHX))
#undef  PL_tokenbuf
#define PL_tokenbuf		(*Perl_Itokenbuf_ptr(aTHX))
#undef  PL_uid
#define PL_uid			(*Perl_Iuid_ptr(aTHX))
#undef  PL_unicode
#define PL_unicode		(*Perl_Iunicode_ptr(aTHX))
#undef  PL_unlockhook
#define PL_unlockhook		(*Perl_Iunlockhook_ptr(aTHX))
#undef  PL_unsafe
#define PL_unsafe		(*Perl_Iunsafe_ptr(aTHX))
#undef  PL_utf8_alnum
#define PL_utf8_alnum		(*Perl_Iutf8_alnum_ptr(aTHX))
#undef  PL_utf8_alnumc
#define PL_utf8_alnumc		(*Perl_Iutf8_alnumc_ptr(aTHX))
#undef  PL_utf8_alpha
#define PL_utf8_alpha		(*Perl_Iutf8_alpha_ptr(aTHX))
#undef  PL_utf8_ascii
#define PL_utf8_ascii		(*Perl_Iutf8_ascii_ptr(aTHX))
#undef  PL_utf8_cntrl
#define PL_utf8_cntrl		(*Perl_Iutf8_cntrl_ptr(aTHX))
#undef  PL_utf8_digit
#define PL_utf8_digit		(*Perl_Iutf8_digit_ptr(aTHX))
#undef  PL_utf8_graph
#define PL_utf8_graph		(*Perl_Iutf8_graph_ptr(aTHX))
#undef  PL_utf8_idcont
#define PL_utf8_idcont		(*Perl_Iutf8_idcont_ptr(aTHX))
#undef  PL_utf8_idstart
#define PL_utf8_idstart		(*Perl_Iutf8_idstart_ptr(aTHX))
#undef  PL_utf8_lower
#define PL_utf8_lower		(*Perl_Iutf8_lower_ptr(aTHX))
#undef  PL_utf8_mark
#define PL_utf8_mark		(*Perl_Iutf8_mark_ptr(aTHX))
#undef  PL_utf8_print
#define PL_utf8_print		(*Perl_Iutf8_print_ptr(aTHX))
#undef  PL_utf8_punct
#define PL_utf8_punct		(*Perl_Iutf8_punct_ptr(aTHX))
#undef  PL_utf8_space
#define PL_utf8_space		(*Perl_Iutf8_space_ptr(aTHX))
#undef  PL_utf8_tofold
#define PL_utf8_tofold		(*Perl_Iutf8_tofold_ptr(aTHX))
#undef  PL_utf8_tolower
#define PL_utf8_tolower		(*Perl_Iutf8_tolower_ptr(aTHX))
#undef  PL_utf8_totitle
#define PL_utf8_totitle		(*Perl_Iutf8_totitle_ptr(aTHX))
#undef  PL_utf8_toupper
#define PL_utf8_toupper		(*Perl_Iutf8_toupper_ptr(aTHX))
#undef  PL_utf8_upper
#define PL_utf8_upper		(*Perl_Iutf8_upper_ptr(aTHX))
#undef  PL_utf8_xdigit
#define PL_utf8_xdigit		(*Perl_Iutf8_xdigit_ptr(aTHX))
#undef  PL_utf8locale
#define PL_utf8locale		(*Perl_Iutf8locale_ptr(aTHX))
#undef  PL_uudmap
#define PL_uudmap		(*Perl_Iuudmap_ptr(aTHX))
#undef  PL_warnhook
#define PL_warnhook		(*Perl_Iwarnhook_ptr(aTHX))
#undef  PL_widesyscalls
#define PL_widesyscalls		(*Perl_Iwidesyscalls_ptr(aTHX))
#undef  PL_xiv_arenaroot
#define PL_xiv_arenaroot	(*Perl_Ixiv_arenaroot_ptr(aTHX))
#undef  PL_xiv_root
#define PL_xiv_root		(*Perl_Ixiv_root_ptr(aTHX))
#undef  PL_xnv_arenaroot
#define PL_xnv_arenaroot	(*Perl_Ixnv_arenaroot_ptr(aTHX))
#undef  PL_xnv_root
#define PL_xnv_root		(*Perl_Ixnv_root_ptr(aTHX))
#undef  PL_xpv_arenaroot
#define PL_xpv_arenaroot	(*Perl_Ixpv_arenaroot_ptr(aTHX))
#undef  PL_xpv_root
#define PL_xpv_root		(*Perl_Ixpv_root_ptr(aTHX))
#undef  PL_xpvav_arenaroot
#define PL_xpvav_arenaroot	(*Perl_Ixpvav_arenaroot_ptr(aTHX))
#undef  PL_xpvav_root
#define PL_xpvav_root		(*Perl_Ixpvav_root_ptr(aTHX))
#undef  PL_xpvbm_arenaroot
#define PL_xpvbm_arenaroot	(*Perl_Ixpvbm_arenaroot_ptr(aTHX))
#undef  PL_xpvbm_root
#define PL_xpvbm_root		(*Perl_Ixpvbm_root_ptr(aTHX))
#undef  PL_xpvcv_arenaroot
#define PL_xpvcv_arenaroot	(*Perl_Ixpvcv_arenaroot_ptr(aTHX))
#undef  PL_xpvcv_root
#define PL_xpvcv_root		(*Perl_Ixpvcv_root_ptr(aTHX))
#undef  PL_xpvhv_arenaroot
#define PL_xpvhv_arenaroot	(*Perl_Ixpvhv_arenaroot_ptr(aTHX))
#undef  PL_xpvhv_root
#define PL_xpvhv_root		(*Perl_Ixpvhv_root_ptr(aTHX))
#undef  PL_xpviv_arenaroot
#define PL_xpviv_arenaroot	(*Perl_Ixpviv_arenaroot_ptr(aTHX))
#undef  PL_xpviv_root
#define PL_xpviv_root		(*Perl_Ixpviv_root_ptr(aTHX))
#undef  PL_xpvlv_arenaroot
#define PL_xpvlv_arenaroot	(*Perl_Ixpvlv_arenaroot_ptr(aTHX))
#undef  PL_xpvlv_root
#define PL_xpvlv_root		(*Perl_Ixpvlv_root_ptr(aTHX))
#undef  PL_xpvmg_arenaroot
#define PL_xpvmg_arenaroot	(*Perl_Ixpvmg_arenaroot_ptr(aTHX))
#undef  PL_xpvmg_root
#define PL_xpvmg_root		(*Perl_Ixpvmg_root_ptr(aTHX))
#undef  PL_xpvnv_arenaroot
#define PL_xpvnv_arenaroot	(*Perl_Ixpvnv_arenaroot_ptr(aTHX))
#undef  PL_xpvnv_root
#define PL_xpvnv_root		(*Perl_Ixpvnv_root_ptr(aTHX))
#undef  PL_xrv_arenaroot
#define PL_xrv_arenaroot	(*Perl_Ixrv_arenaroot_ptr(aTHX))
#undef  PL_xrv_root
#define PL_xrv_root		(*Perl_Ixrv_root_ptr(aTHX))
#undef  PL_yychar
#define PL_yychar		(*Perl_Iyychar_ptr(aTHX))
#undef  PL_yydebug
#define PL_yydebug		(*Perl_Iyydebug_ptr(aTHX))
#undef  PL_yyerrflag
#define PL_yyerrflag		(*Perl_Iyyerrflag_ptr(aTHX))
#undef  PL_yylval
#define PL_yylval		(*Perl_Iyylval_ptr(aTHX))
#undef  PL_yynerrs
#define PL_yynerrs		(*Perl_Iyynerrs_ptr(aTHX))
#undef  PL_yyval
#define PL_yyval		(*Perl_Iyyval_ptr(aTHX))
#undef  PL_Sv
#define PL_Sv			(*Perl_TSv_ptr(aTHX))
#undef  PL_Xpv
#define PL_Xpv			(*Perl_TXpv_ptr(aTHX))
#undef  PL_av_fetch_sv
#define PL_av_fetch_sv		(*Perl_Tav_fetch_sv_ptr(aTHX))
#undef  PL_bodytarget
#define PL_bodytarget		(*Perl_Tbodytarget_ptr(aTHX))
#undef  PL_bostr
#define PL_bostr		(*Perl_Tbostr_ptr(aTHX))
#undef  PL_chopset
#define PL_chopset		(*Perl_Tchopset_ptr(aTHX))
#undef  PL_colors
#define PL_colors		(*Perl_Tcolors_ptr(aTHX))
#undef  PL_colorset
#define PL_colorset		(*Perl_Tcolorset_ptr(aTHX))
#undef  PL_comppad
#define PL_comppad		(*Perl_Tcomppad_ptr(aTHX))
#undef  PL_curcop
#define PL_curcop		(*Perl_Tcurcop_ptr(aTHX))
#undef  PL_curpad
#define PL_curpad		(*Perl_Tcurpad_ptr(aTHX))
#undef  PL_curpm
#define PL_curpm		(*Perl_Tcurpm_ptr(aTHX))
#undef  PL_curstack
#define PL_curstack		(*Perl_Tcurstack_ptr(aTHX))
#undef  PL_curstackinfo
#define PL_curstackinfo		(*Perl_Tcurstackinfo_ptr(aTHX))
#undef  PL_curstash
#define PL_curstash		(*Perl_Tcurstash_ptr(aTHX))
#undef  PL_defoutgv
#define PL_defoutgv		(*Perl_Tdefoutgv_ptr(aTHX))
#undef  PL_defstash
#define PL_defstash		(*Perl_Tdefstash_ptr(aTHX))
#undef  PL_delaymagic
#define PL_delaymagic		(*Perl_Tdelaymagic_ptr(aTHX))
#undef  PL_dirty
#define PL_dirty		(*Perl_Tdirty_ptr(aTHX))
#undef  PL_dumpindent
#define PL_dumpindent		(*Perl_Tdumpindent_ptr(aTHX))
#undef  PL_efloatbuf
#define PL_efloatbuf		(*Perl_Tefloatbuf_ptr(aTHX))
#undef  PL_efloatsize
#define PL_efloatsize		(*Perl_Tefloatsize_ptr(aTHX))
#undef  PL_errors
#define PL_errors		(*Perl_Terrors_ptr(aTHX))
#undef  PL_extralen
#define PL_extralen		(*Perl_Textralen_ptr(aTHX))
#undef  PL_firstgv
#define PL_firstgv		(*Perl_Tfirstgv_ptr(aTHX))
#undef  PL_formtarget
#define PL_formtarget		(*Perl_Tformtarget_ptr(aTHX))
#undef  PL_hv_fetch_ent_mh
#define PL_hv_fetch_ent_mh	(*Perl_Thv_fetch_ent_mh_ptr(aTHX))
#undef  PL_hv_fetch_sv
#define PL_hv_fetch_sv		(*Perl_Thv_fetch_sv_ptr(aTHX))
#undef  PL_in_eval
#define PL_in_eval		(*Perl_Tin_eval_ptr(aTHX))
#undef  PL_last_in_gv
#define PL_last_in_gv		(*Perl_Tlast_in_gv_ptr(aTHX))
#undef  PL_lastgotoprobe
#define PL_lastgotoprobe	(*Perl_Tlastgotoprobe_ptr(aTHX))
#undef  PL_lastscream
#define PL_lastscream		(*Perl_Tlastscream_ptr(aTHX))
#undef  PL_localizing
#define PL_localizing		(*Perl_Tlocalizing_ptr(aTHX))
#undef  PL_mainstack
#define PL_mainstack		(*Perl_Tmainstack_ptr(aTHX))
#undef  PL_markstack
#define PL_markstack		(*Perl_Tmarkstack_ptr(aTHX))
#undef  PL_markstack_max
#define PL_markstack_max	(*Perl_Tmarkstack_max_ptr(aTHX))
#undef  PL_markstack_ptr
#define PL_markstack_ptr	(*Perl_Tmarkstack_ptr_ptr(aTHX))
#undef  PL_maxscream
#define PL_maxscream		(*Perl_Tmaxscream_ptr(aTHX))
#undef  PL_modcount
#define PL_modcount		(*Perl_Tmodcount_ptr(aTHX))
#undef  PL_na
#define PL_na			(*Perl_Tna_ptr(aTHX))
#undef  PL_nrs
#define PL_nrs			(*Perl_Tnrs_ptr(aTHX))
#undef  PL_ofs_sv
#define PL_ofs_sv		(*Perl_Tofs_sv_ptr(aTHX))
#undef  PL_op
#define PL_op			(*Perl_Top_ptr(aTHX))
#undef  PL_opsave
#define PL_opsave		(*Perl_Topsave_ptr(aTHX))
#undef  PL_peepp
#define PL_peepp		(*Perl_Tpeepp_ptr(aTHX))
#undef  PL_protect
#define PL_protect		(*Perl_Tprotect_ptr(aTHX))
#undef  PL_reg_call_cc
#define PL_reg_call_cc		(*Perl_Treg_call_cc_ptr(aTHX))
#undef  PL_reg_curpm
#define PL_reg_curpm		(*Perl_Treg_curpm_ptr(aTHX))
#undef  PL_reg_eval_set
#define PL_reg_eval_set		(*Perl_Treg_eval_set_ptr(aTHX))
#undef  PL_reg_flags
#define PL_reg_flags		(*Perl_Treg_flags_ptr(aTHX))
#undef  PL_reg_ganch
#define PL_reg_ganch		(*Perl_Treg_ganch_ptr(aTHX))
#undef  PL_reg_leftiter
#define PL_reg_leftiter		(*Perl_Treg_leftiter_ptr(aTHX))
#undef  PL_reg_magic
#define PL_reg_magic		(*Perl_Treg_magic_ptr(aTHX))
#undef  PL_reg_match_utf8
#define PL_reg_match_utf8	(*Perl_Treg_match_utf8_ptr(aTHX))
#undef  PL_reg_maxiter
#define PL_reg_maxiter		(*Perl_Treg_maxiter_ptr(aTHX))
#undef  PL_reg_oldcurpm
#define PL_reg_oldcurpm		(*Perl_Treg_oldcurpm_ptr(aTHX))
#undef  PL_reg_oldpos
#define PL_reg_oldpos		(*Perl_Treg_oldpos_ptr(aTHX))
#undef  PL_reg_oldsaved
#define PL_reg_oldsaved		(*Perl_Treg_oldsaved_ptr(aTHX))
#undef  PL_reg_oldsavedlen
#define PL_reg_oldsavedlen	(*Perl_Treg_oldsavedlen_ptr(aTHX))
#undef  PL_reg_poscache
#define PL_reg_poscache		(*Perl_Treg_poscache_ptr(aTHX))
#undef  PL_reg_poscache_size
#define PL_reg_poscache_size	(*Perl_Treg_poscache_size_ptr(aTHX))
#undef  PL_reg_re
#define PL_reg_re		(*Perl_Treg_re_ptr(aTHX))
#undef  PL_reg_start_tmp
#define PL_reg_start_tmp	(*Perl_Treg_start_tmp_ptr(aTHX))
#undef  PL_reg_start_tmpl
#define PL_reg_start_tmpl	(*Perl_Treg_start_tmpl_ptr(aTHX))
#undef  PL_reg_starttry
#define PL_reg_starttry		(*Perl_Treg_starttry_ptr(aTHX))
#undef  PL_reg_sv
#define PL_reg_sv		(*Perl_Treg_sv_ptr(aTHX))
#undef  PL_reg_whilem_seen
#define PL_reg_whilem_seen	(*Perl_Treg_whilem_seen_ptr(aTHX))
#undef  PL_regbol
#define PL_regbol		(*Perl_Tregbol_ptr(aTHX))
#undef  PL_regcc
#define PL_regcc		(*Perl_Tregcc_ptr(aTHX))
#undef  PL_regcode
#define PL_regcode		(*Perl_Tregcode_ptr(aTHX))
#undef  PL_regcomp_parse
#define PL_regcomp_parse	(*Perl_Tregcomp_parse_ptr(aTHX))
#undef  PL_regcomp_rx
#define PL_regcomp_rx		(*Perl_Tregcomp_rx_ptr(aTHX))
#undef  PL_regcompat1
#define PL_regcompat1		(*Perl_Tregcompat1_ptr(aTHX))
#undef  PL_regcompp
#define PL_regcompp		(*Perl_Tregcompp_ptr(aTHX))
#undef  PL_regdata
#define PL_regdata		(*Perl_Tregdata_ptr(aTHX))
#undef  PL_regdummy
#define PL_regdummy		(*Perl_Tregdummy_ptr(aTHX))
#undef  PL_regendp
#define PL_regendp		(*Perl_Tregendp_ptr(aTHX))
#undef  PL_regeol
#define PL_regeol		(*Perl_Tregeol_ptr(aTHX))
#undef  PL_regexecp
#define PL_regexecp		(*Perl_Tregexecp_ptr(aTHX))
#undef  PL_regflags
#define PL_regflags		(*Perl_Tregflags_ptr(aTHX))
#undef  PL_regfree
#define PL_regfree		(*Perl_Tregfree_ptr(aTHX))
#undef  PL_regindent
#define PL_regindent		(*Perl_Tregindent_ptr(aTHX))
#undef  PL_reginput
#define PL_reginput		(*Perl_Treginput_ptr(aTHX))
#undef  PL_regint_start
#define PL_regint_start		(*Perl_Tregint_start_ptr(aTHX))
#undef  PL_regint_string
#define PL_regint_string	(*Perl_Tregint_string_ptr(aTHX))
#undef  PL_reginterp_cnt
#define PL_reginterp_cnt	(*Perl_Treginterp_cnt_ptr(aTHX))
#undef  PL_reglastcloseparen
#define PL_reglastcloseparen	(*Perl_Treglastcloseparen_ptr(aTHX))
#undef  PL_reglastparen
#define PL_reglastparen		(*Perl_Treglastparen_ptr(aTHX))
#undef  PL_regnarrate
#define PL_regnarrate		(*Perl_Tregnarrate_ptr(aTHX))
#undef  PL_regnaughty
#define PL_regnaughty		(*Perl_Tregnaughty_ptr(aTHX))
#undef  PL_regnpar
#define PL_regnpar		(*Perl_Tregnpar_ptr(aTHX))
#undef  PL_regprecomp
#define PL_regprecomp		(*Perl_Tregprecomp_ptr(aTHX))
#undef  PL_regprogram
#define PL_regprogram		(*Perl_Tregprogram_ptr(aTHX))
#undef  PL_regsawback
#define PL_regsawback		(*Perl_Tregsawback_ptr(aTHX))
#undef  PL_regseen
#define PL_regseen		(*Perl_Tregseen_ptr(aTHX))
#undef  PL_regsize
#define PL_regsize		(*Perl_Tregsize_ptr(aTHX))
#undef  PL_regstartp
#define PL_regstartp		(*Perl_Tregstartp_ptr(aTHX))
#undef  PL_regtill
#define PL_regtill		(*Perl_Tregtill_ptr(aTHX))
#undef  PL_regxend
#define PL_regxend		(*Perl_Tregxend_ptr(aTHX))
#undef  PL_restartop
#define PL_restartop		(*Perl_Trestartop_ptr(aTHX))
#undef  PL_retstack
#define PL_retstack		(*Perl_Tretstack_ptr(aTHX))
#undef  PL_retstack_ix
#define PL_retstack_ix		(*Perl_Tretstack_ix_ptr(aTHX))
#undef  PL_retstack_max
#define PL_retstack_max		(*Perl_Tretstack_max_ptr(aTHX))
#undef  PL_rs
#define PL_rs			(*Perl_Trs_ptr(aTHX))
#undef  PL_savestack
#define PL_savestack		(*Perl_Tsavestack_ptr(aTHX))
#undef  PL_savestack_ix
#define PL_savestack_ix		(*Perl_Tsavestack_ix_ptr(aTHX))
#undef  PL_savestack_max
#define PL_savestack_max	(*Perl_Tsavestack_max_ptr(aTHX))
#undef  PL_scopestack
#define PL_scopestack		(*Perl_Tscopestack_ptr(aTHX))
#undef  PL_scopestack_ix
#define PL_scopestack_ix	(*Perl_Tscopestack_ix_ptr(aTHX))
#undef  PL_scopestack_max
#define PL_scopestack_max	(*Perl_Tscopestack_max_ptr(aTHX))
#undef  PL_screamfirst
#define PL_screamfirst		(*Perl_Tscreamfirst_ptr(aTHX))
#undef  PL_screamnext
#define PL_screamnext		(*Perl_Tscreamnext_ptr(aTHX))
#undef  PL_secondgv
#define PL_secondgv		(*Perl_Tsecondgv_ptr(aTHX))
#undef  PL_seen_evals
#define PL_seen_evals		(*Perl_Tseen_evals_ptr(aTHX))
#undef  PL_seen_zerolen
#define PL_seen_zerolen		(*Perl_Tseen_zerolen_ptr(aTHX))
#undef  PL_sortcop
#define PL_sortcop		(*Perl_Tsortcop_ptr(aTHX))
#undef  PL_sortcxix
#define PL_sortcxix		(*Perl_Tsortcxix_ptr(aTHX))
#undef  PL_sortstash
#define PL_sortstash		(*Perl_Tsortstash_ptr(aTHX))
#undef  PL_stack_base
#define PL_stack_base		(*Perl_Tstack_base_ptr(aTHX))
#undef  PL_stack_max
#define PL_stack_max		(*Perl_Tstack_max_ptr(aTHX))
#undef  PL_stack_sp
#define PL_stack_sp		(*Perl_Tstack_sp_ptr(aTHX))
#undef  PL_start_env
#define PL_start_env		(*Perl_Tstart_env_ptr(aTHX))
#undef  PL_statbuf
#define PL_statbuf		(*Perl_Tstatbuf_ptr(aTHX))
#undef  PL_statcache
#define PL_statcache		(*Perl_Tstatcache_ptr(aTHX))
#undef  PL_statgv
#define PL_statgv		(*Perl_Tstatgv_ptr(aTHX))
#undef  PL_statname
#define PL_statname		(*Perl_Tstatname_ptr(aTHX))
#undef  PL_tainted
#define PL_tainted		(*Perl_Ttainted_ptr(aTHX))
#undef  PL_timesbuf
#define PL_timesbuf		(*Perl_Ttimesbuf_ptr(aTHX))
#undef  PL_tmps_floor
#define PL_tmps_floor		(*Perl_Ttmps_floor_ptr(aTHX))
#undef  PL_tmps_ix
#define PL_tmps_ix		(*Perl_Ttmps_ix_ptr(aTHX))
#undef  PL_tmps_max
#define PL_tmps_max		(*Perl_Ttmps_max_ptr(aTHX))
#undef  PL_tmps_stack
#define PL_tmps_stack		(*Perl_Ttmps_stack_ptr(aTHX))
#undef  PL_top_env
#define PL_top_env		(*Perl_Ttop_env_ptr(aTHX))
#undef  PL_toptarget
#define PL_toptarget		(*Perl_Ttoptarget_ptr(aTHX))
#undef  PL_watchaddr
#define PL_watchaddr		(*Perl_Twatchaddr_ptr(aTHX))
#undef  PL_watchok
#define PL_watchok		(*Perl_Twatchok_ptr(aTHX))
#undef  PL_No
#define PL_No			(*Perl_GNo_ptr(NULL))
#undef  PL_Yes
#define PL_Yes			(*Perl_GYes_ptr(NULL))
#undef  PL_csighandlerp
#define PL_csighandlerp		(*Perl_Gcsighandlerp_ptr(NULL))
#undef  PL_curinterp
#define PL_curinterp		(*Perl_Gcurinterp_ptr(NULL))
#undef  PL_do_undump
#define PL_do_undump		(*Perl_Gdo_undump_ptr(NULL))
#undef  PL_dollarzero_mutex
#define PL_dollarzero_mutex	(*Perl_Gdollarzero_mutex_ptr(NULL))
#undef  PL_hexdigit
#define PL_hexdigit		(*Perl_Ghexdigit_ptr(NULL))
#undef  PL_malloc_mutex
#define PL_malloc_mutex		(*Perl_Gmalloc_mutex_ptr(NULL))
#undef  PL_op_mutex
#define PL_op_mutex		(*Perl_Gop_mutex_ptr(NULL))
#undef  PL_patleave
#define PL_patleave		(*Perl_Gpatleave_ptr(NULL))
#undef  PL_sh_path
#define PL_sh_path		(*Perl_Gsh_path_ptr(NULL))
#undef  PL_sigfpe_saved
#define PL_sigfpe_saved		(*Perl_Gsigfpe_saved_ptr(NULL))
#undef  PL_sv_placeholder
#define PL_sv_placeholder	(*Perl_Gsv_placeholder_ptr(NULL))
#undef  PL_thr_key
#define PL_thr_key		(*Perl_Gthr_key_ptr(NULL))
#undef  PL_use_safe_putenv
#define PL_use_safe_putenv	(*Perl_Guse_safe_putenv_ptr(NULL))

#endif /* !PERL_CORE */
#endif /* MULTIPLICITY */

#endif /* __perlapi_h__ */

