/*
 * System call numbers.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	OpenBSD: syscalls.master,v 1.4 1996/08/02 20:20:28 niklas Exp 
 */

#define	IBCS2_SYS_syscall	0
#define	IBCS2_SYS_exit	1
#define	IBCS2_SYS_fork	2
#define	IBCS2_SYS_read	3
#define	IBCS2_SYS_write	4
#define	IBCS2_SYS_open	5
#define	IBCS2_SYS_close	6
#define	IBCS2_SYS_waitsys	7
#define	IBCS2_SYS_creat	8
#define	IBCS2_SYS_link	9
#define	IBCS2_SYS_unlink	10
#define	IBCS2_SYS_execv	11
#define	IBCS2_SYS_chdir	12
#define	IBCS2_SYS_time	13
#define	IBCS2_SYS_mknod	14
#define	IBCS2_SYS_chmod	15
#define	IBCS2_SYS_chown	16
#define	IBCS2_SYS_obreak	17
#define	IBCS2_SYS_stat	18
#define	IBCS2_SYS_lseek	19
#define	IBCS2_SYS_getpid	20
#define	IBCS2_SYS_mount	21
#define	IBCS2_SYS_umount	22
#define	IBCS2_SYS_setuid	23
#define	IBCS2_SYS_getuid	24
#define	IBCS2_SYS_stime	25
#define	IBCS2_SYS_alarm	27
#define	IBCS2_SYS_fstat	28
#define	IBCS2_SYS_pause	29
#define	IBCS2_SYS_utime	30
#define	IBCS2_SYS_access	33
#define	IBCS2_SYS_nice	34
#define	IBCS2_SYS_statfs	35
#define	IBCS2_SYS_sync	36
#define	IBCS2_SYS_kill	37
#define	IBCS2_SYS_fstatfs	38
#define	IBCS2_SYS_pgrpsys	39
#define	IBCS2_SYS_dup	41
#define	IBCS2_SYS_pipe	42
#define	IBCS2_SYS_times	43
#define	IBCS2_SYS_plock	45
#define	IBCS2_SYS_setgid	46
#define	IBCS2_SYS_getgid	47
#define	IBCS2_SYS_sigsys	48
#define	IBCS2_SYS_msgsys	49
#define	IBCS2_SYS_sysi86	50
#define	IBCS2_SYS_shmsys	52
#define	IBCS2_SYS_semsys	53
#define	IBCS2_SYS_ioctl	54
#define	IBCS2_SYS_uadmin	55
#define	IBCS2_SYS_utssys	57
#define	IBCS2_SYS_execve	59
#define	IBCS2_SYS_umask	60
#define	IBCS2_SYS_chroot	61
#define	IBCS2_SYS_fcntl	62
#define	IBCS2_SYS_ulimit	63
				/* 70 is obsolete rfs_advfs */
				/* 71 is obsolete rfs_unadvfs */
				/* 72 is obsolete rfs_rmount */
				/* 73 is obsolete rfs_rumount */
				/* 74 is obsolete rfs_rfstart */
				/* 75 is obsolete rfs_sigret */
				/* 76 is obsolete rfs_rdebug */
				/* 77 is obsolete rfs_rfstop */
#define	IBCS2_SYS_rmdir	79
#define	IBCS2_SYS_mkdir	80
#define	IBCS2_SYS_getdents	81
#define	IBCS2_SYS_sysfs	84
#define	IBCS2_SYS_getmsg	85
#define	IBCS2_SYS_putmsg	86
#define	IBCS2_SYS_poll	87
#define	IBCS2_SYS_symlink	90
#define	IBCS2_SYS_lstat	91
#define	IBCS2_SYS_readlink	92
#define	IBCS2_SYS_sigreturn	103
#define	IBCS2_SYS_rdchk	135
#define	IBCS2_SYS_chsize	138
#define	IBCS2_SYS_ftime	139
#define	IBCS2_SYS_nap	140
#define	IBCS2_SYS_select	164
#define	IBCS2_SYS_eaccess	165
#define	IBCS2_SYS_sigaction	167
#define	IBCS2_SYS_sigprocmask	168
#define	IBCS2_SYS_sigpending	169
#define	IBCS2_SYS_sigsuspend	170
#define	IBCS2_SYS_getgroups	171
#define	IBCS2_SYS_setgroups	172
#define	IBCS2_SYS_sysconf	173
#define	IBCS2_SYS_pathconf	174
#define	IBCS2_SYS_fpathconf	175
#define	IBCS2_SYS_rename	176
#define	IBCS2_SYS_MAXSYSCALL	177
