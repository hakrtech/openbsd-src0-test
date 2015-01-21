/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include <memory.h> /* for memset */
#include "amq.h"

/* Default timeout can be changed using clnt_control() */
static struct timeval TIMEOUT = { 25, 0 };

void *
amqproc_null_57(void *argp, CLIENT *clnt)
{
	static char clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call(clnt, AMQPROC_NULL, xdr_void, argp, xdr_void, &clnt_res, TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return ((void *)&clnt_res);
}

amq_mount_tree_p *
amqproc_mnttree_57(amq_string *argp, CLIENT *clnt)
{
	static amq_mount_tree_p clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call(clnt, AMQPROC_MNTTREE, xdr_amq_string, argp, xdr_amq_mount_tree_p, &clnt_res, TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

void *
amqproc_umnt_57(amq_string *argp, CLIENT *clnt)
{
	static char clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call(clnt, AMQPROC_UMNT, xdr_amq_string, argp, xdr_void, &clnt_res, TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return ((void *)&clnt_res);
}

amq_mount_stats *
amqproc_stats_57(void *argp, CLIENT *clnt)
{
	static amq_mount_stats clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call(clnt, AMQPROC_STATS, xdr_void, argp, xdr_amq_mount_stats, &clnt_res, TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

amq_mount_tree_list *
amqproc_export_57(void *argp, CLIENT *clnt)
{
	static amq_mount_tree_list clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call(clnt, AMQPROC_EXPORT, xdr_void, argp, xdr_amq_mount_tree_list, &clnt_res, TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

int *
amqproc_setopt_57(amq_setopt *argp, CLIENT *clnt)
{
	static int clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call(clnt, AMQPROC_SETOPT, xdr_amq_setopt, argp, xdr_int, &clnt_res, TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

amq_mount_info_list *
amqproc_getmntfs_57(void *argp, CLIENT *clnt)
{
	static amq_mount_info_list clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call(clnt, AMQPROC_GETMNTFS, xdr_void, argp, xdr_amq_mount_info_list, &clnt_res, TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

amq_string *
amqproc_getvers_57(void *argp, CLIENT *clnt)
{
	static amq_string clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call(clnt, AMQPROC_GETVERS, xdr_void, argp, xdr_amq_string, &clnt_res, TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}
