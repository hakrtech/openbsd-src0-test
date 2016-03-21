#	$OpenBSD: obsd-regress.t,v 1.3 2016/03/21 13:35:00 tb Exp $

#
# ksh regression tests from OpenBSD
#

name: eval-1
description:
	Tests for ${name#pat} vs. "${name#pat}" expansion
stdin:
	a=
	for n in ${a#*=}; do echo ${n}; done
	for n in "${a#*=}"; do echo ${n}; done
expected-stdout:
	
---

name: eval-2
description:
	Tests for ${name##pat} vs. "${name##pat}" expansion
stdin:
	a=
	for n in ${a##*=}; do echo ${n}; done
	for n in "${a##*=}"; do echo ${n}; done
expected-stdout:
	
---

name: eval-3
description:
	Tests for ${name%=pat} vs. "${name%=pat}" expansion
stdin:
	a=
	for n in ${a%=*}; do echo ${n}; done
	for n in "${a%=*}"; do echo ${n}; done
expected-stdout:
	
---

name: eval-4
description:
	Tests for ${name%%=pat} vs. "${name%%=pat}" expansion
stdin:
	a=
	for n in ${a%%=*}; do echo ${n}; done
	for n in "${a%%=*}"; do echo ${n}; done
expected-stdout:
	
---

name: and-list-error-1
description:
	Test exit status of rightmost element in 2 element && list in -e mode
stdin:
	true && false
	echo "should not print"
arguments: !-e!
expected-exit: e != 0
---

name: and-list-error-2
description:
	Test exit status of rightmost element in 3 element && list in -e mode
stdin:
	true && true && false
	echo "should not print"
arguments: !-e!
expected-exit: e != 0
---

name: or-list-error-1
description:
	Test exit status of || list in -e mode
stdin:
	false || false
	echo "should not print"
arguments: !-e!
expected-exit: e != 0
---

name: var-functions
description:
	Calling
	
		FOO=bar f
	
	where f is a ksh style function, should not set FOO in the current env.
	If f is a bourne style function, FOO should be set. Furthermore,
	the function should receive a correct value of FOO. Additionally,
	setting FOO in the function itself should not change the value in
	global environment.
	
	Inspired by PR 2450.
stdin:
	function k {
		if [ x$FOO != xbar ]; then
			echo 1
			return 1
		fi
		x=$(env | grep FOO)
		if [ "x$x" != "xFOO=bar" ]; then
			echo 2
			return 1;
		fi
		FOO=foo
		return 0
	}
	b () {
		if [ x$FOO != xbar ]; then
			echo 3
			return 1
		fi
		x=$(env | grep FOO)
		if [ "x$x" != "xFOO=bar" ]; then
			echo 4
			return 1;
		fi
		FOO=foo
		return 0
	}
	FOO=bar k
	if [ $? != 0 ]; then
		exit 1
	fi
	if [ x$FOO != x ]; then
		exit 1
	fi
	FOO=bar b
	if [ $? != 0 ]; then
		exit 1
	fi
	if [ x$FOO != xbar ]; then
		exit 1
	fi
	FOO=barbar
	FOO=bar k
	if [ $? != 0 ]; then
		exit 1
	fi
	if [ x$FOO != xbarbar ]; then
		exit 1
	fi
	FOO=bar b
	if [ $? != 0 ]; then
		exit 1
	fi
	if [ x$FOO != xbar ]; then
		exit 1
	fi
---

name: longline-crash
description:
	This used to cause a core dump
stdin:
	ulimit -c 0
	deplibs="-lz -lpng /usr/local/lib/libjpeg.la -ltiff -lm -lX11 -lXext /usr/local/lib/libiconv.la -L/usr/local/lib -L/usr/ports/devel/gettext/w-gettext-0.10.40/gettext-0.10.40/intl/.libs /usr/local/lib/libintl.la /usr/local/lib/libglib.la /usr/local/lib/libgmodule.la -lintl -lm -lX11 -lXext -L/usr/X11R6/lib -lglib -lgmodule -L/usr/local/lib /usr/local/lib/libgdk.la -lintl -lm -lX11 -lXext -L/usr/X11R6/lib -lglib -lgmodule -L/usr/local/lib /usr/local/lib/libgtk.la -ltiff -ljpeg -lz -lpng -lm -lX11 -lXext -lintl -lglib -lgmodule -lgdk -lgtk -L/usr/X11R6/lib -lglib -lgmodule -L/usr/local/lib /usr/local/lib/libgdk_pixbuf.la -lz -lpng /usr/local/lib/libiconv.la -L/usr/local/lib -L/usr/ports/devel/gettext/w-gettext-0.10.40/gettext-0.10.40/intl/.libs /usr/local/lib/libintl.la /usr/local/lib/libglib.la -lm -lm /usr/local/lib/libaudiofile.la -lm -lm -laudiofile -L/usr/local/lib /usr/local/lib/libesd.la -lm -lz -L/usr/local/lib /usr/local/lib/libgnomesupport.la -lm -lz -lm -lglib -L/usr/local/lib /usr/local/lib/libgnome.la -lX11 -lXext /usr/local/lib/libiconv.la -L/usr/local/lib -L/usr/ports/devel/gettext/w-gettext-0.10.40/gettext-0.10.40/intl/.libs /usr/local/lib/libintl.la /usr/local/lib/libgmodule.la -lintl -lm -lX11 -lXext -L/usr/X11R6/lib -lglib -lgmodule -L/usr/local/lib /usr/local/lib/libgdk.la -lintl -lm -lX11 -lXext -L/usr/X11R6/lib -lglib -lgmodule -L/usr/local/lib /usr/local/lib/libgtk.la -lICE -lSM -lz -lpng /usr/local/lib/libungif.la /usr/local/lib/libjpeg.la -ltiff -lm -lz -lpng /usr/local/lib/libungif.la -lz /usr/local/lib/libjpeg.la -ltiff -L/usr/local/lib -L/usr/X11R6/lib /usr/local/lib/libgdk_imlib.la -lm -L/usr/local/lib /usr/local/lib/libart_lgpl.la -lm -lz -lm -lX11 -lXext -lintl -lglib -lgmodule -lgdk -lgtk -lICE -lSM -lm -lX11 -lXext -lintl -lglib -lgmodule -lgdk -lgtk -L/usr/X11R6/lib -lm -lz -lpng -lungif -lz -ljpeg -ltiff -ljpeg -lgdk_imlib -lglib -lm -laudiofile -lm -laudiofile -lesd -L/usr/local/lib /usr/local/lib/libgnomeui.la -lz -lz /usr/local/lib/libxml.la -lz -lz -lz /usr/local/lib/libxml.la -lm -lX11 -lXext /usr/local/lib/libiconv.la -L/usr/ports/devel/gettext/w-gettext-0.10.40/gettext-0.10.40/intl/.libs /usr/local/lib/libintl.la /usr/local/lib/libglib.la /usr/local/lib/libgmodule.la -lintl -lglib -lgmodule /usr/local/lib/libgdk.la /usr/local/lib/libgtk.la -L/usr/X11R6/lib -L/usr/local/lib /usr/local/lib/libglade.la -lz -lz -lz /usr/local/lib/libxml.la /usr/local/lib/libglib.la -lm -lm /usr/local/lib/libaudiofile.la -lm -lm -laudiofile /usr/local/lib/libesd.la -lm -lz /usr/local/lib/libgnomesupport.la -lm -lz -lm -lglib /usr/local/lib/libgnome.la -lX11 -lXext /usr/local/lib/libiconv.la -L/usr/ports/devel/gettext/w-gettext-0.10.40/gettext-0.10.40/intl/.libs /usr/local/lib/libintl.la /usr/local/lib/libgmodule.la -lintl -lm -lX11 -lXext -lglib -lgmodule /usr/local/lib/libgdk.la -lintl -lm -lX11 -lXext -lglib -lgmodule /usr/local/lib/libgtk.la -lICE -lSM -lz -lpng /usr/local/lib/libungif.la /usr/local/lib/libjpeg.la -ltiff -lm -lz -lz /usr/local/lib/libgdk_imlib.la /usr/local/lib/libart_lgpl.la -lm -lz -lm -lX11 -lXext -lintl -lglib -lgmodule -lgdk -lgtk -lm -lX11 -lXext -lintl -lglib -lgmodule -lgdk -lgtk -lm -lz -lungif -lz -ljpeg -ljpeg -lgdk_imlib -lglib -lm -laudiofile -lm -laudiofile -lesd /usr/local/lib/libgnomeui.la -L/usr/X11R6/lib -L/usr/local/lib /usr/local/lib/libglade-gnome.la /usr/local/lib/libglib.la -lm -lm /usr/local/lib/libaudiofile.la -lm -lm -laudiofile -L/usr/local/lib /usr/local/lib/libesd.la -lm -lz -L/usr/local/lib /usr/local/lib/libgnomesupport.la -lm -lz -lm -lglib -L/usr/local/lib /usr/local/lib/libgnome.la -lX11 -lXext /usr/local/lib/libiconv.la -L/usr/local/lib -L/usr/ports/devel/gettext/w-gettext-0.10.40/gettext-0.10.40/intl/.libs /usr/local/lib/libintl.la /usr/local/lib/libgmodule.la -lintl -lm -lX11 -lXext -L/usr/X11R6/lib -lglib -lgmodule -L/usr/local/lib /usr/local/lib/libgdk.la -lintl -lm -lX11 -lXext -L/usr/X11R6/lib -lglib -lgmodule -L/usr/local/lib /usr/local/lib/libgtk.la -lICE -lSM -lz -lpng /usr/local/lib/libungif.la /usr/local/lib/libjpeg.la -ltiff -lm -lz -lpng /usr/local/lib/libungif.la -lz /usr/local/lib/libjpeg.la -ltiff -L/usr/local/lib -L/usr/X11R6/lib /usr/local/lib/libgdk_imlib.la -lm -L/usr/local/lib /usr/local/lib/libart_lgpl.la -lm -lz -lm -lX11 -lXext -lintl -lglib -lgmodule -lgdk -lgtk -lICE -lSM -lm -lX11 -lXext -lintl -lglib -lgmodule -lgdk -lgtk -L/usr/X11R6/lib -lm -lz -lpng -lungif -lz -ljpeg -ltiff -ljpeg -lgdk_imlib -lglib -lm -laudiofile -lm -laudiofile -lesd -L/usr/local/lib /usr/local/lib/libgnomeui.la -L/usr/X11R6/lib -L/usr/local/lib"
	specialdeplibs="-lgnomeui -lart_lgpl -lgdk_imlib -ltiff -ljpeg -lungif -lpng -lz -lSM -lICE -lgtk -lgdk -lgmodule -lintl -lXext -lX11 -lgnome -lgnomesupport -lesd -laudiofile -lm -lglib"
	for deplib in $deplibs; do
	    case $deplib in
		-L*) 
		    new_libs="$deplib $new_libs" 
		    ;;
		*)
		    case " $specialdeplibs " in
			*" $deplib "*) 
			    new_libs="$deplib $new_libs";;
		    esac
		    ;;
	    esac
	done
---

name: seterror-1
description:
	The -e flag should be ignored when executing a compound list
	followed by an if statement.
stdin:
	if true; then false && false; fi
	true
arguments: !-e!
expected-exit: e == 0
---

name: seterror-2
description:
	The -e flag should be ignored when executing a compound list
	followed by an if statement.
stdin:
	if true; then if true; then false && false; fi; fi
	true
arguments: !-e!
expected-exit: e == 0
---

name: seterror-3
description:
	The -e flag should be ignored when executing a compound list
	followed by an elif statement.
stdin:
	if true; then :; elif true; then false && false; fi
arguments: !-e!
expected-exit: e == 0
---

name: seterror-4
description:
	The -e flag should be ignored when executing a pipeline
	beginning with '!'
stdin:
	for i in 1 2 3
	do
		false && false
		true || false
	done
arguments: !-e!
expected-exit: e == 0
---

name: seterror-5
description:
	The -e flag should be ignored when executing a pipeline
	beginning with '!'
stdin:
	! true | false
	true
arguments: !-e!
expected-exit: e == 0
---

name: seterror-6
description:
	When trapping ERR and EXIT, both traps should run in -e mode
	when an error occurs.
stdin:
	trap 'echo EXIT' EXIT
	trap 'echo ERR' ERR
	set -e
	false
	echo DONE
	exit 0
arguments: !-e!
expected-exit: e != 0
expected-stdout:
	ERR
	EXIT
---

name: seterror-7
description:
	The -e flag within a command substitution should be honored
stdin:
	echo $( set -e; false; echo foo )
arguments: !-e!
expected-stdout:
	
---

name: input-comsub
description:
	A command substitution using input redirection should exit with
	failure if the input file does not exist.
stdin:
	var=$(< non-existent)
expected-exit: e != 0
expected-stderr-pattern: /non-existent/

---
name: empty-for-list
description:
	A for list which expands to zero items should not execute the body.
stdin:
	set foo bar baz ; for out in ; do echo $out ; done

---

name: command-pvV-posix-priorities
description:
	For POSIX compatibility, command -v should find aliases and reserved
	words, and command -p[vV] should find aliases, reserved words, and
	builtins over external commands.
stdin:
	PATH=$(command -p getconf PATH) || PATH=/bin:/usr/bin
	alias foo="bar baz"
	bar() { :; }
	for word in 'if' 'foo' 'bar' 'set' 'true' 'ls'; do
		command -v "$word"
		command -pv "$word"
		command -V "$word"
		command -pV "$word"
	done
expected-stdout-pattern:
	/^if
	if
	if is a reserved word
	if is a reserved word
	alias foo='bar baz'
	alias foo='bar baz'
	foo is an alias for 'bar baz'
	foo is an alias for 'bar baz'
	bar
	bar
	bar is a function
	bar is a function
	set
	set
	set is a special shell builtin
	set is a special shell builtin
	true
	true
	true is a shell builtin
	true is a shell builtin
	.*\/ls.*
	.*\/ls.*
	ls is a tracked alias for .*\/ls.*
	ls is .*\/ls.*$/
---

name: whence-preserve-tradition
description:
	POSIX 'command' and ksh88/pdksh-specific 'whence' are handled by the
	same c_whence() function.  This regression test is to ensure that
	the POSIX compatibility changes for 'command' (see previous test) do
	not affect traditional 'whence' behaviour.
stdin:
	PATH=$(command -p getconf PATH) || PATH=/bin:/usr/bin
	alias foo="bar baz"
	bar() { :; }
	for word in 'if' 'foo' 'bar' 'set' 'true' 'ls'; do
		whence "$word"
		whence -p "$word"
		whence -v "$word"
		whence -pv "$word"
	done
expected-stdout-pattern:
	/^if
	if is a reserved word
	if not found
	'bar baz'
	foo is an alias for 'bar baz'
	foo not found
	bar
	bar is a function
	bar not found
	set
	set is a special shell builtin
	set not found
	true
	.*\/true.*
	true is a shell builtin
	true is a tracked alias for .*\/true.*
	.*\/ls.*
	.*\/ls.*
	ls is a tracked alias for .*\/ls.*
	ls is a tracked alias for .*\/ls.*$/
---

name: shellopt-u-1
description:
	Check that "$@" and "$*" are exempt from 'set -u' (nounset)
stdin:
	set -u
	: "$@$*$1"
expected-exit: e == 1
expected-stderr-pattern:
	/: 1: parameter not set$/
---
