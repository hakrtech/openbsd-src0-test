CHTA.dlm
DLMVERSION(<vv>)
CSTRTD(40)
CINET640
buildmark.o
modules.o
ap/ap_base64.o
ap/ap_checkpass.o
ap/ap_cpystrn.o
ap/ap_ebcdic.o
ap/ap_execve.o
ap/ap_fnmatch.o
ap/ap_getpass.o
ap/ap_md5c.o
ap/ap_sha1.o
ap/ap_signal.o
ap/ap_slack.o
ap/ap_snprintf.o
ap/ap_strtol.o
main/alloc.o
main/buff.o
main/http_config.o
main/http_core.o
main/http_log.o
main/http_main.o
main/http_protocol.o
main/http_request.o
main/http_vhost.o
main/rfc1413.o
main/util.o
main/util_date.o
main/util_md5.o
main/util_script.o
main/util_uri.o
modules/standard/mod_access.o
modules/standard/mod_actions.o
modules/standard/mod_alias.o
modules/standard/mod_asis.o
modules/standard/mod_auth.o
modules/standard/mod_autoindex.o
modules/standard/mod_cgi.o
modules/standard/mod_dir.o
modules/standard/mod_env.o
modules/standard/mod_imap.o
modules/standard/mod_include.o
modules/standard/mod_log_config.o
modules/standard/mod_mime.o
modules/standard/mod_negotiation.o
modules/standard/mod_setenvif.o
modules/standard/mod_status.o
modules/standard/mod_userdir.o
os/tpf/cgetop.o
os/tpf/os.o
os/tpf/os-inline.o
regex/regcomp.o
regex/regerror.o
regex/regexec.o
regex/regfree.o
SYSLIB(<your-first-syslib-dsn>)
SYSLIB(<your-final-syslib-dsn>)
OBJLIB(<your-first-objlib-dsn>)
OBJLIB(<your-final-objlib-dsn>)
STUBS()
TARGET(<your-target-dsn-here>)
PRELINK(MAP)
LINK(AMODE=31 RMODE=ANY LIST XREF MAP)
