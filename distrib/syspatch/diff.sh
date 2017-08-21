#!/bin/ksh
#
# $OpenBSD: diff.sh,v 1.4 2017/08/21 08:33:48 ajacoutot Exp $
#
# Copyright (c) 2017 Robert Nagy <robert@openbsd.org>
#
# Permission to use, copy, modify, and distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

umask 0022

trap exit HUP INT TERM

diff -Parq $1 $2 2>&1 | grep ' differ$' | \
while IFS= read -r _d
do
	_o=$(echo $_d | cut -d ' ' -f2)
	_n=$(echo $_d | cut -d ' ' -f4)
	case "${_o##*.}"
	in
		a)
			cmp -s ${_o} ${_n} 34 34 || echo ${_n}
			;;
		1|3p)
			_onm=$(mktemp)
			_nnm=$(mktemp)
			trap 'rm -f ${_onm} ${_nnm}' EXIT
			sed -E '/([0-9]+-[0-9]+-[0-9]+|=+)/d' ${_o} > ${_onm}
			sed -E '/([0-9]+-[0-9]+-[0-9]+|=+)/d' ${_n} > ${_nnm}
			diff -q ${_onm} ${_nnm} >/dev/null || echo ${_n}
			rm -f ${_onm} ${_nnm}
			;;
		EFI|dat|db|tgz|*void|*dir|mk|cache-4)
			;;
		*)
			echo ${_n}
			;;
	esac
done
