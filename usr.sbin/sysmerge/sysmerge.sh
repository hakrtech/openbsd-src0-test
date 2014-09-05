#!/bin/ksh -
#
# $OpenBSD: sysmerge.sh,v 1.171 2014/09/05 17:22:10 ajacoutot Exp $
#
# Copyright (c) 2008-2014 Antoine Jacoutot <ajacoutot@openbsd.org>
# Copyright (c) 1998-2003 Douglas Barton <DougB@FreeBSD.org>
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
#

umask 0022

usage() {
	echo "usage: ${0##*/} [-bdp]" >&2 && exit 1
}

# OpenBSD /etc/rc v1.438
stripcom() {
	local _file="$1"
	local _line

	{
		while read _line ; do
			_line=${_line%%#*}		# strip comments
			test -z "$_line" && continue
			echo $_line
		done
	} < $_file
}

sm_echo() {
	echo "$@" | tee -a ${_WRKDIR}/sysmerge.log
}

sm_error() {
	(($#)) && sm_echo "!!!! ERROR: $@"

	# restore sum files from backups or remove the newly created ones
	for _i in ${_WRKDIR}/{etcsum,xetcsum,examplessum,pkgsum}; do
		_j=$(basename ${_i})
		if [[ -f ${_i} ]]; then
			mv ${_i} /usr/share/sysmerge/${_j}
		elif [[ -f /usr/share/sysmerge/${_j} ]]; then
			rm /usr/share/sysmerge/${_j}
		fi
	done

	# do not empty _WRKDIR, it may still contain our backup files
	rm -rf ${_TMPROOT}
	rmdir ${_WRKDIR} 2>/dev/null
	exit 1
}

trap "sm_error; exit 1" 1 2 3 13 15

sm_warn() {
	(($#)) && sm_echo "**** WARNING: $@" || true
}

sm_extract_sets() {
	[[ -n ${PKGMODE} ]] && return
	local _e _x _set

	[[ -f /usr/share/sysmerge/etc.tgz ]] && _e=etc
	[[ -f /usr/share/sysmerge/xetc.tgz ]] && _x=xetc

	for _set in ${_e} ${_x}; do
		[[ -f /usr/share/sysmerge/${_set}sum ]] && \
			cp /usr/share/sysmerge/${_set}sum \
			${_WRKDIR}/${_set}sum
		tar -xzphf \
			/usr/share/sysmerge/${_set}.tgz || \
			sm_error "failed to extract ${_set}.tgz"
	done
}

# get pkg @sample information
exec_espie() {
	local _tmproot

	_tmproot=${_TMPROOT} /usr/bin/perl <<'EOF'
use strict;
use warnings;

package OpenBSD::PackingElement;

sub walk_sample
{
}

package OpenBSD::PackingElement::Sampledir;
sub walk_sample
{
	my $item = shift;
	print "0-DIR", " ",
	      $item->{owner} // "root", " ",
	      $item->{group} // "wheel", " ",
	      $item->{mode} // "0755", " ",
	      $ENV{'_tmproot'}, $item->fullname,
	      "\n";
}

package OpenBSD::PackingElement::Sample;
sub walk_sample
{
	my $item = shift;
	print "1-FILE", " ",
	      $item->{owner} // "root", " ",
	      $item->{group} // "wheel", " ",
	      $item->{mode} // "0644", " ",
	      $item->{copyfrom}->fullname, " ",
	      $ENV{'_tmproot'}, $item->fullname,
	      "\n";
}

package main;
use OpenBSD::PackageInfo;
use OpenBSD::PackingList;

for my $i (installed_packages()) {
	my $plist = OpenBSD::PackingList->from_installation($i);
	$plist->walk_sample();
}
EOF
}

sm_cp_pkg_samples() {
	[[ -z ${PKGMODE} ]] && return
	local _install_args _i _ret=0 _sample

	[[ -f /usr/share/sysmerge/pkgsum ]] && \
		cp /usr/share/sysmerge/pkgsum ${_WRKDIR}/pkgsum

	# access to full base system hierarchy is implied in packages
	mtree -qdef /etc/mtree/4.4BSD.dist -U >/dev/null
	mtree -qdef /etc/mtree/BSD.x11.dist -U >/dev/null

	# @sample directories are processed first
	exec_espie | sort -u | while read _i; do
		set -A _sample -- ${_i}
		_install_args="-o ${_sample[1]} -g ${_sample[2]} -m ${_sample[3]}"
		if [[ ${_sample[0]} == "0-DIR" ]]; then
			install -d ${_install_args} ${_sample[4]} || _ret=1
		else
			# directory we want to copy the @sample file into
			# does not exist and is not a @sample so we have no
			# knowledge of the required owner/group/mode
			# (e.g. /var/www/usr/sbin in mail/femail,-chroot)
			_pkghier=$(dirname ${_sample[5]#${_TMPROOT}})
			if [[ ! -d ${_pkghier} ]]; then
				sm_warn "skipping ${_sample[5]#${_TMPROOT}}: ${_pkghier} does not exist"
				continue
			fi
			if [[ -d $(dirname ${_sample[5]}) ]]; then
				install ${_install_args} \
					${_sample[4]} ${_sample[5]} || _ret=1
			fi
		fi
	done

	if [[ ${_ret} -eq 0 ]]; then
		find . -type f | sort | \
			xargs sha256 -h ./usr/share/sysmerge/pkgsum || \
			_ret=1
	fi
	[[ ${_ret} -ne 0 ]] && \
		sm_error "failed to populate packages @samples and create sum file"
}

sm_init() {
	local _auto_upg _c _c1 _c2 _cursum _i _k _j _cfdiff _cffiles
	local _ignorefile _cvsid1 _cvsid2 _matchsum _mismatch

	sm_extract_sets
	sm_add_user_grp
	sm_check_an_eg
	sm_cp_pkg_samples

	for _i in etcsum xetcsum pkgsum; do
		if [[ -f /usr/share/sysmerge/${_i} && \
			-f ./usr/share/sysmerge/${_i} && \
			-z ${DIFFMODE} ]]; then
			# redirect stderr; file may not exist
			_matchsum=$(sha256 -c /usr/share/sysmerge/${_i} 2>/dev/null | \
				awk '/OK/ { print $2 }' | \
				sed 's/[:]//')
			# delete file in temproot if it has not changed since
			# last release and is present in current installation
			for _j in ${_matchsum}; do
				# skip sum files
				[[ ${_j} == ./usr/share/sysmerge/${_i} ]] && continue
				[[ -f ${_j#.} && -f ${_j} ]] && \
					rm ${_j}
			done

			# set auto-upgradable files
			_mismatch=$(diff -u ./usr/share/sysmerge/${_i} /usr/share/sysmerge/${_i} | \
				sed -n 's/^+SHA256 (\(.*\)).*/\1/p')
			for _k in ${_mismatch}; do
				# skip sum files
				[[ ${_k} == ./usr/share/sysmerge/${_i} ]] && continue


				# compare CVS $Id's first so if the file hasn't been modified,
				# it will be deleted from temproot and ignored from comparison;
				# several files are generated from scripts so CVS ID is not a
				# reliable way of detecting changes: leave for a full diff
				if [[ -z ${PKGMODE} && \
					${_k} != ./etc/@(fbtab|ttys) && \
					! -h ${_k} ]]; then
					_cvsid1=$(sed -n "/[$]OpenBSD:.*Exp [$]/{p;q;}" ${_k#.} 2>/dev/null)
					_cvsid2=$(sed -n "/[$]OpenBSD:.*Exp [$]/{p;q;}" ${_k} 2>/dev/null)
					[[ ${_cvsid2} == ${_cvsid1} ]] && rm ${_k} && continue
				fi


				# redirect stderr; file may not exist
				_cursum=$(cd / && sha256 ${_k} 2>/dev/null)
				[[ -n $(grep "${_cursum}" /usr/share/sysmerge/${_i}) && \
					-z $(grep "${_cursum}" ./usr/share/sysmerge/${_i}) ]] && \
					_auto_upg="${_auto_upg} ${_k}"
			done
			[[ -n ${_auto_upg} ]] && set -A AUTO_UPG -- ${_auto_upg}
		fi
		[[ -f ./usr/share/sysmerge/${_i} ]] && \
			mv ./usr/share/sysmerge/${_i} /usr/share/sysmerge/${_i}
	done

	# files we don't want/need to deal with
	_ignorefiles="/etc/group
		      /etc/localtime
		      /etc/mail/aliases.db
		      /etc/master.passwd
		      /etc/motd
		      /etc/passwd
		      /etc/pwd.db
		      /etc/spwd.db
		      /usr/share/sysmerge/etcsum
		      /usr/share/sysmerge/examplessum
		      /usr/share/sysmerge/xetcsum
		      /var/db/locate.database
		      /var/mail/root"
	[[ -f /etc/sysmerge.ignore ]] && \
		_ignorefiles="${_ignorefiles} $(stripcom /etc/sysmerge.ignore)"
	for _i in ${_ignorefiles}; do
		rm -f ./${_i}
	done

	# aliases(5) needs to be handled last in case mailer.conf(5) changes
	_c1=$(find . -type f -or -type l | grep -vE '^./etc/mail/aliases$')
	_c2=$(find . -type f -name aliases)
	for COMPFILE in ${_c1} ${_c2}; do
		unset IS_BINFILE IS_LINK
		TARGET=${COMPFILE#.}

		# links need to be treated in a different way
		if [[ -h ${COMPFILE} ]]; then
			IS_LINK=1
			[[ -h ${TARGET} && \
				$(readlink ${COMPFILE}) == $(readlink ${TARGET}) ]] && \
				rm ${COMPFILE} && continue
		else
			# disable sdiff for binaries
			diff -q /dev/null ${COMPFILE} | grep -q Binary && \
				IS_BINFILE=1

			# empty files = binaries (to avoid comparison);
			# only process them if they don't exist on the system
			if [[ ! -s ${COMPFILE} ]]; then
				if [[ -f ${TARGET} ]]; then
					[[ -f ${COMPFILE} ]] && rm ${COMPFILE}
					continue
				else
					IS_BINFILE=1
				fi
			fi

			# make sure files are different; if not: delete
			diff -q ${TARGET} ${COMPFILE} >/dev/null && \
				rm ${COMPFILE} && continue
		fi

		sm_diff_loop
	done
}

sm_install() {
	local _dmode _fgrp _fmode _fown _instdir
	_instdir=$(dirname ${TARGET})

	_dmode=$(stat -f "%OMp%OLp" ./${_instdir}) || return
	eval $(stat -f "_fmode=%OMp%OLp _fown=%Su _fgrp=%Sg" ${COMPFILE}) || return

	[[ -d ${_instdir} ]] && \
		install -d -o root -g wheel -m ${_dmode} "${_instdir}" || return

	if [[ -n ${IS_LINK} ]]; then
		_linkt=$(readlink ${COMPFILE})
		(cd ${_instdir} && ln -sf ${_linkt} . && rm ${_TMPROOT}/${COMPFILE})
		return
	fi

	if [[ -f ${TARGET} ]]; then
		mkdir -p ${_BKPDIR}/${_instdir} || return
		cp -p ${TARGET} ${_BKPDIR}/${_instdir} || return
	fi

	if ! install -m ${_fmode} -o ${_fown} -g ${_fgrp} ${COMPFILE} ${_instdir}; then
		rm ${_BKPDIR}/${COMPFILE}; return 1
	fi
	rm ${COMPFILE}

	case "${TARGET}" in
	/etc/login.conf)
		if [[ -f /etc/login.conf.db ]]; then
			sm_echo " (running cap_mkdb(1), needs a relog)"
			sm_warn $(cap_mkdb /etc/login.conf 2>&1)
		else
			sm_echo
		fi
		;;
	/etc/mail/aliases)
		sm_echo " (running newaliases(8))"
		sm_warn $(newaliases 2>&1 >/dev/null)
		;;
	*)
		sm_echo
		;;
	esac
}

sm_add_user_grp() {
	local _g _gid _l _u NEWGRP NEWUSR
	local _pw="./etc/master.passwd"
	local _gr="./etc/group"

	[[ -n ${PKGMODE} ]] && return

	while read _l; do
		_u=$(echo ${_l} | awk -F ':' '{ print $1 }')
		if [[ ${_u} != "root" ]]; then
			if [[ -z $(grep -E "^${_u}:" /etc/master.passwd) ]]; then
				sm_echo "===> Adding the ${_u} user"
				chpass -la ${_l} && \
					set -A NEWUSR -- ${NEWUSR[@]} ${_u}
			fi
		fi
	done <${_pw}

	while read _l; do
		_g=$(echo ${_l} | awk -F ':' '{ print $1 }')
		_gid=$(echo ${_l} | awk -F ':' '{ print $3 }')
		if [[ -z $(grep -E "^${_g}:" /etc/group) ]]; then
			sm_echo "===> Adding the ${_g} group"
			groupadd -g ${_gid} ${_g} && \
				set -A NEWGRP -- ${NEWGRP[@]} ${_g}
		fi
	done <${_gr}
}

sm_merge_loop() {
	local INSTALL_MERGED MERGE_AGAIN
	[[ ${SM_MERGE} == sdiff* ]] && \
		echo "===> Type h at the sdiff prompt (%) to get usage help\n"
	MERGE_AGAIN=1
	while [[ -n ${MERGE_AGAIN} ]]; do
		cp -p ${COMPFILE} ${COMPFILE}.merged
		${SM_MERGE} ${COMPFILE}.merged \
			${TARGET} ${COMPFILE}
		INSTALL_MERGED=v
		while [[ ${INSTALL_MERGED} == "v" ]]; do
			echo
			echo "  Use 'e' to edit the merged file"
			echo "  Use 'i' to install the merged file"
			echo "  Use 'n' to view a diff between the merged and new files"
			echo "  Use 'o' to view a diff between the old and merged files"
			echo "  Use 'r' to re-do the merge"
			echo "  Use 'v' to view the merged file"
			echo "  Use 'x' to delete the merged file and go back to previous menu"
			echo "  Default is to leave the temporary file to deal with by hand"
			echo
			echo -n "===> How should I deal with the merged file? [Leave it for later] "
			read INSTALL_MERGED
			case ${INSTALL_MERGED} in
			[eE])
				echo "editing merged file...\n"
				${EDITOR} ${COMPFILE}.merged
				INSTALL_MERGED=v
				;;
			[iI])
				mv ${COMPFILE}.merged ${COMPFILE}
				sm_echo -n "\n===> Merging ${TARGET}"
				sm_install || \
					sm_warn "problem merging ${TARGET}"
				unset MERGE_AGAIN
				;;
			[nN])
				(
					echo "comparison between merged and new files:\n"
					diff -u ${COMPFILE}.merged ${COMPFILE}
				) | ${PAGER}
				INSTALL_MERGED=v
				;;
			[oO])
				(
					echo "comparison between old and merged files:\n"
					diff -u ${TARGET} ${COMPFILE}.merged
				) | ${PAGER}
				INSTALL_MERGED=v
				;;
			[rR])
				rm ${COMPFILE}.merged
				;;
			[vV])
				${PAGER} ${COMPFILE}.merged
				;;
			[xX])
				rm ${COMPFILE}.merged
				return 1
				;;
			'')
				sm_echo "===> ${COMPFILE} will remain for your consideration"
				unset MERGE_AGAIN
				;;
			*)
				echo "invalid choice: ${INSTALL_MERGED}"
				INSTALL_MERGED=v
				;;
			esac
		done
	done
}

sm_diff_loop() {
	local i HANDLE_COMPFILE NO_INSTALLED AUTO_INSTALLED_FILES

	[[ -n ${BATCHMODE} ]] && HANDLE_COMPFILE=todo || HANDLE_COMPFILE=v

	unset NO_INSTALLED FORCE_UPG
	while [[ ${HANDLE_COMPFILE} == @(v|todo) ]]; do
		if [[ -f ${TARGET} && -f ${COMPFILE} && -z ${IS_LINK} ]]; then
			if [[ -z ${DIFFMODE} ]]; then
				# automatically install files if current != new
				# and current = old
				for i in ${AUTO_UPG[@]}; do \
					[[ ${i} == ${COMPFILE} ]] && FORCE_UPG=1
				done
				# automatically install files which differ
				# only by CVS Id or that are binaries
				if [[ -z $(diff -q -I'[$]OpenBSD:.*$' ${TARGET} ${COMPFILE}) || \
					-n ${FORCE_UPG} || -n ${IS_BINFILE} ]]; then
					sm_echo -n "===> Updating ${TARGET}"
					sm_install && \
						AUTO_INSTALLED_FILES="${AUTO_INSTALLED_FILES}${TARGET}\n" || \
						sm_warn "problem updating ${TARGET}"
					return
				fi
			fi
			if [[ ${HANDLE_COMPFILE} == "v" ]]; then
				(
					echo "\n========================================================================\n"
					echo "===> Displaying differences between ${COMPFILE} and installed version:"
					echo
					diff -u ${TARGET} ${COMPFILE}
				) | ${PAGER}
				echo
			fi
		else
			# file does not exist on the target system
			if [[ -n ${IS_LINK} ]]; then
				if [[ -n ${DIFFMODE} ]]; then
					sm_echo && NO_INSTALLED=1
				else
					sm_echo "===> Linking ${TARGET}"
					sm_install && \
						AUTO_INSTALLED_FILES="${AUTO_INSTALLED_FILES}${TARGET}\n" || \
						sm_warn "problem creating ${TARGET} link"
					return
				fi
			fi
			if [[ -n ${DIFFMODE} ]]; then
				sm_echo && NO_INSTALLED=1
			else
				sm_echo -n "===> Installing ${TARGET}"
				sm_install && \
					AUTO_INSTALLED_FILES="${AUTO_INSTALLED_FILES}${TARGET}\n" || \
					sm_warn "problem installing ${TARGET}"
				return
			fi
		fi

		if [[ -z ${BATCHMODE} ]]; then
			echo "  Use 'd' to delete the temporary ${COMPFILE}"
			echo "  Use 'i' to install the temporary ${COMPFILE}"
			if [[ -z ${NO_INSTALLED} && -z ${IS_BINFILE} && \
				-z ${IS_LINK} ]]; then
				echo "  Use 'm' to merge the temporary and installed versions"
				echo "  Use 'v' to view the diff results again"
			fi
			echo
			echo "  Default is to leave the temporary file to deal with by hand"
			echo
			echo -n "How should I deal with this? [Leave it for later] "
			read HANDLE_COMPFILE
		else
			unset HANDLE_COMPFILE
		fi

		case ${HANDLE_COMPFILE} in
		[dD])
			rm ${COMPFILE}
			echo "\n===> Deleting ${COMPFILE}"
			;;
		[iI])
			sm_echo
			if [[ -n ${IS_LINK} ]]; then
				sm_echo "===> Linking ${TARGET}"
				sm_install && \
					MERGED_FILES="${MERGED_FILES}${TARGET}\n" || \
					sm_warn "problem creating ${TARGET} link"
			else
				sm_echo -n "===> Updating ${TARGET}"
				sm_install && \
					MERGED_FILES="${MERGED_FILES}${TARGET}\n" || \
					sm_warn "problem updating ${TARGET}"
			fi
			;;
		[mM])
			if [[ -z ${NO_INSTALLED} && -z ${IS_BINFILE} && -z ${IS_LINK} ]]; then
				sm_merge_loop && \
					MERGED_FILES="${MERGED_FILES}${TARGET}\n" || \
						HANDLE_COMPFILE="todo"
			else
				echo "invalid choice: ${HANDLE_COMPFILE}\n"
				HANDLE_COMPFILE="todo"
			fi
			;;
		[vV])
			if [[ -z ${NO_INSTALLED} && -z ${IS_BINFILE} && -z ${IS_LINK} ]]; then
				HANDLE_COMPFILE="v"
			else
				echo "invalid choice: ${HANDLE_COMPFILE}\n"
				HANDLE_COMPFILE="todo"
			fi
			;;
		'')
			sm_echo "===> ${COMPFILE} will remain for your consideration"
			;;
		*)
			echo "invalid choice: ${HANDLE_COMPFILE}\n"
			HANDLE_COMPFILE="todo"
			continue
			;;
		esac
	done
}


sm_check_an_eg() {
	[[ -n ${PKGMODE} ]] && return
	local _egmods _i _managed

	if [[ -f /usr/share/sysmerge/examplessum ]]; then
		cp /usr/share/sysmerge/examplessum ${_WRKDIR}/examplessum
		_egmods=$(cd / && \
			 sha256 -c /usr/share/sysmerge/examplessum 2>/dev/null | \
			 grep 'FAILED$' | \
			 awk '{ print $2 }' | \
			 sed -e "s,:,,")
	fi
	for _i in ${_egmods}; do
		_i=${_i##*/}
		[[ -f /etc/${_i} ]] && \
			_managed="${_managed:+${_managed} }${_i}"
	done
	# only warn for files we care about
	[[ -n ${_managed} ]] && sm_warn "example(s) changed for: ${_managed}"
	mv ./usr/share/sysmerge/examplessum \
		/usr/share/sysmerge/examplessum
}

sm_post() {
	local _i FILES_IN__TMPROOT FILES_IN__BKPDIR

	FILES_IN__TMPROOT=$(find . -type f ! -name \*.merged -size +0)
	[[ -d ${_BKPDIR} ]] && \
		FILES_IN__BKPDIR=$(find ${_BKPDIR} -type f -size +0)

	[[ -n ${FILES_IN__TMPROOT} ]] && \
		sm_warn "some files are still left for comparison"

	mtree -qdef /etc/mtree/4.4BSD.dist -p / -U >/dev/null
	[[ -d /etc/X11 ]] && \
		mtree -qdef /etc/mtree/BSD.x11.dist -p / -U >/dev/null

	if [[ -e ${_WRKDIR}/sysmerge.log ]]; then
		find . -type f -empty | xargs -r rm
		find . -type d | sort -r | xargs -r rmdir 2>/dev/null
		sed '/^$/d' ${_WRKDIR}/sysmerge.log >${_WRKDIR}/sysmerge.log.bak
		mv ${_WRKDIR}/sysmerge.log.bak ${_WRKDIR}/sysmerge.log
		echo "===> Log available at ${_WRKDIR}/sysmerge.log"
	else
		rm -rf ${_WRKDIR}
	fi
}

unset BATCHMODE DIFFMODE PKGMODE
while getopts bdp arg; do
	case ${arg} in
	b)	BATCHMODE=1;;
	d)	DIFFMODE=1;;
	p)	PKGMODE=1;;
	*)	usage;;
	esac
done
shift $(( OPTIND -1 ))
[ $# -ne 0 ] && usage

[[ $(id -u) -ne 0 ]] && echo "${0##*/}: need root privileges" && usage

# global constants
_WRKDIR=$(mktemp -d -p ${TMPDIR:=/var/tmp} sysmerge.XXXXXXXXXX) || exit 1
_BKPDIR=${_WRKDIR}/backups
_TMPROOT=${_WRKDIR}/temproot
_SWIDTH=$(stty size | awk '{w=$2} END {if (w==0) {w=80} print w}')
_RELINT=$(uname -r | tr -d '.')
readonly _WRKDIR _BKPDIR _TMPROOT _SWIDTH _RELINT

SM_MERGE=${SM_MERGE:=sdiff -as -w ${_SWIDTH} -o}
[[ -z ${VISUAL} ]] && EDITOR=${EDITOR:=/usr/bin/vi} || EDITOR=${VISUAL}
PAGER=${PAGER:=/usr/bin/more}

mkdir -p ${_TMPROOT} || sm_error "cannot create ${_TMPROOT}"
cd ${_TMPROOT} || sm_error "cannot enter ${_TMPROOT}"

sm_init
sm_post
