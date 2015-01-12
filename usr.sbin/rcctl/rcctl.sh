#!/bin/sh
#
# $OpenBSD: rcctl.sh,v 1.62 2015/01/12 13:51:20 ajacoutot Exp $
#
# Copyright (c) 2014, 2015 Antoine Jacoutot <ajacoutot@openbsd.org>
# Copyright (c) 2014 Ingo Schwarze <schwarze@openbsd.org>
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

_special_services="accounting check_quotas ipsec multicast_host multicast_router pf spamd_black"
readonly _special_services

# get local functions from rc.subr(8)
FUNCS_ONLY=1
. /etc/rc.d/rc.subr
_rc_parse_conf

usage()
{
	_rc_err "usage: ${0##*/} [-df] enable|disable|get|set|getdef|getall|order|action
             [service | daemon [variable [arguments]] | daemons]"
}

needs_root()
{
	[ "$(id -u)" -ne 0 ] && _rc_err "${0##*/} $1: need root privileges"
}

ls_rcscripts() {
	local _s

	cd /etc/rc.d && set -- *
	for _s; do
		[ "${_s}" = "rc.subr" ] && continue
		[ ! -d "${_s}" ] && echo "${_s}"
	done
}

pkg_scripts_append()
{
	local _svc=$1
	[ -n "${_svc}" ] || return

	rcconf_edit_begin
	if [ -z "${pkg_scripts}" ]; then
		echo pkg_scripts="${_svc}" >>${_TMP_RCCONF}
	elif ! echo ${pkg_scripts} | grep -qw -- ${_svc}; then
		grep -v "^pkg_scripts.*=" /etc/rc.conf.local >${_TMP_RCCONF}
		echo pkg_scripts="${pkg_scripts} ${_svc}" >>${_TMP_RCCONF}
	fi
	rcconf_edit_end
}

pkg_scripts_order()
{
	local _svcs="$*"
	[ -n "${_svcs}" ] || return

	needs_root ${action}
	local _pkg_scripts _svc
	for _svc in ${_svcs}; do
		if svc_is_base ${_svc} || svc_is_special ${_svc}; then
			_rc_err "${0##*/}: ${_svc} is not a pkg script"
		elif ! svc_get ${_svc} status; then
			_rc_err "${0##*/}: ${_svc} is not enabled"
		fi
	done
	_pkg_scripts=$(echo "${_svcs} ${pkg_scripts}" | tr "[:blank:]" "\n" | \
		     awk -v ORS=' ' '!x[$0]++')
	rcconf_edit_begin
	grep -v "^pkg_scripts.*=" /etc/rc.conf.local >${_TMP_RCCONF}
	echo pkg_scripts=${_pkg_scripts} >>${_TMP_RCCONF}
	rcconf_edit_end
}

pkg_scripts_rm()
{
	local _svc=$1
	[ -n "${_svc}" ] || return

	[ -z "${pkg_scripts}" ] && return

	rcconf_edit_begin
	sed "/^pkg_scripts[[:>:]]/{s/[[:<:]]${_svc}[[:>:]]//g
	    s/['\"]//g;s/ *= */=/;s/   */ /g;s/ $//;/=$/d;}" \
	    /etc/rc.conf.local >${_TMP_RCCONF}
	rcconf_edit_end
}

rcconf_edit_begin()
{
	_TMP_RCCONF=$(mktemp -p /etc -t rc.conf.local.XXXXXXXXXX) || exit 1
	if [ -f /etc/rc.conf.local ]; then
		# only to keep permissions (file content is not needed)
		cp -p /etc/rc.conf.local ${_TMP_RCCONF} || exit 1
	else
		touch /etc/rc.conf.local || exit 1
	fi
}

rcconf_edit_end()
{
	sort -u -o ${_TMP_RCCONF} ${_TMP_RCCONF} || exit 1
	mv ${_TMP_RCCONF} /etc/rc.conf.local || exit 1
	if [ ! -s /etc/rc.conf.local ]; then
		rm /etc/rc.conf.local || exit 1
	fi
	_rc_parse_conf # reload new values
}

svc_is_avail()
{
	local _svc=$1
	[ -n "${_svc}" ] || return

	[ -x "/etc/rc.d/${_svc}" ] && return 0
	svc_is_special ${_svc}
}

svc_is_base()
{
	local _svc=$1
	[ -n "${_svc}" ] || return

	grep "^start_daemon " /etc/rc | cut -d ' ' -f2- | grep -qw -- ${_svc}
}

svc_is_special()
{
	local _svc=$1
	[ -n "${_svc}" ] || return

	echo ${_special_services} | grep -qw -- ${_svc}
}

svc_get()
{
	local _svc=$1
	[ -n "${_svc}" ] || return

	local _status=0 _val _var=$2
	local daemon_flags daemon_timeout daemon_user

	if svc_is_special ${_svc}; then
		daemon_flags="$(eval echo \${${_svc}})"
	else
		# set pkg daemon_flags to "NO" to match base svc
		if ! svc_is_base ${_svc}; then
			if ! echo ${pkg_scripts} | grep -qw -- ${_svc}; then
				daemon_flags="NO"
			fi
		fi

		[ -z "${daemon_flags}" ] && \
			daemon_flags="$(eval echo \"\${${_svc}_flags}\")"
		[ -z "${daemon_flags}" ] && \
			daemon_flags="$(svc_getdef ${_svc} flags)"
		[ -z "${daemon_timeout}" ] && \
			daemon_timeout="$(eval echo \"\${${_svc}_timeout}\")"
		[ -z "${daemon_timeout}" ] && \
			daemon_timeout="$(svc_getdef ${_svc} timeout)"
		[ -z "${daemon_user}" ] && \
			daemon_user="$(eval echo \"\${${_svc}_user}\")"
		[ -z "${daemon_user}" ] && \
			daemon_user="$(svc_getdef ${_svc} user)"
	fi

	[ "${daemon_flags}" = "NO" ] && _status=1

	if [ -n "${_var}" ]; then
		[ "${_var}" = "status" ] && return ${_status}
		eval _val=\${daemon_${_var}}
		[ -z "${_val}" ] || print -r -- "${_val}"
	else
		if svc_is_special ${_svc}; then
			echo "${_svc}=${daemon_flags}"
		else
			echo "${_svc}_flags=${daemon_flags}"
			echo "${_svc}_timeout=${daemon_timeout}"
			echo "${_svc}_user=${daemon_user}"
		fi
		return ${_status}
	fi
}

# to prevent namespace pollution, only call in a subshell
svc_getdef()
{
	local _svc=$1
	[ -n "${_svc}" ] || return

	local _status=0 _val _var=$2
	local daemon_flags daemon_timeout daemon_user

	if svc_is_special ${_svc}; then
		# unconditionally parse: we always output flags and/or status
		_rc_parse_conf /etc/rc.conf
		daemon_flags="$(eval echo \${${_svc}})"
		[ "${daemon_flags}" = "NO" ] && _status=1
	else
		if ! svc_is_base ${_svc}; then
			_status=1 # all pkg_scripts are off by default
		else
			
			# abuse /etc/rc.conf behavior of only setting flags
			# to empty or "NO" to get our default status;
			# we'll get our default flags from the rc.d script
			[[ -z ${_var} || ${_var} == status ]] && \
				_rc_parse_conf /etc/rc.conf
			[ "$(eval echo \${${_svc}_flags})" = "NO" ] && _status=1
		fi

		rc_cmd() { }
		. /etc/rc.d/${_svc} >/dev/null 2>&1

		[ -z "${daemon_timeout}" ] && daemon_timeout=30
		[ -z "${daemon_user}" ] && daemon_user=root
	fi

	if [ -n "${_var}" ]; then
		[ "${_var}" = "status" ] && return ${_status}
		eval _val=\${daemon_${_var}}
		[ -z "${_val}" ] || print -r -- "${_val}"
	else
		if svc_is_special ${_svc}; then
			echo "${_svc}=${daemon_flags}"
		else
			echo "${_svc}_flags=${daemon_flags}"
			echo "${_svc}_timeout=${daemon_timeout}"
			echo "${_svc}_user=${daemon_user}"
		fi
		return ${_status}
	fi
}

svc_rm()
{
	local _svc=$1
	[ -n "${_svc}" ] || return

	rcconf_edit_begin
	if svc_is_special ${_svc}; then
		grep -v "^${_svc}.*=" /etc/rc.conf.local >${_TMP_RCCONF}
		( svc_getdef ${_svc} status ) && \
			echo "${_svc}=NO" >>${_TMP_RCCONF}
	else
		grep -Ev "^${_svc}_(flags|timeout|user).*=" \
			/etc/rc.conf.local >${_TMP_RCCONF}
		( svc_getdef ${_svc} status ) && \
			echo "${_svc}_flags=NO" >>${_TMP_RCCONF}
	fi
	rcconf_edit_end
}

svc_set()
{
	local _svc=$1 _var=$2
	[ -n "${_svc}" -a -n "${_var}" ] || return

	shift 2
	local _args="$*"

	if [ "${_var}" = "status" ]; then
		if [ "${_args}" = "on" ]; then
			_var="flags"
			# keep our flags if we're already enabled
			eval "_args=\"\${${_svc}_${_var}}\""
			[ "${_args}" = "NO" ] && unset _args
			if ! svc_is_base ${_svc} && ! svc_is_special ${_svc}; then
				pkg_scripts_append ${_svc}
			fi
		elif [ "${_args}" = "off" ]; then
			if ! svc_is_base ${_svc} && ! svc_is_special ${_svc}; then
				pkg_scripts_rm ${_svc}
			fi
			svc_rm ${_svc}
			return
		else
			_rc_err "${0##*/}: invalid status \"${_args}\""
		fi
	else
		svc_get ${_svc} status || \
			_rc_err "${0##*/}: ${svc} is not enabled"
	fi

	if svc_is_special ${_svc}; then
		[ "${_var}" = "flags" ] || return
		rcconf_edit_begin
		grep -v "^${_svc}.*=" /etc/rc.conf.local >${_TMP_RCCONF}
		( svc_getdef ${_svc} status ) || \
			echo "${_svc}=YES" >>${_TMP_RCCONF}
		rcconf_edit_end
		return
	fi

	if [ -n "${_args}" ]; then
		if [ "${_var}" = "timeout" ]; then
			[[ ${_args} != +([[:digit:]]) || ${_args} -le 0 ]] && \
				_rc_err "${0##*/}: \"${_args}\" is not a positive integer"
		fi
		# unset flags if they match the default enabled ones
		[ "${_args}" = "$(svc_getdef ${_svc} ${_var})" ] && \
			unset _args
	fi

	# protect leading whitespace
	[ "${_args}" = "${_args# }" ] || _args="\"${_args}\""

	# reset: value may have changed
	unset ${_svc}_${_var}

	rcconf_edit_begin
	grep -v "^${_svc}_${_var}.*=" /etc/rc.conf.local >${_TMP_RCCONF}
	if [ -n "${_args}" ] || \
	   ( svc_is_base ${_svc} && ! svc_getdef ${_svc} status && [ "${_var}" == "flags" ] ); then
		echo "${_svc}_${_var}=${_args}" >>${_TMP_RCCONF}
	fi
	rcconf_edit_end
}

unset _RC_DEBUG _RC_FORCE
while getopts "df" c; do
	case "$c" in
		d) _RC_DEBUG=-d;;
		f) _RC_FORCE=-f;;
		*) usage;;
	esac
done
shift $((OPTIND-1))
[ $# -gt 0 ] || usage

action=$1
if [ "${action}" = "order" ]; then
	shift 1
	svcs="$*"
else
	svc=$2
	var=$3
	[ $# -ge 3 ] && shift 3 || shift $#
	args="$*"
fi

if [ -n "${svc}" ]; then
	[[ ${action} = getall ]] && usage
	svc_is_avail ${svc} || \
		_rc_err "${0##*/}: service ${svc} does not exist" 2
elif [[ ${action} != @(getall|order|status) ]] ; then
	usage
fi

if [ -n "${var}" ]; then
	[[ ${var} != @(flags|status|timeout|user) ]] && usage
	[[ ${action} != @(enable|get|getdef|set) ]] && usage
	[[ ${action} == @(enable|set) && ${var} = flags && ${args} = NO ]] && \
		_rc_err "${0##*/}: \"flags NO\" contradicts \"${action}\""
	if svc_is_special ${svc}; then
		if [[ ${action} == @(enable|set) && ${var} != status ]] || \
			[[ ${action} == @(get|getdef) && ${var} == @(timeout|user) ]] ; then
			_rc_err "${0##*/}: \"${svc}\" is a special variable, cannot \"${action} ${svc} ${var}\""
		fi
	fi
	[[ ${action} == enable && ${var} != flags ]] && \
		_rc_err "${0##*/}: invalid action \"${action} ${svc} ${var}\""
elif [ ${action} = "set" ]; then
	usage
fi

case ${action} in
	disable)
		needs_root ${action}
		svc_set ${svc} status off
		;;
	enable)
		needs_root ${action}
		svc_set ${svc} status on
		# XXX backward compat
		if [ -n "${var}" ]; then
			svc_set ${svc} "${var}" "${args}"
		fi
		;;
	get)
		svc_get ${svc} "${var}"
		;;
	getall)
		for i in $(ls_rcscripts) ${_special_services}; do
			svc_get ${i}
		done
		return 0 # we do not want the "status"
		;;
	getdef)
		( svc_getdef ${svc} "${var}" )
		;;
	order)
		if [ -n "${svcs}" ]; then
			needs_root ${action}
			pkg_scripts_order ${svcs}
		else
			[[ -z ${pkg_scripts} ]] || echo ${pkg_scripts}
		fi
		;;
	set)
		needs_root ${action}
		svc_set ${svc} "${var}" "${args}"
		;;
	status) # XXX backward compat
		if [ -n "${svc}" ]; then
			svc_get ${svc} flags
			svc_get ${svc} status
		else
			for i in $(ls_rcscripts) ${_special_services}; do
				svc_get ${i} | grep -Ev '_(timeout|user)'
			done
			return 0 # we do not want the "status"
		fi
		;;
	start|stop|restart|reload|check)
		if svc_is_special ${svc}; then
			_rc_err "${0##*/}: \"${svc}\" is a special variable, no rc.d(8) script"
		fi
		/etc/rc.d/${svc} ${_RC_DEBUG} ${_RC_FORCE} ${action}
		;;
	*)
		usage
		;;
esac
