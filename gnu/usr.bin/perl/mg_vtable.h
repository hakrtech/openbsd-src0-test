/* -*- buffer-read-only: t -*-
 *
 *    mg_vtable.h
 * !!!!!!!   DO NOT EDIT THIS FILE   !!!!!!!
 * This file is built by regen/mg_vtable.pl.
 * Any changes made here will be lost!
 */

/* These constants should be used in preference to raw characters
 * when using magic. Note that some perl guts still assume
 * certain character properties of these constants, namely that
 * isUPPER() and toLOWER() may do useful mappings.
 */

#define PERL_MAGIC_sv             '\0' /* Special scalar variable */
#define PERL_MAGIC_arylen         '#' /* Array length ($#ary) */
#define PERL_MAGIC_rhash          '%' /* extra data for restricted hashes */
#define PERL_MAGIC_proto          '&' /* my sub prototype CV */
#define PERL_MAGIC_pos            '.' /* pos() lvalue */
#define PERL_MAGIC_symtab         ':' /* extra data for symbol tables */
#define PERL_MAGIC_backref        '<' /* for weak ref data */
#define PERL_MAGIC_arylen_p       '@' /* to move arylen out of XPVAV */
#define PERL_MAGIC_bm             'B' /* Boyer-Moore (fast string search) */
#define PERL_MAGIC_overload_table 'c' /* Holds overload table (AMT) on stash */
#define PERL_MAGIC_regdata        'D' /* Regex match position data
                                         (@+ and @- vars) */
#define PERL_MAGIC_regdatum       'd' /* Regex match position data element */
#define PERL_MAGIC_env            'E' /* %ENV hash */
#define PERL_MAGIC_envelem        'e' /* %ENV hash element */
#define PERL_MAGIC_fm             'f' /* Formline ('compiled' format) */
#define PERL_MAGIC_regex_global   'g' /* m//g target */
#define PERL_MAGIC_hints          'H' /* %^H hash */
#define PERL_MAGIC_hintselem      'h' /* %^H hash element */
#define PERL_MAGIC_isa            'I' /* @ISA array */
#define PERL_MAGIC_isaelem        'i' /* @ISA array element */
#define PERL_MAGIC_nkeys          'k' /* scalar(keys()) lvalue */
#define PERL_MAGIC_dbfile         'L' /* Debugger %_<filename */
#define PERL_MAGIC_dbline         'l' /* Debugger %_<filename element */
#define PERL_MAGIC_shared         'N' /* Shared between threads */
#define PERL_MAGIC_shared_scalar  'n' /* Shared between threads */
#define PERL_MAGIC_collxfrm       'o' /* Locale transformation */
#define PERL_MAGIC_tied           'P' /* Tied array or hash */
#define PERL_MAGIC_tiedelem       'p' /* Tied array or hash element */
#define PERL_MAGIC_tiedscalar     'q' /* Tied scalar or handle */
#define PERL_MAGIC_qr             'r' /* precompiled qr// regex */
#define PERL_MAGIC_sig            'S' /* %SIG hash */
#define PERL_MAGIC_sigelem        's' /* %SIG hash element */
#define PERL_MAGIC_taint          't' /* Taintedness */
#define PERL_MAGIC_uvar           'U' /* Available for use by extensions */
#define PERL_MAGIC_uvar_elem      'u' /* Reserved for use by extensions */
#define PERL_MAGIC_vstring        'V' /* SV was vstring literal */
#define PERL_MAGIC_vec            'v' /* vec() lvalue */
#define PERL_MAGIC_utf8           'w' /* Cached UTF-8 information */
#define PERL_MAGIC_substr         'x' /* substr() lvalue */
#define PERL_MAGIC_defelem        'y' /* Shadow "foreach" iterator variable /
                                         smart parameter vivification */
#define PERL_MAGIC_checkcall      ']' /* inlining/mutation of call to this CV */
#define PERL_MAGIC_ext            '~' /* Available for use by extensions */

enum {		/* pass one of these to get_vtbl */
    want_vtbl_arylen,
    want_vtbl_arylen_p,
    want_vtbl_backref,
    want_vtbl_checkcall,
    want_vtbl_collxfrm,
    want_vtbl_dbline,
    want_vtbl_defelem,
    want_vtbl_env,
    want_vtbl_envelem,
    want_vtbl_hints,
    want_vtbl_hintselem,
    want_vtbl_isa,
    want_vtbl_isaelem,
    want_vtbl_mglob,
    want_vtbl_nkeys,
    want_vtbl_ovrld,
    want_vtbl_pack,
    want_vtbl_packelem,
    want_vtbl_pos,
    want_vtbl_regdata,
    want_vtbl_regdatum,
    want_vtbl_regexp,
    want_vtbl_sigelem,
    want_vtbl_substr,
    want_vtbl_sv,
    want_vtbl_taint,
    want_vtbl_utf8,
    want_vtbl_uvar,
    want_vtbl_vec,
    magic_vtable_max
};

#ifdef DOINIT
EXTCONST char * const PL_magic_vtable_names[magic_vtable_max] = {
    "arylen",
    "arylen_p",
    "backref",
    "checkcall",
    "collxfrm",
    "dbline",
    "defelem",
    "env",
    "envelem",
    "hints",
    "hintselem",
    "isa",
    "isaelem",
    "mglob",
    "nkeys",
    "ovrld",
    "pack",
    "packelem",
    "pos",
    "regdata",
    "regdatum",
    "regexp",
    "sigelem",
    "substr",
    "sv",
    "taint",
    "utf8",
    "uvar",
    "vec"
};
#else
EXTCONST char * const PL_magic_vtable_names[magic_vtable_max];
#endif

/* These all need to be 0, not NULL, as NULL can be (void*)0, which is a
 * pointer to data, whereas we're assigning pointers to functions, which are
 * not the same beast. ANSI doesn't allow the assignment from one to the other.
 * (although most, but not all, compilers are prepared to do it)
 */

/* order is:
    get
    set
    len
    clear
    free
    copy
    dup
    local
*/

#ifdef DOINIT
EXT_MGVTBL PL_magic_vtables[magic_vtable_max] = {
  { (int (*)(pTHX_ SV *, MAGIC *))Perl_magic_getarylen, Perl_magic_setarylen, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, Perl_magic_cleararylen_p, Perl_magic_freearylen_p, 0, 0, 0 },
  { 0, 0, 0, 0, Perl_magic_killbackrefs, 0, 0, 0 },
  { 0, 0, 0, 0, 0, Perl_magic_copycallchecker, 0, 0 },
#ifdef USE_LOCALE_COLLATE
  { 0, Perl_magic_setcollxfrm, 0, 0, 0, 0, 0, 0 },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0 },
#endif
  { 0, Perl_magic_setdbline, 0, 0, 0, 0, 0, 0 },
  { Perl_magic_getdefelem, Perl_magic_setdefelem, 0, 0, 0, 0, 0, 0 },
  { 0, Perl_magic_set_all_env, 0, Perl_magic_clear_all_env, 0, 0, 0, 0 },
  { 0, Perl_magic_setenv, 0, Perl_magic_clearenv, 0, 0, 0, 0 },
  { 0, 0, 0, Perl_magic_clearhints, 0, 0, 0, 0 },
  { 0, Perl_magic_sethint, 0, Perl_magic_clearhint, 0, 0, 0, 0 },
  { 0, Perl_magic_setisa, 0, Perl_magic_clearisa, 0, 0, 0, 0 },
  { 0, Perl_magic_setisa, 0, 0, 0, 0, 0, 0 },
  { 0, Perl_magic_setmglob, 0, 0, 0, 0, 0, 0 },
  { Perl_magic_getnkeys, Perl_magic_setnkeys, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, Perl_magic_freeovrld, 0, 0, 0 },
  { 0, 0, Perl_magic_sizepack, Perl_magic_wipepack, 0, 0, 0, 0 },
  { Perl_magic_getpack, Perl_magic_setpack, 0, Perl_magic_clearpack, 0, 0, 0, 0 },
  { Perl_magic_getpos, Perl_magic_setpos, 0, 0, 0, 0, 0, 0 },
  { 0, 0, Perl_magic_regdata_cnt, 0, 0, 0, 0, 0 },
  { Perl_magic_regdatum_get, Perl_magic_regdatum_set, 0, 0, 0, 0, 0, 0 },
  { 0, Perl_magic_setregexp, 0, 0, 0, 0, 0, 0 },
#ifndef PERL_MICRO
  { Perl_magic_getsig, Perl_magic_setsig, 0, Perl_magic_clearsig, 0, 0, 0, 0 },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0 },
#endif
  { Perl_magic_getsubstr, Perl_magic_setsubstr, 0, 0, 0, 0, 0, 0 },
  { Perl_magic_get, Perl_magic_set, 0, 0, 0, 0, 0, 0 },
  { Perl_magic_gettaint, Perl_magic_settaint, 0, 0, 0, 0, 0, 0 },
  { 0, Perl_magic_setutf8, 0, 0, 0, 0, 0, 0 },
  { Perl_magic_getuvar, Perl_magic_setuvar, 0, 0, 0, 0, 0, 0 },
  { Perl_magic_getvec, Perl_magic_setvec, 0, 0, 0, 0, 0, 0 }
};
#else
EXT_MGVTBL PL_magic_vtables[magic_vtable_max];
#endif

#define want_vtbl_bm want_vtbl_regexp
#define want_vtbl_fm want_vtbl_regexp

#define PL_vtbl_arylen PL_magic_vtables[want_vtbl_arylen]
#define PL_vtbl_arylen_p PL_magic_vtables[want_vtbl_arylen_p]
#define PL_vtbl_backref PL_magic_vtables[want_vtbl_backref]
#define PL_vtbl_bm PL_magic_vtables[want_vtbl_bm]
#define PL_vtbl_checkcall PL_magic_vtables[want_vtbl_checkcall]
#define PL_vtbl_collxfrm PL_magic_vtables[want_vtbl_collxfrm]
#define PL_vtbl_dbline PL_magic_vtables[want_vtbl_dbline]
#define PL_vtbl_defelem PL_magic_vtables[want_vtbl_defelem]
#define PL_vtbl_env PL_magic_vtables[want_vtbl_env]
#define PL_vtbl_envelem PL_magic_vtables[want_vtbl_envelem]
#define PL_vtbl_fm PL_magic_vtables[want_vtbl_fm]
#define PL_vtbl_hints PL_magic_vtables[want_vtbl_hints]
#define PL_vtbl_hintselem PL_magic_vtables[want_vtbl_hintselem]
#define PL_vtbl_isa PL_magic_vtables[want_vtbl_isa]
#define PL_vtbl_isaelem PL_magic_vtables[want_vtbl_isaelem]
#define PL_vtbl_mglob PL_magic_vtables[want_vtbl_mglob]
#define PL_vtbl_nkeys PL_magic_vtables[want_vtbl_nkeys]
#define PL_vtbl_ovrld PL_magic_vtables[want_vtbl_ovrld]
#define PL_vtbl_pack PL_magic_vtables[want_vtbl_pack]
#define PL_vtbl_packelem PL_magic_vtables[want_vtbl_packelem]
#define PL_vtbl_pos PL_magic_vtables[want_vtbl_pos]
#define PL_vtbl_regdata PL_magic_vtables[want_vtbl_regdata]
#define PL_vtbl_regdatum PL_magic_vtables[want_vtbl_regdatum]
#define PL_vtbl_regexp PL_magic_vtables[want_vtbl_regexp]
#define PL_vtbl_sigelem PL_magic_vtables[want_vtbl_sigelem]
#define PL_vtbl_substr PL_magic_vtables[want_vtbl_substr]
#define PL_vtbl_sv PL_magic_vtables[want_vtbl_sv]
#define PL_vtbl_taint PL_magic_vtables[want_vtbl_taint]
#define PL_vtbl_utf8 PL_magic_vtables[want_vtbl_utf8]
#define PL_vtbl_uvar PL_magic_vtables[want_vtbl_uvar]
#define PL_vtbl_vec PL_magic_vtables[want_vtbl_vec]

/* ex: set ro: */
