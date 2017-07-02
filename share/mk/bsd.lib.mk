#	$OpenBSD: bsd.lib.mk,v 1.86 2017/07/02 17:55:14 espie Exp $
#	$NetBSD: bsd.lib.mk,v 1.67 1996/01/17 20:39:26 mycroft Exp $
#	@(#)bsd.lib.mk	5.26 (Berkeley) 5/2/91

.include <bsd.own.mk>				# for 'NOPIC' definition

.if exists(${.CURDIR}/../Makefile.inc)
.include "${.CURDIR}/../Makefile.inc"
.endif

.if exists(${.CURDIR}/shlib_version)
.include "${.CURDIR}/shlib_version"
.if defined(LIB) && defined(LIB${LIB}_VERSION)
SHLIB_MAJOR=${LIB${LIB}_VERSION:R}
SHLIB_MINOR=${LIB${LIB}_VERSION:E}
.else
SHLIB_MAJOR=${major}
SHLIB_MINOR=${minor}
.endif
.endif

.MAIN: all

# prefer .S to a .c, add .po, remove stuff not used in the BSD libraries.
# .so used for PIC object files.
# .do used for distrib "crunchgen" object files
# .m for objective c files.
.SUFFIXES:
.SUFFIXES: .out .o .po .so .do .S .s .c .cc .cpp .C .cxx .f .y .l .m4 .m

.if defined(NOPIE)
CFLAGS+=	${NOPIE_FLAGS}
CXXFLAGS+=	${NOPIE_FLAGS}
AFLAGS+=	${NOPIE_FLAGS}
.endif

DIST_CFLAGS+=	-Os

.c.o:
	@echo "${COMPILE.c} ${.IMPSRC} -o ${.TARGET}"
	@${COMPILE.c} ${DFLAGS} ${.IMPSRC}  -o ${.TARGET}.o
	@-mv $@.d $*.d
	@${LD} -X -r ${.TARGET}.o -o ${.TARGET}
	@rm -f ${.TARGET}.o

.c.po:
	@echo "${COMPILE.c} -p ${.IMPSRC} -o ${.TARGET}"
	@${COMPILE.c} ${DFLAGS} -p ${.IMPSRC} -o ${.TARGET}.o
	@-mv $@.d $*.d
	@${LD} -X -r ${.TARGET}.o -o ${.TARGET}
	@rm -f ${.TARGET}.o

.c.so:
	@echo "${COMPILE.c} ${PICFLAG} -DPIC ${.IMPSRC} -o ${.TARGET}"
	@${COMPILE.c} ${DFLAGS} ${PICFLAG} -DPIC ${.IMPSRC} -o ${.TARGET}.o
	@-mv $@.d $*.d
	@${LD} -X -r ${.TARGET}.o -o ${.TARGET}
	@rm -f ${.TARGET}.o

.c.do:
	@echo "${COMPILE.c} ${DIST_CFLAGS} ${.IMPSRC} -o ${.TARGET}"
	@${COMPILE.c} ${DFLAGS} ${DIST_CFLAGS} ${.IMPSRC}  -o ${.TARGET}.o
	@-mv $@.d $*.d
	@${LD} -X -r ${.TARGET}.o -o ${.TARGET}
	@rm -f ${.TARGET}.o

.cc.o .cpp.o .C.o .cxx.o:
	@echo "${COMPILE.cc} ${.IMPSRC} -o ${.TARGET}"
	@${COMPILE.cc} ${DFLAGS} ${.IMPSRC} -o ${.TARGET}.o
	@-mv $@.d $*.d
	@${LD} -X -r ${.TARGET}.o -o ${.TARGET}
	@rm -f ${.TARGET}.o

.cc.po .cpp.po .C.po .cxx.po:
	@echo "${COMPILE.cc} -p ${.IMPSRC} -o ${.TARGET}"
	@${COMPILE.cc} ${DFLAGS} -p ${.IMPSRC} -o ${.TARGET}.o
	@-mv $@.d $*.d
	@${LD} -X -r ${.TARGET}.o -o ${.TARGET}
	@rm -f ${.TARGET}.o

.cc.so .cpp.so .C.so .cxx.so:
	@echo "${COMPILE.cc} ${PICFLAG} -DPIC ${.IMPSRC} -o ${.TARGET}"
	@${COMPILE.cc} ${DFLAGS} ${PICFLAG} -DPIC ${.IMPSRC} -o ${.TARGET}.o
	@-mv $@.d $*.d
	@${LD} -X -r ${.TARGET}.o -o ${.TARGET}
	@rm -f ${.TARGET}.o

# Fortran 77
.f.o:
	@echo "${COMPILE.f} ${.IMPSRC} -o ${.TARGET}"
	@${COMPILE.f} ${DFLAGS} ${.IMPSRC} -o ${.TARGET}.o
	@-mv $@.d $*.d
	@${LD} -X -r ${.TARGET}.o -o ${.TARGET}
	@rm -f ${.TARGET}.o

.f.po:
	@echo "${COMPILE.f} -p ${.IMPSRC} -o ${.TARGET}"
	@${COMPILE.f} ${DFLAGS} -p ${.IMPSRC} -o ${.TARGET}.o
	@-mv $@.d $*.d
	@${LD} -X -r ${.TARGET}.o -o ${.TARGET}
	@rm -f ${.TARGET}.o

.f.so:
	@echo "${COMPILE.f} ${PICFLAG} -DPIC ${.IMPSRC} -o ${.TARGET}"
	@${COMPILE.f} ${DFLAGS} ${PICFLAG} -DPIC ${.IMPSRC} -o ${.TARGET}.o
	@-mv $@.d $*.d
	@${LD} -X -r ${.TARGET}.o -o ${.TARGET}
	@rm -f ${.TARGET}.o

.S.o .s.o:
	@echo "${COMPILE.S} ${CFLAGS:M-[ID]*} ${AINC} ${.IMPSRC} -o ${.TARGET}"
	@${COMPILE.S} ${DFLAGS} -MF $@.d ${CFLAGS:M-[IDM]*} ${AINC} \
	    ${.IMPSRC} -o ${.TARGET}.o
	@-mv $@.d $*.d
	@${LD} -X -r ${.TARGET}.o -o ${.TARGET}
	@rm -f ${.TARGET}.o

.S.po .s.po:
	@echo "${COMPILE.S} -DPROF ${CFLAGS:M-[ID]*} ${AINC} ${.IMPSRC} \
	    -o ${.TARGET}"
	@${COMPILE.S} ${DFLAGS} -MF $@.d -DPROF ${CFLAGS:M-[IDM]*} ${AINC} \
	    ${.IMPSRC} -o ${.TARGET}.o
	@-mv $@.d $*.d
	@${LD} -X -r ${.TARGET}.o -o ${.TARGET}
	@rm -f ${.TARGET}.o

.S.so .s.so:
	@echo "${COMPILE.S} ${PICFLAG} ${CFLAGS:M-[ID]*} ${AINC} ${.IMPSRC} \
	    -o ${.TARGET}"
	@${COMPILE.S} ${DFLAGS} -MF $@.d ${PICFLAG} ${CFLAGS:M-[IDM]*} \
	    ${AINC} ${.IMPSRC} -o ${.TARGET}.o
	@-mv $@.d $*.d
	@${LD} -X -r ${.TARGET}.o -o ${.TARGET}
	@rm -f ${.TARGET}.o

.S.do .s.do:
	@echo "${COMPILE.S} ${CFLAGS:M-[ID]*} ${AINC} ${DIST_CFLAGS} \
	    ${.IMPSRC} -o ${.TARGET}"
	@${COMPILE.S} ${DFLAGS} -MF $@.d ${CFLAGS:M-[IDM]*} ${AINC} \
	    ${DIST_CFLAGS} ${.IMPSRC} -o ${.TARGET}.o
	@-mv $@.d $*.d
	@${LD} -X -r ${.TARGET}.o -o ${.TARGET}
	@rm -f ${.TARGET}.o

.if ${WARNINGS:L} == "yes"
CFLAGS+=	${CDIAGFLAGS}
CXXFLAGS+=	${CXXDIAGFLAGS}
.endif
CFLAGS+=	${COPTS}
CXXFLAGS+=	${CXXOPTS}

DEBUG?=	-g

_LIBS=lib${LIB}.a
.if !defined(NOPROFILE)
_LIBS+=lib${LIB}_p.a
.endif

.if !defined(NOPIC)
.if defined(SHLIB_MAJOR) && defined(SHLIB_MINOR)
FULLSHLIBNAME=lib${LIB}.so.${SHLIB_MAJOR}.${SHLIB_MINOR}
_LIBS+=${FULLSHLIBNAME}
.endif

.if defined(LIBREBUILD)
_LIBS+=${FULLSHLIBNAME}.a

.if exists(${.CURDIR}/Symbols.list)
SYMBOLSMAP=Symbols.map
.endif

.endif

.if defined(VERSION_SCRIPT)
${FULLSHLIBNAME}:	${VERSION_SCRIPT}
LDADD+=	-Wl,--version-script=${VERSION_SCRIPT}
.endif
.endif

all: ${_LIBS} _SUBDIRUSE

BUILDAFTER += ${_LIBS}

OBJS+=	${SRCS:N*.h:R:S/$/.o/}
DEPS+=	${OBJS:R:S/$/.d/}
BUILDAFTER += ${OBJS}

lib${LIB}.a: ${OBJS}
	@echo building standard ${LIB} library
	@rm -f lib${LIB}.a
	@${AR} cqD lib${LIB}.a `${LORDER} ${OBJS} | tsort -q`
	${RANLIB} lib${LIB}.a

POBJS+=	${OBJS:.o=.po}
BUILDAFTER += ${POBJS}

lib${LIB}_p.a: ${POBJS}
	@echo building profiled ${LIB} library
	@rm -f lib${LIB}_p.a
	@${AR} cqD lib${LIB}_p.a `${LORDER} ${POBJS} | tsort -q`
	${RANLIB} lib${LIB}_p.a

SOBJS+=	${OBJS:.o=.so}
BUILDAFTER += ${SOBJS}

${FULLSHLIBNAME}: ${SOBJS} ${DPADD}
	@echo building shared ${LIB} library \(version ${SHLIB_MAJOR}.${SHLIB_MINOR}\)
	@rm -f ${.TARGET}
.if defined(SYSPATCH)
	${CC} -shared ${PICFLAG} -o ${.TARGET} \
	    `readelf -Ws ${LIBDIR}/${.TARGET} | awk '/ FILE/{gsub(/\..*/, ".so", $$NF); sub(".*/", "", $$NF); print $$NF}' | \
	    egrep -v "(cmll-586|libgcc2|unwind-dw2)" | awk '!x[$$0]++'` ${LDADD}
.else
	${CC} -shared ${PICFLAG} -o ${.TARGET} \
	    `echo ${SOBJS} | tr ' ' '\n' | sort -R` ${LDADD}
.endif

${FULLSHLIBNAME}.a: ${SOBJS}
	@echo building shared ${LIB} library \(version ${SHLIB_MAJOR}.${SHLIB_MINOR}\) ar
	@rm -f ${.TARGET}
	@echo ${PICFLAG} ${LDADD} > .ldadd
	ar cqD ${FULLSHLIBNAME}.a ${SOBJS} .ldadd ${SYMBOLSMAP}

# all .do files...
DOBJS+=	${OBJS:.o=.do}
BUILDAFTER += ${DOBJS}

# .do files that we actually need for where this dist lib will be used
.if defined(DIST_OBJS)
SELECTED_DOBJS=${DIST_OBJS:.o=.do}
.else
SELECTED_DOBJS?=${DOBJS}
.endif

DIST_LIB?=lib${LIB}_d.a
${DIST_LIB}: ${SELECTED_DOBJS}
	@echo building distrib ${DIST_LIB} library from ${SELECTED_DOBJS}
	@rm -f ${DIST_LIB}
.if !empty(SELECTED_DOBJS)
	@${AR} cqD ${DIST_LIB} `${LORDER} ${SELECTED_DOBJS} | tsort -q`
.else
	@${AR} cqD ${DIST_LIB}
.endif
	${RANLIB} ${DIST_LIB}

.if !target(clean)
clean: _SUBDIRUSE
	rm -f a.out [Ee]rrs mklog *.core ${CLEANFILES}
	rm -f lib${LIB}.a ${OBJS}
	rm -f lib${LIB}_g.a ${GOBJS}
	rm -f lib${LIB}_p.a ${POBJS}
	rm -f lib${LIB}.so.*.* ${SOBJS} .ldadd
	rm -f ${DIST_LIB} ${DOBJS}
.endif

cleandir: _SUBDIRUSE clean


.if !target(install)
.if !target(beforeinstall)
beforeinstall:
.endif

realinstall:
#	ranlib lib${LIB}.a
	${INSTALL} ${INSTALL_COPY} -S -o ${LIBOWN} -g ${LIBGRP} -m 600 lib${LIB}.a \
	    ${DESTDIR}${LIBDIR}/lib${LIB}.a
.if (${INSTALL_COPY} != "-p")
	${RANLIB} -t ${DESTDIR}${LIBDIR}/lib${LIB}.a
.endif
	chmod ${LIBMODE} ${DESTDIR}${LIBDIR}/lib${LIB}.a
.if !defined(NOPROFILE)
#	ranlib lib${LIB}_p.a
	${INSTALL} ${INSTALL_COPY} -S -o ${LIBOWN} -g ${LIBGRP} -m 600 \
	    lib${LIB}_p.a ${DESTDIR}${LIBDIR}
.if (${INSTALL_COPY} != "-p")
	${RANLIB} -t ${DESTDIR}${LIBDIR}/lib${LIB}_p.a
.endif
	chmod ${LIBMODE} ${DESTDIR}${LIBDIR}/lib${LIB}_p.a
.endif
.if !defined(NOPIC) && defined(SHLIB_MAJOR) && defined(SHLIB_MINOR)
	${INSTALL} ${INSTALL_COPY} -S -o ${LIBOWN} -g ${LIBGRP} -m ${LIBMODE} \
	    ${FULLSHLIBNAME} ${DESTDIR}${LIBDIR}
.if defined(LIBREBUILD)
	${INSTALL} ${INSTALL_COPY} -S -o ${LIBOWN} -g ${LIBGRP} -m ${LIBMODE} \
	    ${FULLSHLIBNAME}.a ${DESTDIR}${LIBDIR}
.endif
.endif
.if defined(LINKS) && !empty(LINKS)
.  for lnk file in ${LINKS}
	@l=${DESTDIR}${lnk}; \
	 t=${DESTDIR}${file}; \
	 echo $$t -\> $$l; \
	 rm -f $$t; ln $$l $$t
.  endfor
.endif

install: maninstall _SUBDIRUSE
maninstall: afterinstall
afterinstall: realinstall
realinstall: beforeinstall
.endif

.if !defined(NOMAN)
.include <bsd.man.mk>
.endif

.include <bsd.obj.mk>
.include <bsd.dep.mk>
.include <bsd.subdir.mk>
.include <bsd.sys.mk>
