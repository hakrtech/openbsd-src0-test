/* Definitions for HP-PA systems running BSD as target machine for GNU compiler.
   Copyright (C) 1993, 1995 Free Software Foundation, Inc.

This file is part of GNU CC.

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

/* We settle for little endian for now */

#define TARGET_ENDIAN_DEFAULT 0

#ifndef CROSS_COMPILE
/* Look for the G++ include files in the system-defined place.  */

#undef GPLUSPLUS_INCLUDE_DIR
#define GPLUSPLUS_INCLUDE_DIR "/usr/include/g++"

/* Under OpenBSD, the normal location of the various *crt*.o files is the
   /usr/lib directory.  */

#undef STANDARD_STARTFILE_PREFIX
#define STANDARD_STARTFILE_PREFIX "/usr/lib/"
#endif

/* Provide a LINK_SPEC appropriate for OpenBSD.  Here we provide support
   for the special GCC options -static, -assert, and -nostdlib.  */

#undef LINK_SPEC
#define LINK_SPEC \
  "%{EB} %{EL} %{shared} %{non_shared} \
   %{call_shared} %{no_archive} %{exact_version} \
   %{!shared: %{!non_shared: %{!call_shared: -non_shared}}} \
   %{!dynamic-linker:-dynamic-linker /usr/libexec/ld.so} \
   %{!nostdlib:%{!r*:%{!e*:-e __start}}} -dc -dp \
   %{static:-Bstatic} %{!static:-Bdynamic} %{assert*}"

/* We have atexit(3).  */

#define HAVE_ATEXIT

/* Implicit library calls should use memcpy, not bcopy, etc.  */

#define TARGET_MEM_FUNCTIONS

/* Define hppa-specific OpenBSD predefines... */
#ifndef CPP_PREDEFINES
#define CPP_PREDEFINES "-Dunix -D__ANSI_COMPAT \
-Asystem(unix) -Asystem(OpenBSD) -Amachine(hppa) \
-D__OpenBSD__ -D__hppa__ -D__hppa"
#endif

/* Always uses gas.  */
#ifndef ASM_SPEC
#define ASM_SPEC "\
%|"
#endif

#ifndef CPP_SPEC
#define CPP_SPEC "\
%{posix:-D_POSIX_SOURCE}"
#endif

#define LIB_SPEC "%{!p:%{!pg:-lc}}%{p:-lc_p}%{pg:-lc_p}"
#define STARTFILE_SPEC \
   "%{!shared:%{pg:gcrt0.o%s}%{!pg:%{p:mcrt0.o%s}%{!p:crt0.o%s}}}"

#ifndef MACHINE_TYPE
#define MACHINE_TYPE "OpenBSD/hppa"
#endif

#define LOCAL_LABEL_PREFIX	"."

/* Since gas and gld are standard on OpenBSD, we don't need this */
#undef ASM_FINAL_SPEC
#undef STARTFILE_SPEC

