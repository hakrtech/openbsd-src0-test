/*    gv.c
 *
 *    Copyright (C) 1991, 1992, 1993, 1994, 1995, 1996, 1997, 1998, 1999,
 *    2000, 2001, 2002, 2003, 2004, 2005, 2006, by Larry Wall and others
 *
 *    You may distribute under the terms of either the GNU General Public
 *    License or the Artistic License, as specified in the README file.
 *
 */

/*
 *   'Mercy!' cried Gandalf.  'If the giving of information is to be the cure
 * of your inquisitiveness, I shall spend all the rest of my days answering
 * you.  What more do you want to know?'
 *   'The names of all the stars, and of all living things, and the whole
 * history of Middle-earth and Over-heaven and of the Sundering Seas,'
 * laughed Pippin.
 */

/*
=head1 GV Functions

A GV is a structure which corresponds to to a Perl typeglob, ie *foo.
It is a structure that holds a pointer to a scalar, an array, a hash etc,
corresponding to $foo, @foo, %foo.

GVs are usually found as values in stashes (symbol table hashes) where
Perl stores its global variables.

=cut
*/

#include "EXTERN.h"
#define PERL_IN_GV_C
#include "perl.h"

const char S_autoload[] = "AUTOLOAD";
const STRLEN S_autolen = sizeof(S_autoload)-1;


#ifdef PERL_DONT_CREATE_GVSV
GV *
Perl_gv_SVadd(pTHX_ GV *gv)
{
    if (!gv || SvTYPE((SV*)gv) != SVt_PVGV)
	Perl_croak(aTHX_ "Bad symbol for scalar");
    if (!GvSV(gv))
	GvSV(gv) = NEWSV(72,0);
    return gv;
}
#endif

GV *
Perl_gv_AVadd(pTHX_ register GV *gv)
{
    if (!gv || SvTYPE((SV*)gv) != SVt_PVGV)
	Perl_croak(aTHX_ "Bad symbol for array");
    if (!GvAV(gv))
	GvAV(gv) = newAV();
    return gv;
}

GV *
Perl_gv_HVadd(pTHX_ register GV *gv)
{
    if (!gv || SvTYPE((SV*)gv) != SVt_PVGV)
	Perl_croak(aTHX_ "Bad symbol for hash");
    if (!GvHV(gv))
	GvHV(gv) = newHV();
    return gv;
}

GV *
Perl_gv_IOadd(pTHX_ register GV *gv)
{
    if (!gv || SvTYPE((SV*)gv) != SVt_PVGV)
	Perl_croak(aTHX_ "Bad symbol for filehandle");
    if (!GvIOp(gv)) {
#ifdef GV_UNIQUE_CHECK
        if (GvUNIQUE(gv)) {
            Perl_croak(aTHX_ "Bad symbol for filehandle (GV is unique)");
        }
#endif
	GvIOp(gv) = newIO();
    }
    return gv;
}

GV *
Perl_gv_fetchfile(pTHX_ const char *name)
{
    char smallbuf[256];
    char *tmpbuf;
    STRLEN tmplen;
    GV *gv;

    if (!PL_defstash)
	return Nullgv;

    tmplen = strlen(name) + 2;
    if (tmplen < sizeof smallbuf)
	tmpbuf = smallbuf;
    else
	Newx(tmpbuf, tmplen + 1, char);
    /* This is where the debugger's %{"::_<$filename"} hash is created */
    tmpbuf[0] = '_';
    tmpbuf[1] = '<';
    memcpy(tmpbuf + 2, name, tmplen - 1);
    gv = *(GV**)hv_fetch(PL_defstash, tmpbuf, tmplen, TRUE);
    if (!isGV(gv)) {
	gv_init(gv, PL_defstash, tmpbuf, tmplen, FALSE);
#ifdef PERL_DONT_CREATE_GVSV
	GvSV(gv) = newSVpvn(name, tmplen - 2);
#else
	sv_setpvn(GvSV(gv), name, tmplen - 2);
#endif
	if (PERLDB_LINE)
	    hv_magic(GvHVn(gv_AVadd(gv)), Nullgv, PERL_MAGIC_dbfile);
    }
    if (tmpbuf != smallbuf)
	Safefree(tmpbuf);
    return gv;
}

void
Perl_gv_init(pTHX_ GV *gv, HV *stash, const char *name, STRLEN len, int multi)
{
    register GP *gp;
    const bool doproto = SvTYPE(gv) > SVt_NULL;
    const char * const proto = (doproto && SvPOK(gv)) ? SvPVX_const(gv) : NULL;

    sv_upgrade((SV*)gv, SVt_PVGV);
    if (SvLEN(gv)) {
	if (proto) {
	    SvPV_set(gv, NULL);
	    SvLEN_set(gv, 0);
	    SvPOK_off(gv);
	} else
	    Safefree(SvPVX_mutable(gv));
    }
    Newxz(gp, 1, GP);
    GvGP(gv) = gp_ref(gp);
#ifdef PERL_DONT_CREATE_GVSV
    GvSV(gv) = 0;
#else
    GvSV(gv) = NEWSV(72,0);
#endif
    GvLINE(gv) = CopLINE(PL_curcop);
    /* XXX Ideally this cast would be replaced with a change to const char*
       in the struct.  */
    GvFILE(gv) = CopFILE(PL_curcop) ? CopFILE(PL_curcop) : (char *) "";
    GvCVGEN(gv) = 0;
    GvEGV(gv) = gv;
    sv_magic((SV*)gv, (SV*)gv, PERL_MAGIC_glob, Nullch, 0);
    GvSTASH(gv) = (HV*)SvREFCNT_inc(stash);
    GvNAME(gv) = savepvn(name, len);
    GvNAMELEN(gv) = len;
    if (multi || doproto)              /* doproto means it _was_ mentioned */
	GvMULTI_on(gv);
    if (doproto) {			/* Replicate part of newSUB here. */
	SvIOK_off(gv);
	ENTER;
	/* XXX unsafe for threads if eval_owner isn't held */
	(void) start_subparse(0,0);	/* Create empty CV in compcv. */
	GvCV(gv) = PL_compcv;
	LEAVE;

	PL_sub_generation++;
	CvGV(GvCV(gv)) = gv;
	CvFILE_set_from_cop(GvCV(gv), PL_curcop);
	CvSTASH(GvCV(gv)) = PL_curstash;
#ifdef USE_5005THREADS
	CvOWNER(GvCV(gv)) = 0;
	if (!CvMUTEXP(GvCV(gv))) {
	    New(666, CvMUTEXP(GvCV(gv)), 1, perl_mutex);
	    MUTEX_INIT(CvMUTEXP(GvCV(gv)));
	}
#endif /* USE_5005THREADS */
	if (proto) {
	    sv_setpv((SV*)GvCV(gv), proto);
	    Safefree(proto);
	}
    }
}

STATIC void
S_gv_init_sv(pTHX_ GV *gv, I32 sv_type)
{
    switch (sv_type) {
    case SVt_PVIO:
	(void)GvIOn(gv);
	break;
    case SVt_PVAV:
	(void)GvAVn(gv);
	break;
    case SVt_PVHV:
	(void)GvHVn(gv);
	break;
#ifdef PERL_DONT_CREATE_GVSV
    case SVt_NULL:
    case SVt_PVCV:
    case SVt_PVFM:
	break;
    default:
	(void)GvSVn(gv);
#endif
    }
}

/*
=for apidoc gv_fetchmeth

Returns the glob with the given C<name> and a defined subroutine or
C<NULL>.  The glob lives in the given C<stash>, or in the stashes
accessible via @ISA and UNIVERSAL::.

The argument C<level> should be either 0 or -1.  If C<level==0>, as a
side-effect creates a glob with the given C<name> in the given C<stash>
which in the case of success contains an alias for the subroutine, and sets
up caching info for this glob.  Similarly for all the searched stashes.

This function grants C<"SUPER"> token as a postfix of the stash name. The
GV returned from C<gv_fetchmeth> may be a method cache entry, which is not
visible to Perl code.  So when calling C<call_sv>, you should not use
the GV directly; instead, you should use the method's CV, which can be
obtained from the GV with the C<GvCV> macro.

=cut
*/

GV *
Perl_gv_fetchmeth(pTHX_ HV *stash, const char *name, STRLEN len, I32 level)
{
    AV* av;
    GV* topgv;
    GV* gv;
    GV** gvp;
    CV* cv;
    const char *hvname;

    /* UNIVERSAL methods should be callable without a stash */
    if (!stash) {
	level = -1;  /* probably appropriate */
	if(!(stash = gv_stashpvn("UNIVERSAL", 9, FALSE)))
	    return 0;
    }

    hvname = HvNAME_get(stash);
    if (!hvname)
      Perl_croak(aTHX_
		 "Can't use anonymous symbol table for method lookup");

    if ((level > 100) || (level < -100))
	Perl_croak(aTHX_ "Recursive inheritance detected while looking for method '%s' in package '%s'",
	      name, hvname);

    DEBUG_o( Perl_deb(aTHX_ "Looking for method %s in package %s\n",name,hvname) );

    gvp = (GV**)hv_fetch(stash, name, len, (level >= 0));
    if (!gvp)
	topgv = Nullgv;
    else {
	topgv = *gvp;
	if (SvTYPE(topgv) != SVt_PVGV)
	    gv_init(topgv, stash, name, len, TRUE);
	if ((cv = GvCV(topgv))) {
	    /* If genuine method or valid cache entry, use it */
	    if (!GvCVGEN(topgv) || GvCVGEN(topgv) == PL_sub_generation)
		return topgv;
	    /* Stale cached entry: junk it */
	    SvREFCNT_dec(cv);
	    GvCV(topgv) = cv = Nullcv;
	    GvCVGEN(topgv) = 0;
	}
	else if (GvCVGEN(topgv) == PL_sub_generation)
	    return 0;  /* cache indicates sub doesn't exist */
    }

    gvp = (GV**)hv_fetch(stash, "ISA", 3, FALSE);
    av = (gvp && (gv = *gvp) && gv != (GV*)&PL_sv_undef) ? GvAV(gv) : Nullav;

    /* create and re-create @.*::SUPER::ISA on demand */
    if (!av || !SvMAGIC(av)) {
	STRLEN packlen = strlen(hvname);

	if (packlen >= 7 && strEQ(hvname + packlen - 7, "::SUPER")) {
	    HV* basestash;

	    packlen -= 7;
	    basestash = gv_stashpvn(hvname, packlen, TRUE);
	    gvp = (GV**)hv_fetch(basestash, "ISA", 3, FALSE);
	    if (gvp && (gv = *gvp) != (GV*)&PL_sv_undef && (av = GvAV(gv))) {
		gvp = (GV**)hv_fetch(stash, "ISA", 3, TRUE);
		if (!gvp || !(gv = *gvp))
		    Perl_croak(aTHX_ "Cannot create %s::ISA", hvname);
		if (SvTYPE(gv) != SVt_PVGV)
		    gv_init(gv, stash, "ISA", 3, TRUE);
		SvREFCNT_dec(GvAV(gv));
		GvAV(gv) = (AV*)SvREFCNT_inc(av);
	    }
	}
    }

    if (av) {
	SV** svp = AvARRAY(av);
	/* NOTE: No support for tied ISA */
	I32 items = AvFILLp(av) + 1;
	while (items--) {
	    SV* const sv = *svp++;
	    HV* const basestash = gv_stashsv(sv, FALSE);
	    if (!basestash) {
		if (ckWARN(WARN_MISC))
		    Perl_warner(aTHX_ packWARN(WARN_MISC), "Can't locate package %"SVf" for @%s::ISA",
			sv, hvname);
		continue;
	    }
	    gv = gv_fetchmeth(basestash, name, len,
			      (level >= 0) ? level + 1 : level - 1);
	    if (gv)
		goto gotcha;
	}
    }

    /* if at top level, try UNIVERSAL */

    if (level == 0 || level == -1) {
	HV* const lastchance = gv_stashpvn("UNIVERSAL", 9, FALSE);

	if (lastchance) {
	    if ((gv = gv_fetchmeth(lastchance, name, len,
				  (level >= 0) ? level + 1 : level - 1)))
	    {
	  gotcha:
		/*
		 * Cache method in topgv if:
		 *  1. topgv has no synonyms (else inheritance crosses wires)
		 *  2. method isn't a stub (else AUTOLOAD fails spectacularly)
		 */
		if (topgv &&
		    GvREFCNT(topgv) == 1 &&
		    (cv = GvCV(gv)) &&
		    (CvROOT(cv) || CvXSUB(cv)))
		{
		    if ((cv = GvCV(topgv)))
			SvREFCNT_dec(cv);
		    GvCV(topgv) = (CV*)SvREFCNT_inc(GvCV(gv));
		    GvCVGEN(topgv) = PL_sub_generation;
		}
		return gv;
	    }
	    else if (topgv && GvREFCNT(topgv) == 1) {
		/* cache the fact that the method is not defined */
		GvCVGEN(topgv) = PL_sub_generation;
	    }
	}
    }

    return 0;
}

/*
=for apidoc gv_fetchmeth_autoload

Same as gv_fetchmeth(), but looks for autoloaded subroutines too.
Returns a glob for the subroutine.

For an autoloaded subroutine without a GV, will create a GV even
if C<level < 0>.  For an autoloaded subroutine without a stub, GvCV()
of the result may be zero.

=cut
*/

GV *
Perl_gv_fetchmeth_autoload(pTHX_ HV *stash, const char *name, STRLEN len, I32 level)
{
    GV *gv = gv_fetchmeth(stash, name, len, level);

    if (!gv) {
	CV *cv;
	GV **gvp;

	if (!stash)
	    return Nullgv;	/* UNIVERSAL::AUTOLOAD could cause trouble */
	if (len == S_autolen && strnEQ(name, S_autoload, S_autolen))
	    return Nullgv;
	if (!(gv = gv_fetchmeth(stash, S_autoload, S_autolen, FALSE)))
	    return Nullgv;
	cv = GvCV(gv);
	if (!(CvROOT(cv) || CvXSUB(cv)))
	    return Nullgv;
	/* Have an autoload */
	if (level < 0)	/* Cannot do without a stub */
	    gv_fetchmeth(stash, name, len, 0);
	gvp = (GV**)hv_fetch(stash, name, len, (level >= 0));
	if (!gvp)
	    return Nullgv;
	return *gvp;
    }
    return gv;
}

/*
=for apidoc gv_fetchmethod

See L<gv_fetchmethod_autoload>.

=cut
*/

GV *
Perl_gv_fetchmethod(pTHX_ HV *stash, const char *name)
{
    return gv_fetchmethod_autoload(stash, name, TRUE);
}

/*
=for apidoc gv_fetchmethod_autoload

Returns the glob which contains the subroutine to call to invoke the method
on the C<stash>.  In fact in the presence of autoloading this may be the
glob for "AUTOLOAD".  In this case the corresponding variable $AUTOLOAD is
already setup.

The third parameter of C<gv_fetchmethod_autoload> determines whether
AUTOLOAD lookup is performed if the given method is not present: non-zero
means yes, look for AUTOLOAD; zero means no, don't look for AUTOLOAD.
Calling C<gv_fetchmethod> is equivalent to calling C<gv_fetchmethod_autoload>
with a non-zero C<autoload> parameter.

These functions grant C<"SUPER"> token as a prefix of the method name. Note
that if you want to keep the returned glob for a long time, you need to
check for it being "AUTOLOAD", since at the later time the call may load a
different subroutine due to $AUTOLOAD changing its value. Use the glob
created via a side effect to do this.

These functions have the same side-effects and as C<gv_fetchmeth> with
C<level==0>.  C<name> should be writable if contains C<':'> or C<'
''>. The warning against passing the GV returned by C<gv_fetchmeth> to
C<call_sv> apply equally to these functions.

=cut
*/

GV *
Perl_gv_fetchmethod_autoload(pTHX_ HV *stash, const char *name, I32 autoload)
{
    register const char *nend;
    const char *nsplit = 0;
    GV* gv;
    HV* ostash = stash;

    if (stash && SvTYPE(stash) < SVt_PVHV)
	stash = Nullhv;

    for (nend = name; *nend; nend++) {
	if (*nend == '\'')
	    nsplit = nend;
	else if (*nend == ':' && *(nend + 1) == ':')
	    nsplit = ++nend;
    }
    if (nsplit) {
	const char * const origname = name;
	name = nsplit + 1;
	if (*nsplit == ':')
	    --nsplit;
	if ((nsplit - origname) == 5 && strnEQ(origname, "SUPER", 5)) {
	    /* ->SUPER::method should really be looked up in original stash */
	    SV *tmpstr = sv_2mortal(Perl_newSVpvf(aTHX_ "%s::SUPER",
						  CopSTASHPV(PL_curcop)));
	    /* __PACKAGE__::SUPER stash should be autovivified */
	    stash = gv_stashpvn(SvPVX_const(tmpstr), SvCUR(tmpstr), TRUE);
	    DEBUG_o( Perl_deb(aTHX_ "Treating %s as %s::%s\n",
			 origname, HvNAME_get(stash), name) );
	}
	else {
            /* don't autovifify if ->NoSuchStash::method */
            stash = gv_stashpvn(origname, nsplit - origname, FALSE);

	    /* however, explicit calls to Pkg::SUPER::method may
	       happen, and may require autovivification to work */
	    if (!stash && (nsplit - origname) >= 7 &&
		strnEQ(nsplit - 7, "::SUPER", 7) &&
		gv_stashpvn(origname, nsplit - origname - 7, FALSE))
	      stash = gv_stashpvn(origname, nsplit - origname, TRUE);
	}
	ostash = stash;
    }

    gv = gv_fetchmeth(stash, name, nend - name, 0);
    if (!gv) {
	if (strEQ(name,"import") || strEQ(name,"unimport"))
	    gv = (GV*)&PL_sv_yes;
	else if (autoload)
	    gv = gv_autoload4(ostash, name, nend - name, TRUE);
    }
    else if (autoload) {
	CV* const cv = GvCV(gv);
	if (!CvROOT(cv) && !CvXSUB(cv)) {
	    GV* stubgv;
	    GV* autogv;

	    if (CvANON(cv))
		stubgv = gv;
	    else {
		stubgv = CvGV(cv);
		if (GvCV(stubgv) != cv)		/* orphaned import */
		    stubgv = gv;
	    }
	    autogv = gv_autoload4(GvSTASH(stubgv),
				  GvNAME(stubgv), GvNAMELEN(stubgv), TRUE);
	    if (autogv)
		gv = autogv;
	}
    }

    return gv;
}

GV*
Perl_gv_autoload4(pTHX_ HV *stash, const char *name, STRLEN len, I32 method)
{
    GV* gv;
    CV* cv;
    HV* varstash;
    GV* vargv;
    SV* varsv;
    const char *packname = "";

    if (len == S_autolen && strnEQ(name, S_autoload, S_autolen))
	return Nullgv;
    if (stash) {
	if (SvTYPE(stash) < SVt_PVHV) {
	    packname = SvPV_nolen_const((SV*)stash);
	    stash = Nullhv;
	}
	else {
	    packname = HvNAME_get(stash);
	}
    }
    if (!(gv = gv_fetchmeth(stash, S_autoload, S_autolen, FALSE)))
	return Nullgv;
    cv = GvCV(gv);

    if (!(CvROOT(cv) || CvXSUB(cv)))
	return Nullgv;

    /*
     * Inheriting AUTOLOAD for non-methods works ... for now.
     */
    if (!method && (GvCVGEN(gv) || GvSTASH(gv) != stash)
	&& ckWARN2(WARN_DEPRECATED, WARN_SYNTAX)
    )
	Perl_warner(aTHX_ packWARN2(WARN_DEPRECATED, WARN_SYNTAX),
	  "Use of inherited AUTOLOAD for non-method %s::%.*s() is deprecated",
	     packname, (int)len, name);

#ifndef USE_5005THREADS
    if (CvXSUB(cv)) {
        /* rather than lookup/init $AUTOLOAD here
         * only to have the XSUB do another lookup for $AUTOLOAD
         * and split that value on the last '::',
         * pass along the same data via some unused fields in the CV
         */
        CvSTASH(cv) = stash;
        SvPV_set(cv, (char *)name); /* cast to lose constness warning */
        SvCUR_set(cv, len);
        return gv;
    }
#endif

    /*
     * Given &FOO::AUTOLOAD, set $FOO::AUTOLOAD to desired function name.
     * The subroutine's original name may not be "AUTOLOAD", so we don't
     * use that, but for lack of anything better we will use the sub's
     * original package to look up $AUTOLOAD.
     */
    varstash = GvSTASH(CvGV(cv));
    vargv = *(GV**)hv_fetch(varstash, S_autoload, S_autolen, TRUE);
    ENTER;

#ifdef USE_5005THREADS
    sv_lock((SV *)varstash);
#endif
    if (!isGV(vargv)) {
	gv_init(vargv, varstash, S_autoload, S_autolen, FALSE);
#ifdef PERL_DONT_CREATE_GVSV
	GvSV(vargv) = NEWSV(72,0);
#endif
    }
    LEAVE;
    varsv = GvSVn(vargv);
#ifdef USE_5005THREADS
    sv_lock(varsv);
#endif
    sv_setpv(varsv, packname);
    sv_catpvn(varsv, "::", 2);
    sv_catpvn(varsv, name, len);
    SvTAINTED_off(varsv);
    return gv;
}

/* The "gv" parameter should be the glob known to Perl code as *!
 * The scalar must already have been magicalized.
 */
STATIC void
S_require_errno(pTHX_ GV *gv)
{
    HV* stash = gv_stashpvn("Errno",5,FALSE);

    if (!stash || !(gv_fetchmethod(stash, "TIEHASH"))) {
	dSP;
	PUTBACK;
	ENTER;
	save_scalar(gv); /* keep the value of $! */
        Perl_load_module(aTHX_ PERL_LOADMOD_NOIMPORT,
                         newSVpvn("Errno",5), Nullsv);
	LEAVE;
	SPAGAIN;
	stash = gv_stashpvn("Errno",5,FALSE);
	if (!stash || !(gv_fetchmethod(stash, "TIEHASH")))
	    Perl_croak(aTHX_ "Can't use %%! because Errno.pm is not available");
    }
}

/*
=for apidoc gv_stashpv

Returns a pointer to the stash for a specified package.  C<name> should
be a valid UTF-8 string and must be null-terminated.  If C<create> is set
then the package will be created if it does not already exist.  If C<create>
is not set and the package does not exist then NULL is returned.

=cut
*/

HV*
Perl_gv_stashpv(pTHX_ const char *name, I32 create)
{
    return gv_stashpvn(name, strlen(name), create);
}

/*
=for apidoc gv_stashpvn

Returns a pointer to the stash for a specified package.  C<name> should
be a valid UTF-8 string.  The C<namelen> parameter indicates the length of
the C<name>, in bytes.  If C<create> is set then the package will be
created if it does not already exist.  If C<create> is not set and the
package does not exist then NULL is returned.

=cut
*/

HV*
Perl_gv_stashpvn(pTHX_ const char *name, U32 namelen, I32 create)
{
    char smallbuf[256];
    char *tmpbuf;
    HV *stash;
    GV *tmpgv;

    if (namelen + 3 < sizeof smallbuf)
	tmpbuf = smallbuf;
    else
	Newx(tmpbuf, namelen + 3, char);
    Copy(name,tmpbuf,namelen,char);
    tmpbuf[namelen++] = ':';
    tmpbuf[namelen++] = ':';
    tmpbuf[namelen] = '\0';
    tmpgv = gv_fetchpv(tmpbuf, create, SVt_PVHV);
    if (tmpbuf != smallbuf)
	Safefree(tmpbuf);
    if (!tmpgv)
	return 0;
    if (!GvHV(tmpgv))
	GvHV(tmpgv) = newHV();
    stash = GvHV(tmpgv);
    if (!HvNAME_get(stash))
	hv_name_set(stash, name, namelen, 0);
    return stash;
}

/*
=for apidoc gv_stashsv

Returns a pointer to the stash for a specified package, which must be a
valid UTF-8 string.  See C<gv_stashpv>.

=cut
*/

HV*
Perl_gv_stashsv(pTHX_ SV *sv, I32 create)
{
    STRLEN len;
    const char * const ptr = SvPV_const(sv,len);
    return gv_stashpvn(ptr, len, create);
}


GV *
Perl_gv_fetchpv(pTHX_ const char *nambeg, I32 add, I32 sv_type)
{
    register const char *name = nambeg;
    register GV *gv = 0;
    GV**gvp;
    I32 len;
    register const char *namend;
    HV *stash = 0;

    if (*name == '*' && isALPHA(name[1])) /* accidental stringify on a GV? */
	name++;

    for (namend = name; *namend; namend++) {
	if ((*namend == ':' && namend[1] == ':')
	    || (*namend == '\'' && namend[1]))
	{
	    if (!stash)
		stash = PL_defstash;
	    if (!stash || !SvREFCNT(stash)) /* symbol table under destruction */
		return Nullgv;

	    len = namend - name;
	    if (len > 0) {
		char smallbuf[256];
		char *tmpbuf;

		if (len + 3 < sizeof (smallbuf))
		    tmpbuf = smallbuf;
		else
		    Newx(tmpbuf, len+3, char);
		Copy(name, tmpbuf, len, char);
		tmpbuf[len++] = ':';
		tmpbuf[len++] = ':';
		tmpbuf[len] = '\0';
		gvp = (GV**)hv_fetch(stash,tmpbuf,len,add);
		gv = gvp ? *gvp : Nullgv;
		if (gv && gv != (GV*)&PL_sv_undef) {
		    if (SvTYPE(gv) != SVt_PVGV)
			gv_init(gv, stash, tmpbuf, len, (add & GV_ADDMULTI));
		    else
			GvMULTI_on(gv);
		}
		if (tmpbuf != smallbuf)
		    Safefree(tmpbuf);
		if (!gv || gv == (GV*)&PL_sv_undef)
		    return Nullgv;

		if (!(stash = GvHV(gv)))
		    stash = GvHV(gv) = newHV();

		if (!HvNAME_get(stash))
		    hv_name_set(stash, nambeg, namend - nambeg, 0);
	    }

	    if (*namend == ':')
		namend++;
	    namend++;
	    name = namend;
	    if (!*name)
		return gv ? gv : (GV*)*hv_fetch(PL_defstash, "main::", 6, TRUE);
	}
    }
    len = namend - name;

    /* No stash in name, so see how we can default */

    if (!stash) {
	if (isIDFIRST_lazy(name)) {
	    bool global = FALSE;

	    /* name is always \0 terminated, and initial \0 wouldn't return
	       true from isIDFIRST_lazy, so we know that name[1] is defined  */
	    switch (name[1]) {
	    case '\0':
		if (*name == '_')
		    global = TRUE;
		break;
	    case 'N':
		if (strEQ(name, "INC") || strEQ(name, "ENV"))
		    global = TRUE;
		break;
	    case 'I':
		if (strEQ(name, "SIG"))
		    global = TRUE;
		break;
	    case 'T':
		if (strEQ(name, "STDIN") || strEQ(name, "STDOUT") ||
		    strEQ(name, "STDERR"))
		    global = TRUE;
		break;
	    case 'R':
		if (strEQ(name, "ARGV") || strEQ(name, "ARGVOUT"))
		    global = TRUE;
		break;
	    }

	    if (global)
		stash = PL_defstash;
	    else if (IN_PERL_COMPILETIME) {
		stash = PL_curstash;
		if (add && (PL_hints & HINT_STRICT_VARS) &&
		    sv_type != SVt_PVCV &&
		    sv_type != SVt_PVGV &&
		    sv_type != SVt_PVFM &&
		    sv_type != SVt_PVIO &&
		    !(len == 1 && sv_type == SVt_PV &&
		      (*name == 'a' || *name == 'b')) )
		{
		    gvp = (GV**)hv_fetch(stash,name,len,0);
		    if (!gvp ||
			*gvp == (GV*)&PL_sv_undef ||
			SvTYPE(*gvp) != SVt_PVGV)
		    {
			stash = 0;
		    }
		    else if ((sv_type == SVt_PV   && !GvIMPORTED_SV(*gvp)) ||
			     (sv_type == SVt_PVAV && !GvIMPORTED_AV(*gvp)) ||
			     (sv_type == SVt_PVHV && !GvIMPORTED_HV(*gvp)) )
		    {
			Perl_warn(aTHX_ "Variable \"%c%s\" is not imported",
			    sv_type == SVt_PVAV ? '@' :
			    sv_type == SVt_PVHV ? '%' : '$',
			    name);
			if (GvCVu(*gvp))
			    Perl_warn(aTHX_ "\t(Did you mean &%s instead?)\n", name);
			stash = 0;
		    }
		}
	    }
	    else
		stash = CopSTASH(PL_curcop);
	}
	else
	    stash = PL_defstash;
    }

    /* By this point we should have a stash and a name */

    if (!stash) {
	if (add) {
	    SV * const err = Perl_mess(aTHX_
		 "Global symbol \"%s%s\" requires explicit package name",
		 (sv_type == SVt_PV ? "$"
		  : sv_type == SVt_PVAV ? "@"
		  : sv_type == SVt_PVHV ? "%"
		  : ""), name);
	    if (USE_UTF8_IN_NAMES)
		SvUTF8_on(err);
	    qerror(err);
	    stash = PL_nullstash;
	}
	else
	    return Nullgv;
    }

    if (!SvREFCNT(stash))	/* symbol table under destruction */
	return Nullgv;

    gvp = (GV**)hv_fetch(stash,name,len,add);
    if (!gvp || *gvp == (GV*)&PL_sv_undef)
	return Nullgv;
    gv = *gvp;
    if (SvTYPE(gv) == SVt_PVGV) {
	if (add) {
	    GvMULTI_on(gv);
	    gv_init_sv(gv, sv_type);
	    if (*name=='!' && sv_type == SVt_PVHV && len==1)
		require_errno(gv);
	}
	return gv;
    } else if (add & GV_NOINIT) {
	return gv;
    }

    /* Adding a new symbol */

    if (add & GV_ADDWARN && ckWARN_d(WARN_INTERNAL))
	Perl_warner(aTHX_ packWARN(WARN_INTERNAL), "Had to create %s unexpectedly", nambeg);
    gv_init(gv, stash, name, len, add & GV_ADDMULTI);
    gv_init_sv(gv, sv_type);

    if (isALPHA(name[0]) && ! (isLEXWARN_on ? ckWARN(WARN_ONCE)
			                    : (PL_dowarn & G_WARN_ON ) ) )
        GvMULTI_on(gv) ;

    /* set up magic where warranted */
    if (len > 1) {
#ifndef EBCDIC
	if (*name > 'V' ) {
	    /* Nothing else to do.
	       The compiler will probably turn the switch statement into a
	       branch table. Make sure we avoid even that small overhead for
	       the common case of lower case variable names.  */
	} else
#endif
	{
	    const char * const name2 = name + 1;
	    switch (*name) {
	    case 'A':
		if (strEQ(name2, "RGV")) {
		    IoFLAGS(GvIOn(gv)) |= IOf_ARGV|IOf_START;
		}
		break;
	    case 'E':
		if (strnEQ(name2, "XPORT", 5))
		    GvMULTI_on(gv);
		break;
	    case 'I':
		if (strEQ(name2, "SA")) {
		    AV* const av = GvAVn(gv);
		    GvMULTI_on(gv);
		    sv_magic((SV*)av, (SV*)gv, PERL_MAGIC_isa, Nullch, 0);
		    /* NOTE: No support for tied ISA */
		    if ((add & GV_ADDMULTI) && strEQ(nambeg,"AnyDBM_File::ISA")
			&& AvFILLp(av) == -1)
			{
			    const char *pname;
			    av_push(av, newSVpvn(pname = "NDBM_File",9));
			    gv_stashpvn(pname, 9, TRUE);
			    av_push(av, newSVpvn(pname = "DB_File",7));
			    gv_stashpvn(pname, 7, TRUE);
			    av_push(av, newSVpvn(pname = "GDBM_File",9));
			    gv_stashpvn(pname, 9, TRUE);
			    av_push(av, newSVpvn(pname = "SDBM_File",9));
			    gv_stashpvn(pname, 9, TRUE);
			    av_push(av, newSVpvn(pname = "ODBM_File",9));
			    gv_stashpvn(pname, 9, TRUE);
			}
		}
		break;
	    case 'O':
		if (strEQ(name2, "VERLOAD")) {
		    HV* const hv = GvHVn(gv);
		    GvMULTI_on(gv);
		    hv_magic(hv, Nullgv, PERL_MAGIC_overload);
		}
		break;
	    case 'S':
		if (strEQ(name2, "IG")) {
		    HV *hv;
		    I32 i;
		    if (!PL_psig_ptr) {
			Newxz(PL_psig_ptr,  SIG_SIZE, SV*);
			Newxz(PL_psig_name, SIG_SIZE, SV*);
			Newxz(PL_psig_pend, SIG_SIZE, int);
		    }
		    GvMULTI_on(gv);
		    hv = GvHVn(gv);
		    hv_magic(hv, Nullgv, PERL_MAGIC_sig);
		    for (i = 1; i < SIG_SIZE; i++) {
			SV * const * const init = hv_fetch(hv, PL_sig_name[i], strlen(PL_sig_name[i]), 1);
			if (init)
			    sv_setsv(*init, &PL_sv_undef);
			PL_psig_ptr[i] = 0;
			PL_psig_name[i] = 0;
			PL_psig_pend[i] = 0;
		    }
		}
		break;
	    case 'V':
		if (strEQ(name2, "ERSION"))
		    GvMULTI_on(gv);
		break;
	    case '\005':	/* $^ENCODING */
		if (strEQ(name2, "NCODING"))
		    goto magicalize;
		break;
	    case '\017':	/* $^OPEN */
		if (strEQ(name2, "PEN"))
		    goto magicalize;
		break;
	    case '\024':	/* ${^TAINT} */
		if (strEQ(name2, "AINT"))
		    goto ro_magicalize;
		break;
	    case '\025':	/* ${^UNICODE}, ${^UTF8LOCALE} */
		if (strEQ(name2, "NICODE"))
		    goto ro_magicalize;
		if (strEQ(name2, "TF8LOCALE"))
		    goto ro_magicalize;
		break;
	    case '\027':	/* $^WARNING_BITS */
		if (strEQ(name2, "ARNING_BITS"))
		    goto magicalize;
		break;
	    case '1':
	    case '2':
	    case '3':
	    case '4':
	    case '5':
	    case '6':
	    case '7':
	    case '8':
	    case '9':
	    {
		/* ensures variable is only digits */
		/* ${"1foo"} fails this test (and is thus writeable) */
		/* added by japhy, but borrowed from is_gv_magical */
		const char *end = name + len;
		while (--end > name) {
		    if (!isDIGIT(*end)) return gv;
		}
		goto ro_magicalize;
	    }
	    }
	}
    } else {
	/* Names of length 1.  (Or 0. But name is NUL terminated, so that will
	   be case '\0' in this switch statement (ie a default case)  */
	switch (*name) {
	case '&':
	case '`':
	case '\'':
	    if (
		sv_type == SVt_PVAV ||
		sv_type == SVt_PVHV ||
		sv_type == SVt_PVCV ||
		sv_type == SVt_PVFM ||
		sv_type == SVt_PVIO
		) { break; }
	    PL_sawampersand = TRUE;
	    goto ro_magicalize;

	case ':':
	    sv_setpv(GvSVn(gv),PL_chopset);
	    goto magicalize;

	case '?':
	    (void)SvUPGRADE(GvSVn(gv), SVt_PVLV);
	    goto magicalize;

	case '!':

	    /* If %! has been used, automatically load Errno.pm.
	       The require will itself set errno, so in order to
	       preserve its value we have to set up the magic
	       now (rather than going to magicalize)
	    */

	    sv_magic(GvSVn(gv), (SV*)gv, PERL_MAGIC_sv, name, len);

	    if (sv_type == SVt_PVHV)
		require_errno(gv);

	    break;
	case '-':
	{
	    AV* const av = GvAVn(gv);
            sv_magic((SV*)av, Nullsv, PERL_MAGIC_regdata, Nullch, 0);
	    SvREADONLY_on(av);
	    goto magicalize;
	}
	case '#':
	case '*':
	    if (sv_type == SVt_PV && ckWARN2(WARN_DEPRECATED, WARN_SYNTAX))
		Perl_warner(aTHX_ packWARN2(WARN_DEPRECATED, WARN_SYNTAX),
			    "Use of $%s is deprecated", name);
	    goto magicalize;
	case '|':
	    sv_setiv(GvSVn(gv), (IV)(IoFLAGS(GvIOp(PL_defoutgv)) & IOf_FLUSH) != 0);
	    goto magicalize;

	case '+':
	{
	    AV* const av = GvAVn(gv);
            sv_magic((SV*)av, (SV*)av, PERL_MAGIC_regdata, Nullch, 0);
	    SvREADONLY_on(av);
	    /* FALL THROUGH */
	}
	case '\023':	/* $^S */
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	ro_magicalize:
	    SvREADONLY_on(GvSVn(gv));
	    /* FALL THROUGH */
	case '[':
	case '^':
	case '~':
	case '=':
	case '%':
	case '.':
	case '(':
	case ')':
	case '<':
	case '>':
	case ',':
	case '\\':
	case '/':
	case '\001':	/* $^A */
	case '\003':	/* $^C */
	case '\004':	/* $^D */
	case '\005':	/* $^E */
	case '\006':	/* $^F */
	case '\010':	/* $^H */
	case '\011':	/* $^I, NOT \t in EBCDIC */
	case '\016':	/* $^N */
	case '\017':	/* $^O */
	case '\020':	/* $^P */
	case '\024':	/* $^T */
	case '\027':	/* $^W */
	magicalize:
	    sv_magic(GvSVn(gv), (SV*)gv, PERL_MAGIC_sv, name, len);
	    break;

	case '\014':	/* $^L */
	    sv_setpvn(GvSVn(gv),"\f",1);
	    PL_formfeed = GvSVn(gv);
	    break;
	case ';':
	    sv_setpvn(GvSVn(gv),"\034",1);
	    break;
	case ']':
	{
	    SV * const sv = GvSVn(gv);
	    (void)SvUPGRADE(sv, SVt_PVNV);
	    Perl_sv_setpvf(aTHX_ sv,
#if defined(PERL_SUBVERSION) && (PERL_SUBVERSION > 0)
			    "%8.6"
#else
			    "%5.3"
#endif
			    NVff,
			    SvNVX(PL_patchlevel));
	    SvNVX(sv) = SvNVX(PL_patchlevel);
	    SvNOK_on(sv);
	    SvREADONLY_on(sv);
	}
	break;
	case '\026':	/* $^V */
	{
	    SV * const sv = GvSVn(gv);
	    GvSV(gv) = SvREFCNT_inc(PL_patchlevel);
	    SvREFCNT_dec(sv);
	}
	break;
	}
    }
    return gv;
}

void
Perl_gv_fullname4(pTHX_ SV *sv, GV *gv, const char *prefix, bool keepmain)
{
    const char *name;
    const HV * const hv = GvSTASH(gv);
    if (!hv) {
	SvOK_off(sv);
	return;
    }
    sv_setpv(sv, prefix ? prefix : "");

    name = HvNAME_get(hv);
    if (!name)
	name = "__ANON__";

    if (keepmain || strNE(name, "main")) {
	sv_catpv(sv,name);
	sv_catpvn(sv,"::", 2);
    }
    sv_catpvn(sv,GvNAME(gv),GvNAMELEN(gv));
}

void
Perl_gv_fullname3(pTHX_ SV *sv, GV *gv, const char *prefix)
{
    gv_fullname4(sv, gv, prefix, TRUE);
}

void
Perl_gv_efullname4(pTHX_ SV *sv, GV *gv, const char *prefix, bool keepmain)
{
    const GV * const egv = GvEGV(gv);
    gv_fullname4(sv, (GV *) (egv ? egv : gv), prefix, keepmain);
}

void
Perl_gv_efullname3(pTHX_ SV *sv, GV *gv, const char *prefix)
{
    gv_efullname4(sv, gv, prefix, TRUE);
}

/* compatibility with versions <= 5.003. */
void
Perl_gv_fullname(pTHX_ SV *sv, GV *gv)
{
    gv_fullname3(sv, gv, sv == (const SV*)gv ? "*" : "");
}

/* compatibility with versions <= 5.003. */
void
Perl_gv_efullname(pTHX_ SV *sv, GV *gv)
{
    gv_efullname3(sv, gv, sv == (const SV*)gv ? "*" : "");
}

IO *
Perl_newIO(pTHX)
{
    GV *iogv;
    IO * const io = (IO*)NEWSV(0,0);

    sv_upgrade((SV *)io,SVt_PVIO);
    /* This used to read SvREFCNT(io) = 1;
       It's not clear why the reference count needed an explicit reset. NWC
    */
    assert (SvREFCNT(io) == 1);
    SvOBJECT_on(io);
    /* Clear the stashcache because a new IO could overrule a package name */
    hv_clear(PL_stashcache);
    iogv = gv_fetchpv("FileHandle::", FALSE, SVt_PVHV);
    /* unless exists($main::{FileHandle}) and defined(%main::FileHandle::) */
    if (!(iogv && GvHV(iogv) && HvARRAY(GvHV(iogv))))
      iogv = gv_fetchpv("IO::Handle::", TRUE, SVt_PVHV);
    SvSTASH_set(io, (HV*)SvREFCNT_inc(GvHV(iogv)));
    return io;
}

void
Perl_gv_check(pTHX_ HV *stash)
{
    register I32 i;

    if (!HvARRAY(stash))
	return;
    for (i = 0; i <= (I32) HvMAX(stash); i++) {
        const HE *entry;
	for (entry = HvARRAY(stash)[i]; entry; entry = HeNEXT(entry)) {
            register GV *gv;
            HV *hv;
	    if (HeKEY(entry)[HeKLEN(entry)-1] == ':' &&
		(gv = (GV*)HeVAL(entry)) && isGV(gv) && (hv = GvHV(gv)))
	    {
		if (hv != PL_defstash && hv != stash)
		     gv_check(hv);              /* nested package */
	    }
	    else if (isALPHA(*HeKEY(entry))) {
                const char *file;
		gv = (GV*)HeVAL(entry);
		if (SvTYPE(gv) != SVt_PVGV || GvMULTI(gv))
		    continue;
		file = GvFILE(gv);
		/* performance hack: if filename is absolute and it's a standard
		 * module, don't bother warning */
#ifdef MACOS_TRADITIONAL
#   define LIB_COMPONENT ":lib:"
#else
#   define LIB_COMPONENT "/lib/"
#endif
		if (file
		    && PERL_FILE_IS_ABSOLUTE(file)
		    && (instr(file, LIB_COMPONENT) || instr(file, ".pm")))
		{
		    continue;
		}
		CopLINE_set(PL_curcop, GvLINE(gv));
#ifdef USE_ITHREADS
		CopFILE(PL_curcop) = (char *)file;	/* set for warning */
#else
		CopFILEGV(PL_curcop) = gv_fetchfile(file);
#endif
		Perl_warner(aTHX_ packWARN(WARN_ONCE),
			"Name \"%s::%s\" used only once: possible typo",
			HvNAME_get(stash), GvNAME(gv));
	    }
	}
    }
}

GV *
Perl_newGVgen(pTHX_ char *pack)
{
    return gv_fetchpv(Perl_form(aTHX_ "%s::_GEN_%ld", pack, (long)PL_gensym++),
		      TRUE, SVt_PVGV);
}

/* hopefully this is only called on local symbol table entries */

GP*
Perl_gp_ref(pTHX_ GP *gp)
{
    if (!gp)
	return (GP*)NULL;
    gp->gp_refcnt++;
    if (gp->gp_cv) {
	if (gp->gp_cvgen) {
	    /* multi-named GPs cannot be used for method cache */
	    SvREFCNT_dec(gp->gp_cv);
	    gp->gp_cv = Nullcv;
	    gp->gp_cvgen = 0;
	}
	else {
	    /* Adding a new name to a subroutine invalidates method cache */
	    PL_sub_generation++;
	}
    }
    return gp;
}

void
Perl_gp_free(pTHX_ GV *gv)
{
    GP* gp;

    if (!gv || !(gp = GvGP(gv)))
	return;
    if (gp->gp_refcnt == 0) {
	if (ckWARN_d(WARN_INTERNAL))
	    Perl_warner(aTHX_ packWARN(WARN_INTERNAL),
			"Attempt to free unreferenced glob pointers"
                        pTHX__FORMAT pTHX__VALUE);
        return;
    }
    if (gp->gp_cv) {
	/* Deleting the name of a subroutine invalidates method cache */
	PL_sub_generation++;
    }
    if (--gp->gp_refcnt > 0) {
	if (gp->gp_egv == gv)
	    gp->gp_egv = 0;
        return;
    }

    if (gp->gp_sv) SvREFCNT_dec(gp->gp_sv);
    if (gp->gp_av) SvREFCNT_dec(gp->gp_av);
    /* FIXME - another reference loop GV -> symtab -> GV ?
       Somehow gp->gp_hv can end up pointing at freed garbage.  */
    if (gp->gp_hv && SvTYPE(gp->gp_hv) == SVt_PVHV) {
	/* FIXME strlen HvNAME  */
	const char *hvname = HvNAME_get(gp->gp_hv);
	if (PL_stashcache && hvname)
	    hv_delete(PL_stashcache, hvname, strlen(hvname), G_DISCARD);
	SvREFCNT_dec(gp->gp_hv);
    }
    if (gp->gp_io)   SvREFCNT_dec(gp->gp_io);
    if (gp->gp_cv)   SvREFCNT_dec(gp->gp_cv);
    if (gp->gp_form) SvREFCNT_dec(gp->gp_form);

    Safefree(gp);
    GvGP(gv) = 0;
}

int
Perl_magic_freeovrld(pTHX_ SV *sv, MAGIC *mg)
{
    AMT * const amtp = (AMT*)mg->mg_ptr;
    PERL_UNUSED_ARG(sv);

    if (amtp && AMT_AMAGIC(amtp)) {
	int i;
	for (i = 1; i < NofAMmeth; i++) {
	    CV * const cv = amtp->table[i];
	    if (cv != Nullcv) {
		SvREFCNT_dec((SV *) cv);
		amtp->table[i] = Nullcv;
	    }
	}
    }
 return 0;
}

/* Updates and caches the CV's */

bool
Perl_Gv_AMupdate(pTHX_ HV *stash)
{
  MAGIC* const mg = mg_find((SV*)stash, PERL_MAGIC_overload_table);
  AMT * const amtp = (mg) ? (AMT*)mg->mg_ptr: (AMT *) NULL;
  AMT amt;

  if (mg && amtp->was_ok_am == PL_amagic_generation
      && amtp->was_ok_sub == PL_sub_generation)
      return (bool)AMT_OVERLOADED(amtp);
  sv_unmagic((SV*)stash, PERL_MAGIC_overload_table);

  DEBUG_o( Perl_deb(aTHX_ "Recalcing overload magic in package %s\n",HvNAME_get(stash)) );

  Zero(&amt,1,AMT);
  amt.was_ok_am = PL_amagic_generation;
  amt.was_ok_sub = PL_sub_generation;
  amt.fallback = AMGfallNO;
  amt.flags = 0;

  {
    int filled = 0, have_ovl = 0;
    int i, lim = 1;

    /* Work with "fallback" key, which we assume to be first in PL_AMG_names */

    /* Try to find via inheritance. */
    GV *gv = gv_fetchmeth(stash, PL_AMG_names[0], 2, -1);
    SV * const sv = gv ? GvSV(gv) : NULL;
    CV* cv;

    if (!gv)
	lim = DESTROY_amg;		/* Skip overloading entries. */
#ifdef PERL_DONT_CREATE_GVSV
    else if (!sv) {
	/* Equivalent to !SvTRUE and !SvOK  */
    }
#endif
    else if (SvTRUE(sv))
	amt.fallback=AMGfallYES;
    else if (SvOK(sv))
	amt.fallback=AMGfallNEVER;

    for (i = 1; i < lim; i++)
	amt.table[i] = Nullcv;
    for (; i < NofAMmeth; i++) {
	const char *cooky = PL_AMG_names[i];
	/* Human-readable form, for debugging: */
	const char *cp = (i >= DESTROY_amg ? cooky : AMG_id2name(i));
	const STRLEN l = strlen(cooky);

	DEBUG_o( Perl_deb(aTHX_ "Checking overloading of \"%s\" in package \"%.256s\"\n",
		     cp, HvNAME_get(stash)) );
	/* don't fill the cache while looking up!
	   Creation of inheritance stubs in intermediate packages may
	   conflict with the logic of runtime method substitution.
	   Indeed, for inheritance A -> B -> C, if C overloads "+0",
	   then we could have created stubs for "(+0" in A and C too.
	   But if B overloads "bool", we may want to use it for
	   numifying instead of C's "+0". */
	if (i >= DESTROY_amg)
	    gv = Perl_gv_fetchmeth_autoload(aTHX_ stash, cooky, l, 0);
	else				/* Autoload taken care of below */
	    gv = Perl_gv_fetchmeth(aTHX_ stash, cooky, l, -1);
        cv = 0;
        if (gv && (cv = GvCV(gv))) {
	    const char *hvname;
	    if (GvNAMELEN(CvGV(cv)) == 3 && strEQ(GvNAME(CvGV(cv)), "nil")
		&& strEQ(hvname = HvNAME_get(GvSTASH(CvGV(cv))), "overload")) {
		/* This is a hack to support autoloading..., while
		   knowing *which* methods were declared as overloaded. */
		/* GvSV contains the name of the method. */
		GV *ngv = Nullgv;
		SV *gvsv = GvSV(gv);

		DEBUG_o( Perl_deb(aTHX_ "Resolving method \"%"SVf256\
			"\" for overloaded \"%s\" in package \"%.256s\"\n",
			     GvSV(gv), cp, hvname) );
		if (!gvsv || !SvPOK(gvsv)
		    || !(ngv = gv_fetchmethod_autoload(stash, SvPVX_const(gvsv),
						       FALSE)))
		{
		    /* Can be an import stub (created by "can"). */
		    const char * const name = (gvsv && SvPOK(gvsv)) ?  SvPVX_const(gvsv) : "???";
		    Perl_croak(aTHX_ "%s method \"%.256s\" overloading \"%s\" "\
				"in package \"%.256s\"",
			       (GvCVGEN(gv) ? "Stub found while resolving"
				: "Can't resolve"),
			       name, cp, hvname);
		}
		cv = GvCV(gv = ngv);
	    }
	    DEBUG_o( Perl_deb(aTHX_ "Overloading \"%s\" in package \"%.256s\" via \"%.256s::%.256s\"\n",
			 cp, HvNAME_get(stash), HvNAME_get(GvSTASH(CvGV(cv))),
			 GvNAME(CvGV(cv))) );
	    filled = 1;
	    if (i < DESTROY_amg)
		have_ovl = 1;
	} else if (gv) {		/* Autoloaded... */
	    cv = (CV*)gv;
	    filled = 1;
	}
	amt.table[i]=(CV*)SvREFCNT_inc(cv);
    }
    if (filled) {
      AMT_AMAGIC_on(&amt);
      if (have_ovl)
	  AMT_OVERLOADED_on(&amt);
      sv_magic((SV*)stash, 0, PERL_MAGIC_overload_table,
						(char*)&amt, sizeof(AMT));
      return have_ovl;
    }
  }
  /* Here we have no table: */
  /* no_table: */
  AMT_AMAGIC_off(&amt);
  sv_magic((SV*)stash, 0, PERL_MAGIC_overload_table,
						(char*)&amt, sizeof(AMTS));
  return FALSE;
}


CV*
Perl_gv_handler(pTHX_ HV *stash, I32 id)
{
    MAGIC *mg;
    AMT *amtp;

    if (!stash || !HvNAME_get(stash))
        return Nullcv;
    mg = mg_find((SV*)stash, PERL_MAGIC_overload_table);
    if (!mg) {
      do_update:
	Gv_AMupdate(stash);
	mg = mg_find((SV*)stash, PERL_MAGIC_overload_table);
    }
    amtp = (AMT*)mg->mg_ptr;
    if ( amtp->was_ok_am != PL_amagic_generation
	 || amtp->was_ok_sub != PL_sub_generation )
	goto do_update;
    if (AMT_AMAGIC(amtp)) {
	CV * const ret = amtp->table[id];
	if (ret && isGV(ret)) {		/* Autoloading stab */
	    /* Passing it through may have resulted in a warning
	       "Inherited AUTOLOAD for a non-method deprecated", since
	       our caller is going through a function call, not a method call.
	       So return the CV for AUTOLOAD, setting $AUTOLOAD. */
	    GV * const gv = gv_fetchmethod(stash, PL_AMG_names[id]);

	    if (gv && GvCV(gv))
		return GvCV(gv);
	}
	return ret;
    }

    return Nullcv;
}


SV*
Perl_amagic_call(pTHX_ SV *left, SV *right, int method, int flags)
{
  MAGIC *mg;
  CV *cv=NULL;
  CV **cvp=NULL, **ocvp=NULL;
  AMT *amtp=NULL, *oamtp=NULL;
  int off = 0, off1, lr = 0, notfound = 0;
  int postpr = 0, force_cpy = 0;
  int assign = AMGf_assign & flags;
  const int assignshift = assign ? 1 : 0;
#ifdef DEBUGGING
  int fl=0;
#endif
  HV* stash=NULL;
  if (!(AMGf_noleft & flags) && SvAMAGIC(left)
      && (stash = SvSTASH(SvRV(left)))
      && (mg = mg_find((SV*)stash, PERL_MAGIC_overload_table))
      && (ocvp = cvp = (AMT_AMAGIC((AMT*)mg->mg_ptr)
			? (oamtp = amtp = (AMT*)mg->mg_ptr)->table
			: (CV **) NULL))
      && ((cv = cvp[off=method+assignshift])
	  || (assign && amtp->fallback > AMGfallNEVER && /* fallback to
						          * usual method */
		  (
#ifdef DEBUGGING
		   fl = 1,
#endif
		   cv = cvp[off=method])))) {
    lr = -1;			/* Call method for left argument */
  } else {
    if (cvp && amtp->fallback > AMGfallNEVER && flags & AMGf_unary) {
      int logic;

      /* look for substituted methods */
      /* In all the covered cases we should be called with assign==0. */
	 switch (method) {
	 case inc_amg:
	   force_cpy = 1;
	   if ((cv = cvp[off=add_ass_amg])
	       || ((cv = cvp[off = add_amg]) && (force_cpy = 0, postpr = 1))) {
	     right = &PL_sv_yes; lr = -1; assign = 1;
	   }
	   break;
	 case dec_amg:
	   force_cpy = 1;
	   if ((cv = cvp[off = subtr_ass_amg])
	       || ((cv = cvp[off = subtr_amg]) && (force_cpy = 0, postpr=1))) {
	     right = &PL_sv_yes; lr = -1; assign = 1;
	   }
	   break;
	 case bool__amg:
	   (void)((cv = cvp[off=numer_amg]) || (cv = cvp[off=string_amg]));
	   break;
	 case numer_amg:
	   (void)((cv = cvp[off=string_amg]) || (cv = cvp[off=bool__amg]));
	   break;
	 case string_amg:
	   (void)((cv = cvp[off=numer_amg]) || (cv = cvp[off=bool__amg]));
	   break;
         case not_amg:
           (void)((cv = cvp[off=bool__amg])
                  || (cv = cvp[off=numer_amg])
                  || (cv = cvp[off=string_amg]));
           postpr = 1;
           break;
	 case copy_amg:
	   {
	     /*
		  * SV* ref causes confusion with the interpreter variable of
		  * the same name
		  */
	     SV* const tmpRef=SvRV(left);
	     if (!SvROK(tmpRef) && SvTYPE(tmpRef) <= SVt_PVMG) {
		/*
		 * Just to be extra cautious.  Maybe in some
		 * additional cases sv_setsv is safe, too.
		 */
		SV* const newref = newSVsv(tmpRef);
		SvOBJECT_on(newref);
		SvSTASH_set(newref, (HV*)SvREFCNT_inc(SvSTASH(tmpRef)));
		return newref;
	     }
	   }
	   break;
	 case abs_amg:
	   if ((cvp[off1=lt_amg] || cvp[off1=ncmp_amg])
	       && ((cv = cvp[off=neg_amg]) || (cv = cvp[off=subtr_amg]))) {
	     SV* const nullsv=sv_2mortal(newSViv(0));
	     if (off1==lt_amg) {
	       SV* const lessp = amagic_call(left,nullsv,
				       lt_amg,AMGf_noright);
	       logic = SvTRUE(lessp);
	     } else {
	       SV* const lessp = amagic_call(left,nullsv,
				       ncmp_amg,AMGf_noright);
	       logic = (SvNV(lessp) < 0);
	     }
	     if (logic) {
	       if (off==subtr_amg) {
		 right = left;
		 left = nullsv;
		 lr = 1;
	       }
	     } else {
	       return left;
	     }
	   }
	   break;
	 case neg_amg:
	   if ((cv = cvp[off=subtr_amg])) {
	     right = left;
	     left = sv_2mortal(newSViv(0));
	     lr = 1;
	   }
	   break;
	 case int_amg:
	 case iter_amg:			/* XXXX Eventually should do to_gv. */
	     /* FAIL safe */
	     return NULL;	/* Delegate operation to standard mechanisms. */
	     break;
	 case to_sv_amg:
	 case to_av_amg:
	 case to_hv_amg:
	 case to_gv_amg:
	 case to_cv_amg:
	     /* FAIL safe */
	     return left;	/* Delegate operation to standard mechanisms. */
	     break;
	 default:
	   goto not_found;
	 }
	 if (!cv) goto not_found;
    } else if (!(AMGf_noright & flags) && SvAMAGIC(right)
	       && (stash = SvSTASH(SvRV(right)))
	       && (mg = mg_find((SV*)stash, PERL_MAGIC_overload_table))
	       && (cvp = (AMT_AMAGIC((AMT*)mg->mg_ptr)
			  ? (amtp = (AMT*)mg->mg_ptr)->table
			  : (CV **) NULL))
	       && (cv = cvp[off=method])) { /* Method for right
					     * argument found */
      lr=1;
    } else if (((ocvp && oamtp->fallback > AMGfallNEVER
		 && (cvp=ocvp) && (lr = -1))
		|| (cvp && amtp->fallback > AMGfallNEVER && (lr=1)))
	       && !(flags & AMGf_unary)) {
				/* We look for substitution for
				 * comparison operations and
				 * concatenation */
      if (method==concat_amg || method==concat_ass_amg
	  || method==repeat_amg || method==repeat_ass_amg) {
	return NULL;		/* Delegate operation to string conversion */
      }
      off = -1;
      switch (method) {
	 case lt_amg:
	 case le_amg:
	 case gt_amg:
	 case ge_amg:
	 case eq_amg:
	 case ne_amg:
	   postpr = 1; off=ncmp_amg; break;
	 case slt_amg:
	 case sle_amg:
	 case sgt_amg:
	 case sge_amg:
	 case seq_amg:
	 case sne_amg:
	   postpr = 1; off=scmp_amg; break;
	 }
      if (off != -1) cv = cvp[off];
      if (!cv) {
	goto not_found;
      }
    } else {
    not_found:			/* No method found, either report or croak */
      switch (method) {
	 case to_sv_amg:
	 case to_av_amg:
	 case to_hv_amg:
	 case to_gv_amg:
	 case to_cv_amg:
	     /* FAIL safe */
	     return left;	/* Delegate operation to standard mechanisms. */
	     break;
      }
      if (ocvp && (cv=ocvp[nomethod_amg])) { /* Call report method */
	notfound = 1; lr = -1;
      } else if (cvp && (cv=cvp[nomethod_amg])) {
	notfound = 1; lr = 1;
      } else {
	SV *msg;
	if (off==-1) off=method;
	msg = sv_2mortal(Perl_newSVpvf(aTHX_
		      "Operation \"%s\": no method found,%sargument %s%s%s%s",
		      AMG_id2name(method + assignshift),
		      (flags & AMGf_unary ? " " : "\n\tleft "),
		      SvAMAGIC(left)?
		        "in overloaded package ":
		        "has no overloaded magic",
		      SvAMAGIC(left)?
		        HvNAME_get(SvSTASH(SvRV(left))):
		        "",
		      SvAMAGIC(right)?
		        ",\n\tright argument in overloaded package ":
		        (flags & AMGf_unary
			 ? ""
			 : ",\n\tright argument has no overloaded magic"),
		      SvAMAGIC(right)?
		        HvNAME_get(SvSTASH(SvRV(right))):
		        ""));
	if (amtp && amtp->fallback >= AMGfallYES) {
	  DEBUG_o( Perl_deb(aTHX_ "%s", SvPVX_const(msg)) );
	} else {
	  Perl_croak(aTHX_ "%"SVf, msg);
	}
	return NULL;
      }
      force_cpy = force_cpy || assign;
    }
  }
#ifdef DEBUGGING
  if (!notfound) {
    DEBUG_o(Perl_deb(aTHX_
		     "Overloaded operator \"%s\"%s%s%s:\n\tmethod%s found%s in package %s%s\n",
		     AMG_id2name(off),
		     method+assignshift==off? "" :
		     " (initially \"",
		     method+assignshift==off? "" :
		     AMG_id2name(method+assignshift),
		     method+assignshift==off? "" : "\")",
		     flags & AMGf_unary? "" :
		     lr==1 ? " for right argument": " for left argument",
		     flags & AMGf_unary? " for argument" : "",
		     stash ? HvNAME_get(stash) : "null",
		     fl? ",\n\tassignment variant used": "") );
  }
#endif
    /* Since we use shallow copy during assignment, we need
     * to dublicate the contents, probably calling user-supplied
     * version of copy operator
     */
    /* We need to copy in following cases:
     * a) Assignment form was called.
     * 		assignshift==1,  assign==T, method + 1 == off
     * b) Increment or decrement, called directly.
     * 		assignshift==0,  assign==0, method + 0 == off
     * c) Increment or decrement, translated to assignment add/subtr.
     * 		assignshift==0,  assign==T,
     *		force_cpy == T
     * d) Increment or decrement, translated to nomethod.
     * 		assignshift==0,  assign==0,
     *		force_cpy == T
     * e) Assignment form translated to nomethod.
     * 		assignshift==1,  assign==T, method + 1 != off
     *		force_cpy == T
     */
    /*	off is method, method+assignshift, or a result of opcode substitution.
     *	In the latter case assignshift==0, so only notfound case is important.
     */
  if (( (method + assignshift == off)
	&& (assign || (method == inc_amg) || (method == dec_amg)))
      || force_cpy)
    RvDEEPCP(left);
  {
    dSP;
    BINOP myop;
    SV* res;
    const bool oldcatch = CATCH_GET;

    CATCH_SET(TRUE);
    Zero(&myop, 1, BINOP);
    myop.op_last = (OP *) &myop;
    myop.op_next = Nullop;
    myop.op_flags = OPf_WANT_SCALAR | OPf_STACKED;

    PUSHSTACKi(PERLSI_OVERLOAD);
    ENTER;
    SAVEOP();
    PL_op = (OP *) &myop;
    if (PERLDB_SUB && PL_curstash != PL_debstash)
	PL_op->op_private |= OPpENTERSUB_DB;
    PUTBACK;
    pp_pushmark();

    EXTEND(SP, notfound + 5);
    PUSHs(lr>0? right: left);
    PUSHs(lr>0? left: right);
    PUSHs( lr > 0 ? &PL_sv_yes : ( assign ? &PL_sv_undef : &PL_sv_no ));
    if (notfound) {
      PUSHs( sv_2mortal(newSVpv(AMG_id2name(method + assignshift),0)));
    }
    PUSHs((SV*)cv);
    PUTBACK;

    if ((PL_op = Perl_pp_entersub(aTHX)))
      CALLRUNOPS(aTHX);
    LEAVE;
    SPAGAIN;

    res=POPs;
    PUTBACK;
    POPSTACK;
    CATCH_SET(oldcatch);

    if (postpr) {
      int ans;
      switch (method) {
      case le_amg:
      case sle_amg:
	ans=SvIV(res)<=0; break;
      case lt_amg:
      case slt_amg:
	ans=SvIV(res)<0; break;
      case ge_amg:
      case sge_amg:
	ans=SvIV(res)>=0; break;
      case gt_amg:
      case sgt_amg:
	ans=SvIV(res)>0; break;
      case eq_amg:
      case seq_amg:
	ans=SvIV(res)==0; break;
      case ne_amg:
      case sne_amg:
	ans=SvIV(res)!=0; break;
      case inc_amg:
      case dec_amg:
	SvSetSV(left,res); return left;
      case not_amg:
	ans=!SvTRUE(res); break;
      default:
        ans=0; break;
      }
      return boolSV(ans);
    } else if (method==copy_amg) {
      if (!SvROK(res)) {
	Perl_croak(aTHX_ "Copy method did not return a reference");
      }
      return SvREFCNT_inc(SvRV(res));
    } else {
      return res;
    }
  }
}

/*
=for apidoc is_gv_magical

Returns C<TRUE> if given the name of a magical GV.

Currently only useful internally when determining if a GV should be
created even in rvalue contexts.

C<flags> is not used at present but available for future extension to
allow selecting particular classes of magical variable.

Currently assumes that C<name> is NUL terminated (as well as len being valid).
This assumption is met by all callers within the perl core, which all pass
pointers returned by SvPV.

=cut
*/
bool
Perl_is_gv_magical(pTHX_ char *name, STRLEN len, U32 flags)
{
    PERL_UNUSED_ARG(flags);

    if (len > 1) {
	const char * const name1 = name + 1;
	switch (*name) {
	case 'I':
	    if (len == 3 && name1[1] == 'S' && name[2] == 'A')
		goto yes;
	    break;
	case 'O':
	    if (len == 8 && strEQ(name1, "VERLOAD"))
		goto yes;
	    break;
	case 'S':
	    if (len == 3 && name[1] == 'I' && name[2] == 'G')
		goto yes;
	    break;
	    /* Using ${^...} variables is likely to be sufficiently rare that
	       it seems sensible to avoid the space hit of also checking the
	       length.  */
	case '\017':   /* ${^OPEN} */
	    if (strEQ(name1, "PEN"))
		goto yes;
	    break;
	case '\024':   /* ${^TAINT} */
	    if (strEQ(name1, "AINT"))
		goto yes;
	    break;
	case '\025':	/* ${^UNICODE} */
	    if (strEQ(name1, "NICODE"))
		goto yes;
	    if (strEQ(name1, "TF8LOCALE"))
		goto yes;
	    break;
	case '\027':   /* ${^WARNING_BITS} */
	    if (strEQ(name1, "ARNING_BITS"))
		goto yes;
	    break;
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	{
	    const char *end = name + len;
	    while (--end > name) {
		if (!isDIGIT(*end))
		    return FALSE;
	    }
	    goto yes;
	}
	}
    } else {
	/* Because we're already assuming that name is NUL terminated
	   below, we can treat an empty name as "\0"  */
	switch (*name) {
	case '&':
	case '`':
	case '\'':
	case ':':
	case '?':
	case '!':
	case '-':
	case '*':
	case '#':
	case '[':
	case '^':
	case '~':
	case '=':
	case '%':
	case '.':
	case '(':
	case ')':
	case '<':
	case '>':
	case ',':
	case '\\':
	case '/':
	case '|':
	case '+':
	case ';':
	case ']':
	case '\001':   /* $^A */
	case '\003':   /* $^C */
	case '\004':   /* $^D */
	case '\005':   /* $^E */
	case '\006':   /* $^F */
	case '\010':   /* $^H */
	case '\011':   /* $^I, NOT \t in EBCDIC */
	case '\014':   /* $^L */
	case '\016':   /* $^N */
	case '\017':   /* $^O */
	case '\020':   /* $^P */
	case '\023':   /* $^S */
	case '\024':   /* $^T */
	case '\026':   /* $^V */
	case '\027':   /* $^W */
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	yes:
	    return TRUE;
	default:
	    break;
	}
    }
    return FALSE;
}

/*
 * Local variables:
 * c-indentation-style: bsd
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 *
 * ex: set ts=8 sts=4 sw=4 noet:
 */
