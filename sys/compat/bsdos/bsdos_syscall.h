/*	$OpenBSD: bsdos_syscall.h,v 1.22 2010/05/23 11:37:29 deraadt Exp $	*/

/*
 * System call numbers.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	OpenBSD: syscalls.master,v 1.19 2010/05/23 11:35:18 deraadt Exp 
 */

/* syscall: "syscall" ret: "int" args: */
#define	BSDOS_SYS_syscall	0

/* syscall: "exit" ret: "int" args: "int" */
#define	BSDOS_SYS_exit	1

/* syscall: "fork" ret: "int" args: */
#define	BSDOS_SYS_fork	2

/* syscall: "read" ret: "int" args: "int" "char *" "u_int" */
#define	BSDOS_SYS_read	3

/* syscall: "write" ret: "int" args: "int" "char *" "u_int" */
#define	BSDOS_SYS_write	4

/* syscall: "open" ret: "int" args: "char *" "int" "int" */
#define	BSDOS_SYS_open	5

/* syscall: "close" ret: "int" args: "int" */
#define	BSDOS_SYS_close	6

/* syscall: "wait4" ret: "int" args: "int" "int *" "int" "struct rusage *" */
#define	BSDOS_SYS_wait4	7

/* syscall: "ocreat" ret: "int" args: "char *" "int" */
#define	BSDOS_SYS_ocreat	8

/* syscall: "link" ret: "int" args: "char *" "char *" */
#define	BSDOS_SYS_link	9

/* syscall: "unlink" ret: "int" args: "char *" */
#define	BSDOS_SYS_unlink	10

				/* 11 is obsolete execv */
/* syscall: "chdir" ret: "int" args: "char *" */
#define	BSDOS_SYS_chdir	12

/* syscall: "fchdir" ret: "int" args: "int" */
#define	BSDOS_SYS_fchdir	13

/* syscall: "mknod" ret: "int" args: "char *" "int" "int" */
#define	BSDOS_SYS_mknod	14

/* syscall: "chmod" ret: "int" args: "char *" "int" */
#define	BSDOS_SYS_chmod	15

/* syscall: "chown" ret: "int" args: "char *" "int" "int" */
#define	BSDOS_SYS_chown	16

/* syscall: "break" ret: "int" args: "char *" */
#define	BSDOS_SYS_break	17

/* syscall: "getfsstat" ret: "int" args: "struct ostatfs *" "long" "int" */
#define	BSDOS_SYS_getfsstat	18

/* syscall: "olseek" ret: "long" args: "int" "long" "int" */
#define	BSDOS_SYS_olseek	19

/* syscall: "getpid" ret: "pid_t" args: */
#define	BSDOS_SYS_getpid	20

/* syscall: "mount" ret: "int" args: "int" "char *" "int" "caddr_t" */
#define	BSDOS_SYS_mount	21

/* syscall: "unmount" ret: "int" args: "char *" "int" */
#define	BSDOS_SYS_unmount	22

/* syscall: "setuid" ret: "int" args: "uid_t" */
#define	BSDOS_SYS_setuid	23

/* syscall: "getuid" ret: "uid_t" args: */
#define	BSDOS_SYS_getuid	24

/* syscall: "geteuid" ret: "uid_t" args: */
#define	BSDOS_SYS_geteuid	25

/* syscall: "ptrace" ret: "int" args: "int" "pid_t" "caddr_t" "int" */
#define	BSDOS_SYS_ptrace	26

/* syscall: "recvmsg" ret: "int" args: "int" "struct msghdr *" "int" */
#define	BSDOS_SYS_recvmsg	27

/* syscall: "sendmsg" ret: "int" args: "int" "caddr_t" "int" */
#define	BSDOS_SYS_sendmsg	28

/* syscall: "recvfrom" ret: "int" args: "int" "caddr_t" "size_t" "int" "caddr_t" "int *" */
#define	BSDOS_SYS_recvfrom	29

/* syscall: "accept" ret: "int" args: "int" "caddr_t" "int *" */
#define	BSDOS_SYS_accept	30

/* syscall: "getpeername" ret: "int" args: "int" "caddr_t" "int *" */
#define	BSDOS_SYS_getpeername	31

/* syscall: "getsockname" ret: "int" args: "int" "caddr_t" "int *" */
#define	BSDOS_SYS_getsockname	32

/* syscall: "access" ret: "int" args: "char *" "int" */
#define	BSDOS_SYS_access	33

/* syscall: "chflags" ret: "int" args: "char *" "int" */
#define	BSDOS_SYS_chflags	34

/* syscall: "fchflags" ret: "int" args: "int" "int" */
#define	BSDOS_SYS_fchflags	35

/* syscall: "sync" ret: "int" args: */
#define	BSDOS_SYS_sync	36

/* syscall: "kill" ret: "int" args: "int" "int" */
#define	BSDOS_SYS_kill	37

/* syscall: "stat43" ret: "int" args: "char *" "struct stat43 *" */
#define	BSDOS_SYS_stat43	38

/* syscall: "getppid" ret: "pid_t" args: */
#define	BSDOS_SYS_getppid	39

/* syscall: "lstat43" ret: "int" args: "char *" "struct stat43 *" */
#define	BSDOS_SYS_lstat43	40

/* syscall: "dup" ret: "int" args: "u_int" */
#define	BSDOS_SYS_dup	41

/* syscall: "opipe" ret: "int" args: */
#define	BSDOS_SYS_opipe	42

/* syscall: "getegid" ret: "gid_t" args: */
#define	BSDOS_SYS_getegid	43

/* syscall: "profil" ret: "int" args: "caddr_t" "u_int" "u_int" "u_int" */
#define	BSDOS_SYS_profil	44

/* syscall: "ktrace" ret: "int" args: "char *" "int" "int" "int" */
#define	BSDOS_SYS_ktrace	45

/* syscall: "sigaction" ret: "int" args: "int" "struct sigaction *" "struct sigaction *" */
#define	BSDOS_SYS_sigaction	46

/* syscall: "getgid" ret: "gid_t" args: */
#define	BSDOS_SYS_getgid	47

/* syscall: "sigprocmask" ret: "int" args: "int" "sigset_t" */
#define	BSDOS_SYS_sigprocmask	48

/* syscall: "getlogin" ret: "int" args: "char *" "u_int" */
#define	BSDOS_SYS_getlogin	49

/* syscall: "setlogin" ret: "int" args: "char *" */
#define	BSDOS_SYS_setlogin	50

/* syscall: "acct" ret: "int" args: "char *" */
#define	BSDOS_SYS_acct	51

/* syscall: "sigpending" ret: "int" args: */
#define	BSDOS_SYS_sigpending	52

/* syscall: "osigaltstack" ret: "int" args: "struct osigaltstack *" "struct osigaltstack *" */
#define	BSDOS_SYS_osigaltstack	53

/* syscall: "ioctl" ret: "int" args: "int" "u_long" "caddr_t" */
#define	BSDOS_SYS_ioctl	54

/* syscall: "reboot" ret: "int" args: "int" */
#define	BSDOS_SYS_reboot	55

/* syscall: "revoke" ret: "int" args: "char *" */
#define	BSDOS_SYS_revoke	56

/* syscall: "symlink" ret: "int" args: "char *" "char *" */
#define	BSDOS_SYS_symlink	57

/* syscall: "readlink" ret: "int" args: "char *" "char *" "int" */
#define	BSDOS_SYS_readlink	58

/* syscall: "execve" ret: "int" args: "char *" "char **" "char **" */
#define	BSDOS_SYS_execve	59

/* syscall: "umask" ret: "int" args: "int" */
#define	BSDOS_SYS_umask	60

/* syscall: "chroot" ret: "int" args: "char *" */
#define	BSDOS_SYS_chroot	61

/* syscall: "fstat43" ret: "int" args: "int" "struct stat43 *" */
#define	BSDOS_SYS_fstat43	62

/* syscall: "ogetkerninfo" ret: "int" args: "int" "char *" "int *" "int" */
#define	BSDOS_SYS_ogetkerninfo	63

/* syscall: "ogetpagesize" ret: "int" args: */
#define	BSDOS_SYS_ogetpagesize	64

/* syscall: "msync" ret: "int" args: "void *" "size_t" "int" */
#define	BSDOS_SYS_msync	65

/* syscall: "vfork" ret: "int" args: */
#define	BSDOS_SYS_vfork	66

				/* 67 is obsolete vread */
				/* 68 is obsolete vwrite */
/* syscall: "sbrk" ret: "int" args: "int" */
#define	BSDOS_SYS_sbrk	69

/* syscall: "sstk" ret: "int" args: "int" */
#define	BSDOS_SYS_sstk	70

/* syscall: "ommap" ret: "int" args: "caddr_t" "size_t" "int" "int" "int" "long" */
#define	BSDOS_SYS_ommap	71

				/* 72 is obsolete vadvise */
/* syscall: "munmap" ret: "int" args: "caddr_t" "size_t" */
#define	BSDOS_SYS_munmap	73

/* syscall: "mprotect" ret: "int" args: "caddr_t" "size_t" "int" */
#define	BSDOS_SYS_mprotect	74

/* syscall: "madvise" ret: "int" args: "caddr_t" "size_t" "int" */
#define	BSDOS_SYS_madvise	75

				/* 76 is obsolete vhangup */
				/* 77 is obsolete vlimit */
/* syscall: "mincore" ret: "int" args: "caddr_t" "size_t" "char *" */
#define	BSDOS_SYS_mincore	78

/* syscall: "getgroups" ret: "int" args: "u_int" "gid_t *" */
#define	BSDOS_SYS_getgroups	79

/* syscall: "setgroups" ret: "int" args: "u_int" "gid_t *" */
#define	BSDOS_SYS_setgroups	80

/* syscall: "getpgrp" ret: "int" args: */
#define	BSDOS_SYS_getpgrp	81

/* syscall: "setpgid" ret: "int" args: "int" "int" */
#define	BSDOS_SYS_setpgid	82

/* syscall: "setitimer" ret: "int" args: "u_int" "struct itimerval *" "struct itimerval *" */
#define	BSDOS_SYS_setitimer	83

/* syscall: "owait" ret: "int" args: */
#define	BSDOS_SYS_owait	84

/* syscall: "swapon" ret: "int" args: "char *" */
#define	BSDOS_SYS_swapon	85

/* syscall: "getitimer" ret: "int" args: "u_int" "struct itimerval *" */
#define	BSDOS_SYS_getitimer	86

/* syscall: "ogethostname" ret: "int" args: "char *" "u_int" */
#define	BSDOS_SYS_ogethostname	87

/* syscall: "osethostname" ret: "int" args: "char *" "u_int" */
#define	BSDOS_SYS_osethostname	88

/* syscall: "ogetdtablesize" ret: "int" args: */
#define	BSDOS_SYS_ogetdtablesize	89

/* syscall: "dup2" ret: "int" args: "u_int" "u_int" */
#define	BSDOS_SYS_dup2	90

/* syscall: "fcntl" ret: "int" args: "int" "int" "void *" */
#define	BSDOS_SYS_fcntl	92

/* syscall: "select" ret: "int" args: "u_int" "fd_set *" "fd_set *" "fd_set *" "struct timeval *" */
#define	BSDOS_SYS_select	93

/* syscall: "fsync" ret: "int" args: "int" */
#define	BSDOS_SYS_fsync	95

/* syscall: "setpriority" ret: "int" args: "int" "int" "int" */
#define	BSDOS_SYS_setpriority	96

/* syscall: "socket" ret: "int" args: "int" "int" "int" */
#define	BSDOS_SYS_socket	97

/* syscall: "connect" ret: "int" args: "int" "caddr_t" "int" */
#define	BSDOS_SYS_connect	98

/* syscall: "oaccept" ret: "int" args: "int" "caddr_t" "int *" */
#define	BSDOS_SYS_oaccept	99

/* syscall: "getpriority" ret: "int" args: "int" "int" */
#define	BSDOS_SYS_getpriority	100

/* syscall: "osend" ret: "int" args: "int" "caddr_t" "int" "int" */
#define	BSDOS_SYS_osend	101

/* syscall: "orecv" ret: "int" args: "int" "caddr_t" "int" "int" */
#define	BSDOS_SYS_orecv	102

/* syscall: "sigreturn" ret: "int" args: "struct sigcontext *" */
#define	BSDOS_SYS_sigreturn	103

/* syscall: "bind" ret: "int" args: "int" "caddr_t" "int" */
#define	BSDOS_SYS_bind	104

/* syscall: "setsockopt" ret: "int" args: "int" "int" "int" "caddr_t" "int" */
#define	BSDOS_SYS_setsockopt	105

/* syscall: "listen" ret: "int" args: "int" "int" */
#define	BSDOS_SYS_listen	106

				/* 107 is obsolete vtimes */
/* syscall: "osigvec" ret: "int" args: "int" "struct sigvec *" "struct sigvec *" */
#define	BSDOS_SYS_osigvec	108

/* syscall: "osigblock" ret: "int" args: "int" */
#define	BSDOS_SYS_osigblock	109

/* syscall: "osigsetmask" ret: "int" args: "int" */
#define	BSDOS_SYS_osigsetmask	110

/* syscall: "sigsuspend" ret: "int" args: "int" */
#define	BSDOS_SYS_sigsuspend	111

/* syscall: "osigstack" ret: "int" args: "struct sigstack *" "struct sigstack *" */
#define	BSDOS_SYS_osigstack	112

/* syscall: "orecvmsg" ret: "int" args: "int" "struct omsghdr *" "int" */
#define	BSDOS_SYS_orecvmsg	113

/* syscall: "osendmsg" ret: "int" args: "int" "caddr_t" "int" */
#define	BSDOS_SYS_osendmsg	114

/* syscall: "vtrace" ret: "int" args: "int" "int" */
#define	BSDOS_SYS_vtrace	115

				/* 115 is obsolete vtrace */
/* syscall: "gettimeofday" ret: "int" args: "struct timeval *" "struct timezone *" */
#define	BSDOS_SYS_gettimeofday	116

/* syscall: "getrusage" ret: "int" args: "int" "struct rusage *" */
#define	BSDOS_SYS_getrusage	117

/* syscall: "getsockopt" ret: "int" args: "int" "int" "int" "caddr_t" "int *" */
#define	BSDOS_SYS_getsockopt	118

				/* 119 is obsolete resuba */
/* syscall: "readv" ret: "int" args: "int" "struct iovec *" "u_int" */
#define	BSDOS_SYS_readv	120

/* syscall: "writev" ret: "int" args: "int" "struct iovec *" "u_int" */
#define	BSDOS_SYS_writev	121

/* syscall: "settimeofday" ret: "int" args: "struct timeval *" "struct timezone *" */
#define	BSDOS_SYS_settimeofday	122

/* syscall: "fchown" ret: "int" args: "int" "int" "int" */
#define	BSDOS_SYS_fchown	123

/* syscall: "fchmod" ret: "int" args: "int" "int" */
#define	BSDOS_SYS_fchmod	124

/* syscall: "orecvfrom" ret: "int" args: "int" "caddr_t" "size_t" "int" "caddr_t" "int *" */
#define	BSDOS_SYS_orecvfrom	125

/* syscall: "setreuid" ret: "int" args: "uid_t" "uid_t" */
#define	BSDOS_SYS_setreuid	126

/* syscall: "setregid" ret: "int" args: "gid_t" "gid_t" */
#define	BSDOS_SYS_setregid	127

/* syscall: "rename" ret: "int" args: "char *" "char *" */
#define	BSDOS_SYS_rename	128

/* syscall: "otruncate" ret: "int" args: "char *" "long" */
#define	BSDOS_SYS_otruncate	129

/* syscall: "oftruncate" ret: "int" args: "int" "long" */
#define	BSDOS_SYS_oftruncate	130

/* syscall: "flock" ret: "int" args: "int" "int" */
#define	BSDOS_SYS_flock	131

/* syscall: "mkfifo" ret: "int" args: "char *" "int" */
#define	BSDOS_SYS_mkfifo	132

/* syscall: "sendto" ret: "int" args: "int" "caddr_t" "size_t" "int" "caddr_t" "int" */
#define	BSDOS_SYS_sendto	133

/* syscall: "shutdown" ret: "int" args: "int" "int" */
#define	BSDOS_SYS_shutdown	134

/* syscall: "socketpair" ret: "int" args: "int" "int" "int" "int *" */
#define	BSDOS_SYS_socketpair	135

/* syscall: "mkdir" ret: "int" args: "char *" "int" */
#define	BSDOS_SYS_mkdir	136

/* syscall: "rmdir" ret: "int" args: "char *" */
#define	BSDOS_SYS_rmdir	137

/* syscall: "utimes" ret: "int" args: "char *" "struct timeval *" */
#define	BSDOS_SYS_utimes	138

				/* 139 is obsolete 4.2 sigreturn */
/* syscall: "adjtime" ret: "int" args: "struct timeval *" "struct timeval *" */
#define	BSDOS_SYS_adjtime	140

/* syscall: "ogetpeername" ret: "int" args: "int" "caddr_t" "int *" */
#define	BSDOS_SYS_ogetpeername	141

/* syscall: "ogethostid" ret: "int32_t" args: */
#define	BSDOS_SYS_ogethostid	142

/* syscall: "osethostid" ret: "int" args: "int32_t" */
#define	BSDOS_SYS_osethostid	143

/* syscall: "ogetrlimit" ret: "int" args: "u_int" "struct ogetrlimit *" */
#define	BSDOS_SYS_ogetrlimit	144

/* syscall: "osetrlimit" ret: "int" args: "u_int" "struct ogetrlimit *" */
#define	BSDOS_SYS_osetrlimit	145

/* syscall: "okillpg" ret: "int" args: "int" "int" */
#define	BSDOS_SYS_okillpg	146

/* syscall: "setsid" ret: "int" args: */
#define	BSDOS_SYS_setsid	147

/* syscall: "quotactl" ret: "int" args: "char *" "int" "int" "caddr_t" */
#define	BSDOS_SYS_quotactl	148

/* syscall: "oquota" ret: "int" args: */
#define	BSDOS_SYS_oquota	149

/* syscall: "ogetsockname" ret: "int" args: "int" "caddr_t" "int *" */
#define	BSDOS_SYS_ogetsockname	150

/* syscall: "nfssvc" ret: "int" args: "int" "caddr_t" */
#define	BSDOS_SYS_nfssvc	155

/* syscall: "ogetdirentries" ret: "int" args: "int" "char *" "u_int" "long *" */
#define	BSDOS_SYS_ogetdirentries	156

/* syscall: "statfs" ret: "int" args: "char *" "struct ostatfs *" */
#define	BSDOS_SYS_statfs	157

/* syscall: "fstatfs" ret: "int" args: "int" "struct ostatfs *" */
#define	BSDOS_SYS_fstatfs	158

/* syscall: "getfh" ret: "int" args: "char *" "fhandle_t *" */
#define	BSDOS_SYS_getfh	161

/* syscall: "shmsys" ret: "int" args: "int" "int" "int" "int" */
#define	BSDOS_SYS_shmsys	171

/* syscall: "setgid" ret: "int" args: "gid_t" */
#define	BSDOS_SYS_setgid	181

/* syscall: "setegid" ret: "int" args: "gid_t" */
#define	BSDOS_SYS_setegid	182

/* syscall: "seteuid" ret: "int" args: "uid_t" */
#define	BSDOS_SYS_seteuid	183

/* syscall: "stat35" ret: "int" args: "char *" "struct stat35 *" */
#define	BSDOS_SYS_stat35	188

/* syscall: "fstat35" ret: "int" args: "int" "struct stat35 *" */
#define	BSDOS_SYS_fstat35	189

/* syscall: "lstat35" ret: "int" args: "char *" "struct stat35 *" */
#define	BSDOS_SYS_lstat35	190

/* syscall: "pathconf" ret: "int" args: "char *" "int" */
#define	BSDOS_SYS_pathconf	191

/* syscall: "fpathconf" ret: "int" args: "int" "int" */
#define	BSDOS_SYS_fpathconf	192

/* syscall: "getrlimit" ret: "int" args: "u_int" "struct rlimit *" */
#define	BSDOS_SYS_getrlimit	194

/* syscall: "setrlimit" ret: "int" args: "u_int" "struct rlimit *" */
#define	BSDOS_SYS_setrlimit	195

/* syscall: "getdirentries" ret: "int" args: "int" "char *" "u_int" "long *" */
#define	BSDOS_SYS_getdirentries	196

/* syscall: "mmap" ret: "caddr_t" args: "caddr_t" "size_t" "int" "int" "int" "long" "off_t" */
#define	BSDOS_SYS_mmap	197

/* syscall: "__syscall" ret: "int" args: */
#define	BSDOS_SYS___syscall	198

/* syscall: "lseek" ret: "off_t" args: "int" "int" "off_t" "int" */
#define	BSDOS_SYS_lseek	199

/* syscall: "truncate" ret: "int" args: "char *" "int" "off_t" */
#define	BSDOS_SYS_truncate	200

/* syscall: "ftruncate" ret: "int" args: "int" "int" "off_t" */
#define	BSDOS_SYS_ftruncate	201

/* syscall: "__sysctl" ret: "int" args: "int *" "u_int" "void *" "size_t *" "void *" "size_t" */
#define	BSDOS_SYS___sysctl	202

/* syscall: "mlock" ret: "int" args: "caddr_t" "size_t" */
#define	BSDOS_SYS_mlock	203

/* syscall: "munlock" ret: "int" args: "caddr_t" "size_t" */
#define	BSDOS_SYS_munlock	204

/* syscall: "__semctl" ret: "int" args: "int" "int" "int" "union semun *" */
#define	BSDOS_SYS___semctl	220

/* syscall: "semget" ret: "int" args: "key_t" "int" "int" */
#define	BSDOS_SYS_semget	221

/* syscall: "semop" ret: "int" args: "int" "struct sembuf *" "u_int" */
#define	BSDOS_SYS_semop	222

				/* 223 is obsolete sys_semconfig */
/* syscall: "msgctl" ret: "int" args: "int" "int" "struct msqid_ds *" */
#define	BSDOS_SYS_msgctl	224

/* syscall: "msgget" ret: "int" args: "key_t" "int" */
#define	BSDOS_SYS_msgget	225

/* syscall: "msgsnd" ret: "int" args: "int" "void *" "size_t" "int" */
#define	BSDOS_SYS_msgsnd	226

/* syscall: "msgrcv" ret: "int" args: "int" "void *" "size_t" "long" "int" */
#define	BSDOS_SYS_msgrcv	227

/* syscall: "shmat" ret: "int" args: "int" "void *" "int" */
#define	BSDOS_SYS_shmat	228

/* syscall: "shmctl" ret: "int" args: "int" "int" "struct shmid_ds *" */
#define	BSDOS_SYS_shmctl	229

/* syscall: "shmdt" ret: "int" args: "void *" */
#define	BSDOS_SYS_shmdt	230

/* syscall: "shmget" ret: "int" args: "key_t" "int" "int" */
#define	BSDOS_SYS_shmget	231

#define	BSDOS_SYS_MAXSYSCALL	232
