/*
 * System call numbers.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.31 1996/02/27 04:20:41 jonathan Exp 
 */

#define	SYS_syscall	0
#define	SYS_exit	1
#define	SYS_fork	2
#define	SYS_read	3
#define	SYS_write	4
#define	SYS_open	5
#define	SYS_close	6
#define	SYS_wait4	7
				/* 8 is compat_43 ocreat */
#define	SYS_link	9
#define	SYS_unlink	10
				/* 11 is obsolete execv */
#define	SYS_chdir	12
#define	SYS_fchdir	13
#define	SYS_mknod	14
#define	SYS_chmod	15
#define	SYS_chown	16
#define	SYS_break	17
#define	SYS_getfsstat	18
				/* 19 is compat_43 olseek */
#define	SYS_getpid	20
#define	SYS_mount	21
#define	SYS_unmount	22
#define	SYS_setuid	23
#define	SYS_getuid	24
#define	SYS_geteuid	25
#define	SYS_ptrace	26
#define	SYS_recvmsg	27
#define	SYS_sendmsg	28
#define	SYS_recvfrom	29
#define	SYS_accept	30
#define	SYS_getpeername	31
#define	SYS_getsockname	32
#define	SYS_access	33
#define	SYS_chflags	34
#define	SYS_fchflags	35
#define	SYS_sync	36
#define	SYS_kill	37
				/* 38 is compat_43 ostat */
#define	SYS_getppid	39
				/* 40 is compat_43 olstat */
#define	SYS_dup	41
#define	SYS_pipe	42
#define	SYS_getegid	43
#define	SYS_profil	44
#define	SYS_ktrace	45
#define	SYS_sigaction	46
#define	SYS_getgid	47
#define	SYS_sigprocmask	48
#define	SYS_getlogin	49
#define	SYS_setlogin	50
#define	SYS_acct	51
#define	SYS_sigpending	52
#define	SYS_sigaltstack	53
#define	SYS_ioctl	54
#define	SYS_reboot	55
#define	SYS_revoke	56
#define	SYS_symlink	57
#define	SYS_readlink	58
#define	SYS_execve	59
#define	SYS_umask	60
#define	SYS_chroot	61
				/* 62 is compat_43 ofstat */
				/* 63 is compat_43 ogetkerninfo */
				/* 64 is compat_43 ogetpagesize */
#define	SYS_msync	65
#define	SYS_vfork	66
				/* 67 is obsolete vread */
				/* 68 is obsolete vwrite */
#define	SYS_sbrk	69
#define	SYS_sstk	70
				/* 71 is compat_43 ommap */
#define	SYS_vadvise	72
#define	SYS_munmap	73
#define	SYS_mprotect	74
#define	SYS_madvise	75
				/* 76 is obsolete vhangup */
				/* 77 is obsolete vlimit */
#define	SYS_mincore	78
#define	SYS_getgroups	79
#define	SYS_setgroups	80
#define	SYS_getpgrp	81
#define	SYS_setpgid	82
#define	SYS_setitimer	83
				/* 84 is compat_43 owait */
#define	SYS_swapon	85
#define	SYS_getitimer	86
				/* 87 is compat_43 ogethostname */
				/* 88 is compat_43 osethostname */
				/* 89 is compat_43 ogetdtablesize */
#define	SYS_dup2	90
#define	SYS_fcntl	92
#define	SYS_select	93
#define	SYS_fsync	95
#define	SYS_setpriority	96
#define	SYS_socket	97
#define	SYS_connect	98
				/* 99 is compat_43 oaccept */
#define	SYS_getpriority	100
				/* 101 is compat_43 osend */
				/* 102 is compat_43 orecv */
#define	SYS_sigreturn	103
#define	SYS_bind	104
#define	SYS_setsockopt	105
#define	SYS_listen	106
				/* 107 is obsolete vtimes */
				/* 108 is compat_43 osigvec */
				/* 109 is compat_43 osigblock */
				/* 110 is compat_43 osigsetmask */
#define	SYS_sigsuspend	111
				/* 112 is compat_43 osigstack */
				/* 113 is compat_43 orecvmsg */
				/* 114 is compat_43 osendmsg */
#define	SYS_vtrace	115
				/* 115 is obsolete vtrace */
#define	SYS_gettimeofday	116
#define	SYS_getrusage	117
#define	SYS_getsockopt	118
				/* 119 is obsolete resuba */
#define	SYS_readv	120
#define	SYS_writev	121
#define	SYS_settimeofday	122
#define	SYS_fchown	123
#define	SYS_fchmod	124
				/* 125 is compat_43 orecvfrom */
				/* 126 is compat_43 osetreuid */
				/* 127 is compat_43 osetregid */
#define	SYS_rename	128
				/* 129 is compat_43 otruncate */
				/* 130 is compat_43 oftruncate */
#define	SYS_flock	131
#define	SYS_mkfifo	132
#define	SYS_sendto	133
#define	SYS_shutdown	134
#define	SYS_socketpair	135
#define	SYS_mkdir	136
#define	SYS_rmdir	137
#define	SYS_utimes	138
				/* 139 is obsolete 4.2 sigreturn */
#define	SYS_adjtime	140
				/* 141 is compat_43 ogetpeername */
				/* 142 is compat_43 ogethostid */
				/* 143 is compat_43 osethostid */
				/* 144 is compat_43 ogetrlimit */
				/* 145 is compat_43 osetrlimit */
				/* 146 is compat_43 okillpg */
#define	SYS_setsid	147
#define	SYS_quotactl	148
				/* 149 is compat_43 oquota */
				/* 150 is compat_43 ogetsockname */
#define	SYS_nfssvc	155
				/* 156 is compat_43 ogetdirentries */
#define	SYS_statfs	157
#define	SYS_fstatfs	158
#define	SYS_getfh	161
				/* 162 is compat_09 ogetdomainname */
				/* 163 is compat_09 osetdomainname */
				/* 164 is compat_09 ouname */
#define	SYS_sysarch	165
				/* 169 is compat_10 osemsys */
				/* 170 is compat_10 omsgsys */
				/* 171 is compat_10 oshmsys */
#define	SYS_ntp_gettime	175
#define	SYS_ntp_adjtime	176
#define	SYS_setgid	181
#define	SYS_setegid	182
#define	SYS_seteuid	183
#define	SYS_lfs_bmapv	184
#define	SYS_lfs_markv	185
#define	SYS_lfs_segclean	186
#define	SYS_lfs_segwait	187
#define	SYS_stat	188
#define	SYS_fstat	189
#define	SYS_lstat	190
#define	SYS_pathconf	191
#define	SYS_fpathconf	192
#define	SYS_getrlimit	194
#define	SYS_setrlimit	195
#define	SYS_getdirentries	196
#define	SYS_mmap	197
#define	SYS___syscall	198
#define	SYS_lseek	199
#define	SYS_truncate	200
#define	SYS_ftruncate	201
#define	SYS___sysctl	202
#define	SYS_mlock	203
#define	SYS_munlock	204
#define	SYS_undelete	205
#define	SYS___semctl	220
#define	SYS_semget	221
#define	SYS_semop	222
#define	SYS_semconfig	223
#define	SYS_msgctl	224
#define	SYS_msgget	225
#define	SYS_msgsnd	226
#define	SYS_msgrcv	227
#define	SYS_shmat	228
#define	SYS_shmctl	229
#define	SYS_shmdt	230
#define	SYS_shmget	231
#define	SYS_minherit	250
#define	SYS_rfork	251
#define	SYS_MAXSYSCALL	252
