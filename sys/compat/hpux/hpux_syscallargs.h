/*	$OpenBSD: hpux_syscallargs.h,v 1.8 2001/08/26 04:14:26 deraadt Exp $	*/

/*
 * System call argument lists.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	OpenBSD: syscalls.master,v 1.6 1999/06/07 07:17:46 deraadt Exp 
 */

#ifdef	syscallarg
#undef	syscallarg
#endif

#define	syscallarg(x)							\
	union {								\
		register_t pad;						\
		struct { x datum; } le;					\
		struct {						\
			int8_t pad[ (sizeof (register_t) < sizeof (x))	\
				? 0					\
				: sizeof (register_t) - sizeof (x)];	\
			x datum;					\
		} be;							\
	}

struct hpux_sys_read_args {
	syscallarg(int) fd;
	syscallarg(char *) buf;
	syscallarg(u_int) nbyte;
};

struct hpux_sys_write_args {
	syscallarg(int) fd;
	syscallarg(char *) buf;
	syscallarg(u_int) nbyte;
};

struct hpux_sys_open_args {
	syscallarg(char *) path;
	syscallarg(int) flags;
	syscallarg(int) mode;
};

struct hpux_sys_wait_args {
	syscallarg(int *) status;
};

struct hpux_sys_creat_args {
	syscallarg(char *) path;
	syscallarg(int) mode;
};

struct hpux_sys_unlink_args {
	syscallarg(char *) path;
};

struct hpux_sys_execv_args {
	syscallarg(char *) path;
	syscallarg(char **) argp;
};

struct hpux_sys_chdir_args {
	syscallarg(char *) path;
};

struct hpux_sys_time_6x_args {
	syscallarg(time_t *) t;
};

struct hpux_sys_mknod_args {
	syscallarg(char *) path;
	syscallarg(int) mode;
	syscallarg(int) dev;
};

struct hpux_sys_chmod_args {
	syscallarg(char *) path;
	syscallarg(int) mode;
};

struct hpux_sys_chown_args {
	syscallarg(char *) path;
	syscallarg(int) uid;
	syscallarg(int) gid;
};

struct hpux_sys_stat_6x_args {
	syscallarg(char *) path;
	syscallarg(struct hpux_ostat *) sb;
};

struct hpux_sys_stime_6x_args {
	syscallarg(int) time;
};

struct hpux_sys_ptrace_args {
	syscallarg(int) req;
	syscallarg(int) pid;
	syscallarg(int *) addr;
	syscallarg(int) data;
};

struct hpux_sys_alarm_6x_args {
	syscallarg(int) deltat;
};

struct hpux_sys_fstat_6x_args {
	syscallarg(int) fd;
	syscallarg(struct hpux_ostat *) sb;
};

struct hpux_sys_utime_6x_args {
	syscallarg(char *) fname;
	syscallarg(time_t *) tptr;
};

struct hpux_sys_stty_6x_args {
	syscallarg(int) fd;
	syscallarg(caddr_t) arg;
};

struct hpux_sys_gtty_6x_args {
	syscallarg(int) fd;
	syscallarg(caddr_t) arg;
};

struct hpux_sys_access_args {
	syscallarg(char *) path;
	syscallarg(int) flags;
};

struct hpux_sys_nice_6x_args {
	syscallarg(int) nval;
};

struct hpux_sys_ftime_6x_args {
	syscallarg(struct hpux_timeb *) tp;
};

struct hpux_sys_kill_args {
	syscallarg(pid_t) pid;
	syscallarg(int) signo;
};

struct hpux_sys_stat_args {
	syscallarg(char *) path;
	syscallarg(struct hpux_stat *) sb;
};

struct hpux_sys_lstat_args {
	syscallarg(char *) path;
	syscallarg(struct hpux_stat *) sb;
};

struct hpux_sys_dup_args {
	syscallarg(int) fd;
};

struct hpux_sys_times_6x_args {
	syscallarg(struct tms *) tms;
};

struct hpux_sys_ssig_6x_args {
	syscallarg(int) signo;
	syscallarg(sig_t) fun;
};

struct hpux_sys_ioctl_args {
	syscallarg(int) fd;
	syscallarg(int) com;
	syscallarg(caddr_t) data;
};

struct hpux_sys_symlink_args {
	syscallarg(char *) path;
	syscallarg(char *) link;
};

struct hpux_sys_utssys_args {
	syscallarg(struct hpux_utsname *) uts;
	syscallarg(int) dev;
	syscallarg(int) request;
};

struct hpux_sys_readlink_args {
	syscallarg(char *) path;
	syscallarg(char *) buf;
	syscallarg(int) count;
};

struct hpux_sys_execve_args {
	syscallarg(char *) path;
	syscallarg(char **) argp;
	syscallarg(char **) envp;
};

struct hpux_sys_fcntl_args {
	syscallarg(int) fd;
	syscallarg(int) cmd;
	syscallarg(int) arg;
};

struct hpux_sys_ulimit_args {
	syscallarg(int) cmd;
	syscallarg(int) newlimit;
};

struct hpux_sys_mmap_args {
	syscallarg(caddr_t) addr;
	syscallarg(size_t) len;
	syscallarg(int) prot;
	syscallarg(int) flags;
	syscallarg(int) fd;
	syscallarg(long) pos;
};

struct hpux_sys_getpgrp2_args {
	syscallarg(pid_t) pid;
};

struct hpux_sys_setpgrp2_args {
	syscallarg(pid_t) pid;
	syscallarg(pid_t) pgid;
};

struct hpux_sys_wait3_args {
	syscallarg(int *) status;
	syscallarg(int) options;
	syscallarg(int) rusage;
};

struct hpux_sys_fstat_args {
	syscallarg(int) fd;
	syscallarg(struct hpux_stat *) sb;
};

struct hpux_sys_sigreturn_args {
	syscallarg(struct hpuxsigcontext *) sigcntxp;
};

struct hpux_sys_sigvec_args {
	syscallarg(int) signo;
	syscallarg(struct sigvec *) nsv;
	syscallarg(struct sigvec *) osv;
};

struct hpux_sys_sigblock_args {
	syscallarg(int) mask;
};

struct hpux_sys_sigsetmask_args {
	syscallarg(int) mask;
};

struct hpux_sys_sigpause_args {
	syscallarg(int) mask;
};

struct hpux_sys_readv_args {
	syscallarg(int) fd;
	syscallarg(struct iovec *) iovp;
	syscallarg(u_int) iovcnt;
};

struct hpux_sys_writev_args {
	syscallarg(int) fd;
	syscallarg(struct iovec *) iovp;
	syscallarg(u_int) iovcnt;
};

struct hpux_sys_setresuid_args {
	syscallarg(uid_t) r;
	syscallarg(uid_t) e;
	syscallarg(uid_t) s;
};

struct hpux_sys_setresgid_args {
	syscallarg(gid_t) r;
	syscallarg(gid_t) e;
	syscallarg(gid_t) s;
};

struct hpux_sys_rename_args {
	syscallarg(char *) from;
	syscallarg(char *) to;
};

struct hpux_sys_truncate_args {
	syscallarg(char *) path;
	syscallarg(long) length;
};

struct hpux_sys_sysconf_args {
	syscallarg(int) name;
};

struct hpux_sys_mkdir_args {
	syscallarg(char *) path;
	syscallarg(int) mode;
};

struct hpux_sys_rmdir_args {
	syscallarg(char *) path;
};

struct hpux_sys_getrlimit_args {
	syscallarg(u_int) which;
	syscallarg(struct ogetrlimit *) rlp;
};

struct hpux_sys_setrlimit_args {
	syscallarg(u_int) which;
	syscallarg(struct ogetrlimit *) rlp;
};

struct hpux_sys_rtprio_args {
	syscallarg(pid_t) pid;
	syscallarg(int) prio;
};

struct hpux_sys_netioctl_args {
	syscallarg(int) call;
	syscallarg(int *) args;
};

struct hpux_sys_lockf_args {
	syscallarg(int) fd;
	syscallarg(int) func;
	syscallarg(long) size;
};

struct hpux_sys_shmctl_args {
	syscallarg(int) shmid;
	syscallarg(int) cmd;
	syscallarg(caddr_t) buf;
};

struct hpux_sys_advise_args {
	syscallarg(int) arg;
};

struct hpux_sys_getcontext_args {
	syscallarg(char *) buf;
	syscallarg(int) len;
};

struct hpux_sys_getaccess_args {
	syscallarg(char *) path;
	syscallarg(uid_t) uid;
	syscallarg(int) ngroups;
	syscallarg(gid_t *) gidset;
	syscallarg(void *) label;
	syscallarg(void *) privs;
};

struct hpux_sys_waitpid_args {
	syscallarg(pid_t) pid;
	syscallarg(int *) status;
	syscallarg(int) options;
	syscallarg(struct rusage *) rusage;
};

struct hpux_sys_sigaction_args {
	syscallarg(int) signo;
	syscallarg(struct hpux_sigaction *) nsa;
	syscallarg(struct hpux_sigaction *) osa;
};

struct hpux_sys_sigprocmask_args {
	syscallarg(int) how;
	syscallarg(hpux_sigset_t *) set;
	syscallarg(hpux_sigset_t *) oset;
};

struct hpux_sys_sigpending_args {
	syscallarg(hpux_sigset_t *) set;
};

struct hpux_sys_sigsuspend_args {
	syscallarg(hpux_sigset_t *) set;
};

struct hpux_sys_setsockopt2_args {
	syscallarg(int) s;
	syscallarg(int) level;
	syscallarg(int) name;
	syscallarg(caddr_t) val;
	syscallarg(int) valsize;
};

struct hpux_sys_nshmctl_args {
	syscallarg(int) shmid;
	syscallarg(int) cmd;
	syscallarg(caddr_t) buf;
};

/*
 * System call prototypes.
 */

int	sys_nosys	__P((struct proc *, void *, register_t *));
int	sys_exit	__P((struct proc *, void *, register_t *));
int	hpux_sys_fork	__P((struct proc *, void *, register_t *));
int	hpux_sys_read	__P((struct proc *, void *, register_t *));
int	hpux_sys_write	__P((struct proc *, void *, register_t *));
int	hpux_sys_open	__P((struct proc *, void *, register_t *));
int	sys_close	__P((struct proc *, void *, register_t *));
int	hpux_sys_wait	__P((struct proc *, void *, register_t *));
int	hpux_sys_creat	__P((struct proc *, void *, register_t *));
int	sys_link	__P((struct proc *, void *, register_t *));
int	hpux_sys_unlink	__P((struct proc *, void *, register_t *));
int	hpux_sys_execv	__P((struct proc *, void *, register_t *));
int	hpux_sys_chdir	__P((struct proc *, void *, register_t *));
int	hpux_sys_time_6x	__P((struct proc *, void *, register_t *));
int	hpux_sys_mknod	__P((struct proc *, void *, register_t *));
int	hpux_sys_chmod	__P((struct proc *, void *, register_t *));
int	hpux_sys_chown	__P((struct proc *, void *, register_t *));
int	sys_obreak	__P((struct proc *, void *, register_t *));
int	hpux_sys_stat_6x	__P((struct proc *, void *, register_t *));
int	compat_43_sys_lseek	__P((struct proc *, void *, register_t *));
int	sys_getpid	__P((struct proc *, void *, register_t *));
int	sys_setuid	__P((struct proc *, void *, register_t *));
int	sys_getuid	__P((struct proc *, void *, register_t *));
int	hpux_sys_stime_6x	__P((struct proc *, void *, register_t *));
int	hpux_sys_ptrace	__P((struct proc *, void *, register_t *));
int	hpux_sys_alarm_6x	__P((struct proc *, void *, register_t *));
int	hpux_sys_fstat_6x	__P((struct proc *, void *, register_t *));
int	hpux_sys_pause_6x	__P((struct proc *, void *, register_t *));
int	hpux_sys_utime_6x	__P((struct proc *, void *, register_t *));
int	hpux_sys_stty_6x	__P((struct proc *, void *, register_t *));
int	hpux_sys_gtty_6x	__P((struct proc *, void *, register_t *));
int	hpux_sys_access	__P((struct proc *, void *, register_t *));
int	hpux_sys_nice_6x	__P((struct proc *, void *, register_t *));
int	hpux_sys_ftime_6x	__P((struct proc *, void *, register_t *));
int	sys_sync	__P((struct proc *, void *, register_t *));
int	hpux_sys_kill	__P((struct proc *, void *, register_t *));
int	hpux_sys_stat	__P((struct proc *, void *, register_t *));
int	hpux_sys_setpgrp_6x	__P((struct proc *, void *, register_t *));
int	hpux_sys_lstat	__P((struct proc *, void *, register_t *));
int	hpux_sys_dup	__P((struct proc *, void *, register_t *));
int	sys_opipe	__P((struct proc *, void *, register_t *));
int	hpux_sys_times_6x	__P((struct proc *, void *, register_t *));
int	sys_profil	__P((struct proc *, void *, register_t *));
int	sys_setgid	__P((struct proc *, void *, register_t *));
int	sys_getgid	__P((struct proc *, void *, register_t *));
int	hpux_sys_ssig_6x	__P((struct proc *, void *, register_t *));
int	hpux_sys_ioctl	__P((struct proc *, void *, register_t *));
int	hpux_sys_symlink	__P((struct proc *, void *, register_t *));
int	hpux_sys_utssys	__P((struct proc *, void *, register_t *));
int	hpux_sys_readlink	__P((struct proc *, void *, register_t *));
int	hpux_sys_execve	__P((struct proc *, void *, register_t *));
int	sys_umask	__P((struct proc *, void *, register_t *));
int	sys_chroot	__P((struct proc *, void *, register_t *));
int	hpux_sys_fcntl	__P((struct proc *, void *, register_t *));
int	hpux_sys_ulimit	__P((struct proc *, void *, register_t *));
int	hpux_sys_vfork	__P((struct proc *, void *, register_t *));
int	hpux_sys_read	__P((struct proc *, void *, register_t *));
int	hpux_sys_write	__P((struct proc *, void *, register_t *));
int	hpux_sys_mmap	__P((struct proc *, void *, register_t *));
int	sys_munmap	__P((struct proc *, void *, register_t *));
int	sys_mprotect	__P((struct proc *, void *, register_t *));
int	sys_getgroups	__P((struct proc *, void *, register_t *));
int	sys_setgroups	__P((struct proc *, void *, register_t *));
int	hpux_sys_getpgrp2	__P((struct proc *, void *, register_t *));
int	hpux_sys_setpgrp2	__P((struct proc *, void *, register_t *));
int	sys_setitimer	__P((struct proc *, void *, register_t *));
int	hpux_sys_wait3	__P((struct proc *, void *, register_t *));
int	sys_getitimer	__P((struct proc *, void *, register_t *));
int	sys_dup2	__P((struct proc *, void *, register_t *));
int	hpux_sys_fstat	__P((struct proc *, void *, register_t *));
int	sys_select	__P((struct proc *, void *, register_t *));
int	sys_fsync	__P((struct proc *, void *, register_t *));
int	hpux_sys_sigreturn	__P((struct proc *, void *, register_t *));
int	hpux_sys_sigvec	__P((struct proc *, void *, register_t *));
int	hpux_sys_sigblock	__P((struct proc *, void *, register_t *));
int	hpux_sys_sigsetmask	__P((struct proc *, void *, register_t *));
int	hpux_sys_sigpause	__P((struct proc *, void *, register_t *));
int	compat_43_sys_sigstack	__P((struct proc *, void *, register_t *));
int	sys_gettimeofday	__P((struct proc *, void *, register_t *));
int	hpux_sys_readv	__P((struct proc *, void *, register_t *));
int	hpux_sys_writev	__P((struct proc *, void *, register_t *));
int	sys_settimeofday	__P((struct proc *, void *, register_t *));
int	sys_fchown	__P((struct proc *, void *, register_t *));
int	sys_fchmod	__P((struct proc *, void *, register_t *));
int	hpux_sys_setresuid	__P((struct proc *, void *, register_t *));
int	hpux_sys_setresgid	__P((struct proc *, void *, register_t *));
int	hpux_sys_rename	__P((struct proc *, void *, register_t *));
int	hpux_sys_truncate	__P((struct proc *, void *, register_t *));
int	compat_43_sys_ftruncate	__P((struct proc *, void *, register_t *));
int	hpux_sys_sysconf	__P((struct proc *, void *, register_t *));
int	hpux_sys_mkdir	__P((struct proc *, void *, register_t *));
int	hpux_sys_rmdir	__P((struct proc *, void *, register_t *));
int	hpux_sys_getrlimit	__P((struct proc *, void *, register_t *));
int	hpux_sys_setrlimit	__P((struct proc *, void *, register_t *));
int	hpux_sys_rtprio	__P((struct proc *, void *, register_t *));
int	hpux_sys_netioctl	__P((struct proc *, void *, register_t *));
int	hpux_sys_lockf	__P((struct proc *, void *, register_t *));
#ifdef SYSVSEM
int	sys_semget	__P((struct proc *, void *, register_t *));
int	sys___semctl	__P((struct proc *, void *, register_t *));
int	sys_semop	__P((struct proc *, void *, register_t *));
#else
#endif
#ifdef SYSVMSG
int	sys_msgget	__P((struct proc *, void *, register_t *));
int	sys_msgctl	__P((struct proc *, void *, register_t *));
int	sys_msgsnd	__P((struct proc *, void *, register_t *));
int	sys_msgrcv	__P((struct proc *, void *, register_t *));
#else
#endif
#ifdef SYSVSHM
int	sys_shmget	__P((struct proc *, void *, register_t *));
int	hpux_sys_shmctl	__P((struct proc *, void *, register_t *));
int	sys_shmat	__P((struct proc *, void *, register_t *));
int	sys_shmdt	__P((struct proc *, void *, register_t *));
#else
#endif
int	hpux_sys_advise	__P((struct proc *, void *, register_t *));
int	hpux_sys_getcontext	__P((struct proc *, void *, register_t *));
int	hpux_sys_getaccess	__P((struct proc *, void *, register_t *));
int	hpux_sys_waitpid	__P((struct proc *, void *, register_t *));
int	sys_pathconf	__P((struct proc *, void *, register_t *));
int	sys_fpathconf	__P((struct proc *, void *, register_t *));
int	compat_43_sys_getdirentries	__P((struct proc *, void *, register_t *));
int	compat_09_sys_getdomainname	__P((struct proc *, void *, register_t *));
int	compat_09_sys_setdomainname	__P((struct proc *, void *, register_t *));
int	hpux_sys_sigaction	__P((struct proc *, void *, register_t *));
int	hpux_sys_sigprocmask	__P((struct proc *, void *, register_t *));
int	hpux_sys_sigpending	__P((struct proc *, void *, register_t *));
int	hpux_sys_sigsuspend	__P((struct proc *, void *, register_t *));
int	compat_43_sys_getdtablesize	__P((struct proc *, void *, register_t *));
int	sys_fchdir	__P((struct proc *, void *, register_t *));
int	compat_43_sys_accept	__P((struct proc *, void *, register_t *));
int	sys_bind	__P((struct proc *, void *, register_t *));
int	sys_connect	__P((struct proc *, void *, register_t *));
int	compat_43_sys_getpeername	__P((struct proc *, void *, register_t *));
int	compat_43_sys_getsockname	__P((struct proc *, void *, register_t *));
int	sys_getsockopt	__P((struct proc *, void *, register_t *));
int	sys_listen	__P((struct proc *, void *, register_t *));
int	compat_43_sys_recv	__P((struct proc *, void *, register_t *));
int	compat_43_sys_recvfrom	__P((struct proc *, void *, register_t *));
int	compat_43_sys_recvmsg	__P((struct proc *, void *, register_t *));
int	compat_43_sys_send	__P((struct proc *, void *, register_t *));
int	compat_43_sys_sendmsg	__P((struct proc *, void *, register_t *));
int	sys_sendto	__P((struct proc *, void *, register_t *));
int	hpux_sys_setsockopt2	__P((struct proc *, void *, register_t *));
int	sys_shutdown	__P((struct proc *, void *, register_t *));
int	sys_socket	__P((struct proc *, void *, register_t *));
int	sys_socketpair	__P((struct proc *, void *, register_t *));
#ifdef SYSVSEM
int	sys___semctl	__P((struct proc *, void *, register_t *));
#else
#endif
#ifdef SYSVMSG
int	sys_msgctl	__P((struct proc *, void *, register_t *));
#else
#endif
#ifdef SYSVSHM
int	hpux_sys_nshmctl	__P((struct proc *, void *, register_t *));
#else
#endif
