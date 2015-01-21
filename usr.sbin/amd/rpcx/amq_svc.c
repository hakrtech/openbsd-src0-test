/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "amq.h"
#include "am.h"
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>/* getenv, exit */
#include <rpc/pmap_clnt.h> /* for pmap_unset */
#include <string.h> /* strcmp */ 
#include <netdb.h>
#include <signal.h>
#include <sys/ttycom.h>/* TIOCNOTTY */
#ifdef __cplusplus
#include <sysent.h> /* getdtablesize, open */
#endif /* __cplusplus */
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <syslog.h>
#include <errno.h>

#ifdef __STDC__
#define SIG_PF void(*)(int)
#endif

#ifdef DEBUG
#define RPC_SVC_FG
#endif

#define _RPCSVC_CLOSEDOWN 120
extern int _rpcpmstart;		/* Started by a port monitor ? */
extern int _rpcfdtype;		/* Whether Stream or Datagram ? */


void	amq_program_57(struct svc_req *rqstp, SVCXPRT *transp);

void
amq_program_57(struct svc_req *rqstp, SVCXPRT *transp)
{
	union {
		amq_string amqproc_mnttree_57_arg;
		amq_string amqproc_umnt_57_arg;
		amq_setopt amqproc_setopt_57_arg;
	} argument;
	char *result;
	xdrproc_t xdr_argument, xdr_result;
	char *(*local)(char *, struct svc_req *);
	extern SVCXPRT *lamqp;

	if (transp != lamqp) {
		struct sockaddr_in *fromsin = svc_getcaller(transp);

		syslog(LOG_WARNING,
		    "non-local amq attempt (might be from %s)",
		    inet_ntoa(fromsin->sin_addr));
		svcerr_noproc(transp);
		return;
	}

	switch (rqstp->rq_proc) {
	case AMQPROC_NULL:
		xdr_argument = (xdrproc_t) xdr_void;
		xdr_result = (xdrproc_t) xdr_void;
		local = (char *(*)(char *, struct svc_req *)) amqproc_null_57_svc;
		break;

	case AMQPROC_MNTTREE:
		xdr_argument = (xdrproc_t) xdr_amq_string;
		xdr_result = (xdrproc_t) xdr_amq_mount_tree_p;
		local = (char *(*)(char *, struct svc_req *)) amqproc_mnttree_57_svc;
		break;

	case AMQPROC_UMNT:
		xdr_argument = (xdrproc_t) xdr_amq_string;
		xdr_result = (xdrproc_t) xdr_void;
		local = (char *(*)(char *, struct svc_req *)) amqproc_umnt_57_svc;
		break;

	case AMQPROC_STATS:
		xdr_argument = (xdrproc_t) xdr_void;
		xdr_result = (xdrproc_t) xdr_amq_mount_stats;
		local = (char *(*)(char *, struct svc_req *)) amqproc_stats_57_svc;
		break;

	case AMQPROC_EXPORT:
		xdr_argument = (xdrproc_t) xdr_void;
		xdr_result = (xdrproc_t) xdr_amq_mount_tree_list;
		local = (char *(*)(char *, struct svc_req *)) amqproc_export_57_svc;
		break;

	case AMQPROC_SETOPT:
		xdr_argument = (xdrproc_t) xdr_amq_setopt;
		xdr_result = (xdrproc_t) xdr_int;
		local = (char *(*)(char *, struct svc_req *)) amqproc_setopt_57_svc;
		break;

	case AMQPROC_GETMNTFS:
		xdr_argument = (xdrproc_t) xdr_void;
		xdr_result = (xdrproc_t) xdr_amq_mount_info_list;
		local = (char *(*)(char *, struct svc_req *)) amqproc_getmntfs_57_svc;
		break;

	case AMQPROC_GETVERS:
		xdr_argument = (xdrproc_t) xdr_void;
		xdr_result = (xdrproc_t) xdr_amq_string;
		local = (char *(*)(char *, struct svc_req *)) amqproc_getvers_57_svc;
		break;

	default:
		svcerr_noproc(transp);
		return;
	}
	(void) memset((char *)&argument, 0, sizeof (argument));
	if (!svc_getargs(transp, xdr_argument, (caddr_t) &argument)) {
		svcerr_decode(transp);
		return;
	}
	result = (*local)((char *)&argument, rqstp);
	if (result != NULL && !svc_sendreply(transp, xdr_result, result)) {
		svcerr_systemerr(transp);
	}
	if (!svc_freeargs(transp, xdr_argument, (caddr_t) &argument)) {
		plog(XLOG_FATAL, "unable to free rpc arguments in amqprog");
		going_down(1);
	}
	return;
}
