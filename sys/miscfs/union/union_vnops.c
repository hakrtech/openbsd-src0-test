/*	$OpenBSD: union_vnops.c,v 1.14 2001/07/26 02:10:41 assar Exp $	*/
/*	$NetBSD: union_vnops.c,v 1.30.4.1 1996/05/25 22:10:14 jtc Exp $	*/

/*
 * Copyright (c) 1992, 1993, 1994 The Regents of the University of California.
 * Copyright (c) 1992, 1993, 1994 Jan-Simon Pendry.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Jan-Simon Pendry.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)union_vnops.c	8.22 (Berkeley) 12/10/94
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <sys/file.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/vnode.h>
#include <sys/mount.h>
#include <sys/namei.h>
#include <sys/malloc.h>
#include <sys/buf.h>
#include <sys/queue.h>
#include <miscfs/union/union.h>

/*
 * Global vfs data structures
 */

int union_lookup	__P((void *));
int union_create	__P((void *));
int union_whiteout	__P((void *));
int union_mknod		__P((void *));
int union_open		__P((void *));
int union_close		__P((void *));
int union_access	__P((void *));
int union_getattr	__P((void *));
int union_setattr	__P((void *));
int union_read		__P((void *));
int union_write		__P((void *));
int union_lease		__P((void *));
int union_ioctl		__P((void *));
int union_select	__P((void *));
int union_fsync		__P((void *));
int union_remove	__P((void *));
int union_link		__P((void *));
int union_rename	__P((void *));
int union_mkdir		__P((void *));
int union_rmdir		__P((void *));
int union_revoke       	__P((void *));
int union_symlink	__P((void *));
int union_readdir	__P((void *));
int union_readlink	__P((void *));
int union_abortop	__P((void *));
int union_inactive	__P((void *));
int union_reclaim	__P((void *));
int union_lock		__P((void *));
int union_unlock	__P((void *));
int union_bmap		__P((void *));
int union_print		__P((void *));
int union_islocked	__P((void *));
int union_pathconf	__P((void *));
int union_advlock	__P((void *));
int union_strategy	__P((void *));

int (**union_vnodeop_p) __P((void *));
struct vnodeopv_entry_desc union_vnodeop_entries[] = {
	{ &vop_default_desc, vn_default_error },
	{ &vop_lookup_desc, union_lookup },		/* lookup */
	{ &vop_create_desc, union_create },		/* create */
	{ &vop_whiteout_desc, union_whiteout },		/* whiteout */
	{ &vop_mknod_desc, union_mknod },		/* mknod */
	{ &vop_open_desc, union_open },			/* open */
	{ &vop_close_desc, union_close },		/* close */
	{ &vop_access_desc, union_access },		/* access */
	{ &vop_getattr_desc, union_getattr },		/* getattr */
	{ &vop_setattr_desc, union_setattr },		/* setattr */
	{ &vop_read_desc, union_read },			/* read */
	{ &vop_write_desc, union_write },		/* write */
	{ &vop_lease_desc, union_lease },		/* lease */
	{ &vop_ioctl_desc, union_ioctl },		/* ioctl */
	{ &vop_select_desc, union_select },		/* select */
	{ &vop_fsync_desc, union_fsync },		/* fsync */
	{ &vop_remove_desc, union_remove },		/* remove */
	{ &vop_link_desc, union_link },			/* link */
	{ &vop_rename_desc, union_rename },		/* rename */
	{ &vop_mkdir_desc, union_mkdir },		/* mkdir */
	{ &vop_rmdir_desc, union_rmdir },		/* rmdir */
	{ &vop_symlink_desc, union_symlink },		/* symlink */
	{ &vop_readdir_desc, union_readdir },		/* readdir */
	{ &vop_readlink_desc, union_readlink },		/* readlink */
	{ &vop_revoke_desc, union_revoke },             /* revoke */
	{ &vop_abortop_desc, union_abortop },		/* abortop */
	{ &vop_inactive_desc, union_inactive },		/* inactive */
	{ &vop_reclaim_desc, union_reclaim },		/* reclaim */
	{ &vop_lock_desc, union_lock },			/* lock */
	{ &vop_unlock_desc, union_unlock },		/* unlock */
	{ &vop_bmap_desc, union_bmap },			/* bmap */
	{ &vop_strategy_desc, union_strategy },		/* strategy */
	{ &vop_print_desc, union_print },		/* print */
	{ &vop_islocked_desc, union_islocked },		/* islocked */
	{ &vop_pathconf_desc, union_pathconf },		/* pathconf */
	{ &vop_advlock_desc, union_advlock },		/* advlock */
	{ (struct vnodeop_desc*)NULL, (int(*) __P((void *)))NULL }
};
struct vnodeopv_desc union_vnodeop_opv_desc =
	{ &union_vnodeop_p, union_vnodeop_entries };

#define FIXUP(un, p) { \
	if (((un)->un_flags & UN_ULOCK) == 0) { \
		union_fixup(un, p); \
	} \
}
static void union_fixup __P((struct union_node *, struct proc *));
static int union_lookup1 __P((struct vnode *, struct vnode **,
			      struct vnode **, struct componentname *));


static void
union_fixup(un, p)
	struct union_node *un;
	struct proc *p;
{
	vn_lock(un->un_uppervp, LK_EXCLUSIVE | LK_RETRY, p);
	un->un_flags |= UN_ULOCK;
}

static int
union_lookup1(udvp, dvpp, vpp, cnp)
	struct vnode *udvp;
	struct vnode **dvpp;
	struct vnode **vpp;
	struct componentname *cnp;
{
	struct proc *p = cnp->cn_proc;
	int error;
	struct vnode *tdvp;
	struct vnode *dvp;
	struct mount *mp;

	dvp = *dvpp;

	/*
	 * If stepping up the directory tree, check for going
	 * back across the mount point, in which case do what
	 * lookup would do by stepping back down the mount
	 * hierarchy.
	 */
	if (cnp->cn_flags & ISDOTDOT) {
		while ((dvp != udvp) && (dvp->v_flag & VROOT)) {
			/*
			 * Don't do the NOCROSSMOUNT check
			 * at this level.  By definition,
			 * union fs deals with namespaces, not
			 * filesystems.
			 */
			tdvp = dvp;
			*dvpp = dvp = dvp->v_mount->mnt_vnodecovered;
			vput(tdvp);
			VREF(dvp);
			vn_lock(dvp, LK_EXCLUSIVE | LK_RETRY, p);
		}
	}

        error = VOP_LOOKUP(dvp, &tdvp, cnp);
	if (error)
		return (error);

	/*
	 * The parent directory will have been unlocked, unless lookup
	 * found the last component.  In which case, re-lock the node
	 * here to allow it to be unlocked again (phew) in union_lookup.
	 */
	if (dvp != tdvp && !(cnp->cn_flags & ISLASTCN))
		vn_lock(dvp, LK_EXCLUSIVE | LK_RETRY, p);

	dvp = tdvp;

	/*
	 * Lastly check if the current node is a mount point in
	 * which case walk up the mount hierarchy making sure not to
	 * bump into the root of the mount tree (ie. dvp != udvp).
	 */
	while (dvp != udvp && (dvp->v_type == VDIR) &&
	       (mp = dvp->v_mountedhere)) {

		if (vfs_busy(mp, 0, 0, p))
			continue;
	
		error = VFS_ROOT(mp, &tdvp);
		vfs_unbusy(mp, p);

		vput(dvp);

		if (error) {
			return (error);
		}

		dvp = tdvp;
	}

	*vpp = dvp;
	return (0);
}

int
union_lookup(v)
	void *v;
{
	struct vop_lookup_args /* {
		struct vnodeop_desc *a_desc;
		struct vnode *a_dvp;
		struct vnode **a_vpp;
		struct componentname *a_cnp;
	} */ *ap = v;
	int error;
	int uerror, lerror;
	struct vnode *uppervp, *lowervp;
	struct vnode *upperdvp, *lowerdvp;
	struct vnode *dvp = ap->a_dvp;
	struct union_node *dun = VTOUNION(dvp);
	struct proc *p = curproc;
	struct componentname *cnp = ap->a_cnp;
	int lockparent = cnp->cn_flags & LOCKPARENT;
	struct union_mount *um = MOUNTTOUNIONMOUNT(dvp->v_mount);
	struct ucred *saved_cred = NULL;
	int iswhiteout;
	struct vattr va;

#ifdef notyet
	if (cnp->cn_namelen == 3 &&
			cnp->cn_nameptr[2] == '.' &&
			cnp->cn_nameptr[1] == '.' &&
			cnp->cn_nameptr[0] == '.') {
		dvp = *ap->a_vpp = LOWERVP(ap->a_dvp);
		if (dvp == NULLVP)
			return (ENOENT);
		VREF(dvp);
		vn_lock(dvp, LK_EXCLUSIVE | LK_RETRY, p);
		if (!lockparent || !(cnp->cn_flags & ISLASTCN))
			VOP_UNLOCK(ap->a_dvp, 0, p, 0, p);
		return (0);
	}
#endif

	cnp->cn_flags |= LOCKPARENT;

	upperdvp = dun->un_uppervp;
	lowerdvp = dun->un_lowervp;
	uppervp = NULLVP;
	lowervp = NULLVP;
	iswhiteout = 0;

	/*
	 * do the lookup in the upper level.
	 * if that level comsumes additional pathnames,
	 * then assume that something special is going
	 * on and just return that vnode.
	 */
	if (upperdvp != NULLVP) {
		FIXUP(dun, p);
		/*
		 * If we're doing `..' in the underlying filesystem,
		 * we must drop our lock on the union node before
		 * going up the tree in the lower file system--if we block
		 * on the lowervp lock, and that's held by someone else
		 * coming down the tree and who's waiting for our lock,
		 * we would be hosed.
		 */
		if (cnp->cn_flags & ISDOTDOT) {
			/* retain lock on underlying VP: */
			dun->un_flags |= UN_KLOCK;
			VOP_UNLOCK(dvp, 0, p);
		}
		uerror = union_lookup1(um->um_uppervp, &upperdvp,
					&uppervp, cnp);
		if (cnp->cn_flags & ISDOTDOT) {
			if (dun->un_uppervp == upperdvp) {
				/*
				 * we got the underlying bugger back locked...
				 * now take back the union node lock.  Since we
				 * hold the uppervp lock, we can diddle union
				 * locking flags at will. :)
				 */
				dun->un_flags |= UN_ULOCK;
			}
			/*
			 * if upperdvp got swapped out, it means we did
			 * some mount point magic, and we do not have
			 * dun->un_uppervp locked currently--so we get it
			 * locked here (don't set the UN_ULOCK flag).
			 */
			vn_lock(dvp, LK_EXCLUSIVE | LK_RETRY, p);
		}

		/*if (uppervp == upperdvp)
			dun->un_flags |= UN_KLOCK;*/

		if (cnp->cn_consume != 0) {
			*ap->a_vpp = uppervp;
			if (!lockparent)
				cnp->cn_flags &= ~LOCKPARENT;
			return (uerror);
		}
		if (uerror == ENOENT || uerror == EJUSTRETURN) {
			if (cnp->cn_flags & ISWHITEOUT) {
				iswhiteout = 1;
			} else if (lowerdvp != NULLVP) {
				lerror = VOP_GETATTR(upperdvp, &va,
						     cnp->cn_cred,
						     cnp->cn_proc);
				if (lerror == 0 && (va.va_flags & OPAQUE))
					iswhiteout = 1;
			}
		}
	} else {
		uerror = ENOENT;
	}

	/*
	 * in a similar way to the upper layer, do the lookup
	 * in the lower layer.   this time, if there is some
	 * component magic going on, then vput whatever we got
	 * back from the upper layer and return the lower vnode
	 * instead.
	 */
	if (lowerdvp != NULLVP && !iswhiteout) {
		int nameiop;

		vn_lock(lowerdvp, LK_EXCLUSIVE | LK_RETRY, p);

		/*
		 * Only do a LOOKUP on the bottom node, since
		 * we won't be making changes to it anyway.
		 */
		nameiop = cnp->cn_nameiop;
		cnp->cn_nameiop = LOOKUP;
		if (um->um_op == UNMNT_BELOW) {
			saved_cred = cnp->cn_cred;
			cnp->cn_cred = um->um_cred;
		}
		/*
		 * we shouldn't have to worry about locking interactions
		 * between the lower layer and our union layer (w.r.t.
		 * `..' processing) because we don't futz with lowervp
		 * locks in the union-node instantiation code path.
		 */
		lerror = union_lookup1(um->um_lowervp, &lowerdvp,
				&lowervp, cnp);
		if (um->um_op == UNMNT_BELOW)
			cnp->cn_cred = saved_cred;
		cnp->cn_nameiop = nameiop;

		if (lowervp != lowerdvp)
			VOP_UNLOCK(lowerdvp, 0, p);

		if (cnp->cn_consume != 0) {
			if (uppervp != NULLVP) {
				if (uppervp == upperdvp)
					vrele(uppervp);
				else
					vput(uppervp);
				uppervp = NULLVP;
			}
			*ap->a_vpp = lowervp;
			if (!lockparent)
				cnp->cn_flags &= ~LOCKPARENT;
			return (lerror);
		}
	} else {
		lerror = ENOENT;
		if ((cnp->cn_flags & ISDOTDOT) && dun->un_pvp != NULLVP) {
			lowervp = LOWERVP(dun->un_pvp);
			if (lowervp != NULLVP) {
				VREF(lowervp);
				vn_lock(lowervp, LK_EXCLUSIVE | LK_RETRY, p);
				lerror = 0;
			}
		}
	}

	if (!lockparent)
		cnp->cn_flags &= ~LOCKPARENT;

	/*
	 * at this point, we have uerror and lerror indicating
	 * possible errors with the lookups in the upper and lower
	 * layers.  additionally, uppervp and lowervp are (locked)
	 * references to existing vnodes in the upper and lower layers.
	 *
	 * there are now three cases to consider.
	 * 1. if both layers returned an error, then return whatever
	 *    error the upper layer generated.
	 *
	 * 2. if the top layer failed and the bottom layer succeeded
	 *    then two subcases occur.
	 *    a.  the bottom vnode is not a directory, in which
	 *	  case just return a new union vnode referencing
	 *	  an empty top layer and the existing bottom layer.
	 *    b.  the bottom vnode is a directory, in which case
	 *	  create a new directory in the top-level and
	 *	  continue as in case 3.
	 *
	 * 3. if the top layer succeeded then return a new union
	 *    vnode referencing whatever the new top layer and
	 *    whatever the bottom layer returned.
	 */

	*ap->a_vpp = NULLVP;

	/* case 1. */
	if ((uerror != 0) && (lerror != 0)) {
		return (uerror);
	}

	/* case 2. */
	if (uerror != 0 /* && (lerror == 0) */ ) {
		if (lowervp->v_type == VDIR) { /* case 2b. */
			/*
			 * We may be racing another process to make the
			 * upper-level shadow directory.  Be careful with
			 * locks/etc!
			 */
			dun->un_flags &= ~UN_ULOCK;
			VOP_UNLOCK(upperdvp, 0, p);
			uerror = union_mkshadow(um, upperdvp, cnp, &uppervp);
			vn_lock(upperdvp, LK_EXCLUSIVE | LK_RETRY, p);
			dun->un_flags |= UN_ULOCK;

			if (uerror) {
				if (lowervp != NULLVP) {
					vput(lowervp);
					lowervp = NULLVP;
				}
				return (uerror);
			}
		}
	}

	if (lowervp != NULLVP)
		VOP_UNLOCK(lowervp, 0, p);

	error = union_allocvp(ap->a_vpp, dvp->v_mount, dvp, upperdvp, cnp,
			      uppervp, lowervp, 1);

	if (error) {
		if (uppervp != NULLVP)
			vput(uppervp);
		if (lowervp != NULLVP)
			vrele(lowervp);
	} else {
		if (*ap->a_vpp != dvp)
			if (!lockparent || !(cnp->cn_flags & ISLASTCN))
				VOP_UNLOCK(dvp, 0, p);
		if (cnp->cn_namelen == 1 &&
			cnp->cn_nameptr[0] == '.' &&
			*ap->a_vpp != dvp) {
			panic("union_lookup returning. (%x) not same as startdir (%x)",
			    ap->a_vpp, dvp);
		}

	}

	return (error);
}

int
union_create(v)
	void *v;
{
	struct vop_create_args /* {
		struct vnode *a_dvp;
		struct vnode **a_vpp;
		struct componentname *a_cnp;
		struct vattr *a_vap;
	} */ *ap = v;
	struct union_node *un = VTOUNION(ap->a_dvp);
	struct vnode *dvp;
	struct componentname *cnp = ap->a_cnp;
	struct proc *p = cnp->cn_proc;

	if ((dvp = un->un_uppervp) != NULLVP) {
		int error;
		struct vnode *vp;
		struct mount *mp;

		FIXUP(un, p);

		VREF(dvp);
		un->un_flags |= UN_KLOCK;
		mp = ap->a_dvp->v_mount;
		vput(ap->a_dvp);
		error = VOP_CREATE(dvp, &vp, cnp, ap->a_vap);
		if (error)
			return (error);

		error = union_allocvp(
				ap->a_vpp,
				mp,
				NULLVP,
				NULLVP,
				cnp,
				vp,
				NULLVP,
				1);
		if (error)
			vput(vp);
		return (error);
	}

	vput(ap->a_dvp);
	return (EROFS);
}

int
union_whiteout(v)
	void *v;
{
	struct vop_whiteout_args /* {
		struct vnode *a_dvp;
		struct componentname *a_cnp;
		int a_flags;
	} */ *ap = v;
	struct union_node *un = VTOUNION(ap->a_dvp);
	struct proc *p = curproc;

	if (un->un_uppervp == NULLVP)
		return (EOPNOTSUPP);

	FIXUP(un, p);
	return (VOP_WHITEOUT(un->un_uppervp, ap->a_cnp, ap->a_flags));
}

int
union_mknod(v)
	void *v;
{
	struct vop_mknod_args /* {
		struct vnode *a_dvp;
		struct vnode **a_vpp;
		struct componentname *a_cnp;
		struct vattr *a_vap;
	} */ *ap = v;
	struct union_node *un = VTOUNION(ap->a_dvp);
	struct vnode *dvp;
	struct proc *p = ap->a_cnp->cn_proc;

	if ((dvp = un->un_uppervp) != NULLVP) {
		int error;
		struct vnode *vp;
		struct mount *mp;

		FIXUP(un, p);

		VREF(dvp);
		un->un_flags |= UN_KLOCK;
		mp = ap->a_dvp->v_mount;
		vput(ap->a_dvp);
		error = VOP_MKNOD(dvp, &vp, ap->a_cnp, ap->a_vap);
		if (error)
			return (error);

		error = union_allocvp(
			ap->a_vpp,
			mp,
			NULLVP,
			NULLVP,
			ap->a_cnp,
			vp,
			NULLVP,
			1);
		if (error)
			vput(vp);
		return (error);
	}

	vput(ap->a_dvp);
	return (EROFS);
}

int
union_open(v)
	void *v;
{
	struct vop_open_args /* {
		struct vnodeop_desc *a_desc;
		struct vnode *a_vp;
		int a_mode;
		struct ucred *a_cred;
		struct proc *a_p;
	} */ *ap = v;
	struct union_node *un = VTOUNION(ap->a_vp);
	struct vnode *tvp;
	int mode = ap->a_mode;
	struct ucred *cred = ap->a_cred;
	struct proc *p = ap->a_p;
	int error;

	/*
	 * If there is an existing upper vp then simply open that.
	 */
	tvp = un->un_uppervp;
	if (tvp == NULLVP) {
		/*
		 * If the lower vnode is being opened for writing, then
		 * copy the file contents to the upper vnode and open that,
		 * otherwise can simply open the lower vnode.
		 */
		tvp = un->un_lowervp;
		if ((ap->a_mode & FWRITE) && (tvp->v_type == VREG)) {
			error = union_copyup(un, (mode&O_TRUNC) == 0, cred, p);
			if (error == 0)
				error = VOP_OPEN(un->un_uppervp, mode, cred, p);
			return (error);
		}

		/*
		 * Just open the lower vnode
		 */
		un->un_openl++;
		vn_lock(tvp, LK_EXCLUSIVE | LK_RETRY, p);
		error = VOP_OPEN(tvp, mode, cred, p);
		VOP_UNLOCK(tvp, 0, p);

		return (error);
	}

	FIXUP(un, p);

	error = VOP_OPEN(tvp, mode, cred, p);

	return (error);
}

int
union_close(v)
	void *v;
{
	struct vop_close_args /* {
		struct vnode *a_vp;
		int  a_fflag;
		struct ucred *a_cred;
		struct proc *a_p;
	} */ *ap = v;
	struct union_node *un = VTOUNION(ap->a_vp);
	struct vnode *vp;

	if ((vp = un->un_uppervp) == NULLVP) {
#ifdef UNION_DIAGNOSTIC
		if (un->un_openl <= 0)
			panic("union: un_openl cnt");
#endif
		--un->un_openl;
		vp = un->un_lowervp;
	}
#ifdef UNION_DIAGNOSTIC
	/*
	 * A stranded union node may end up here with both vnodes NULL,
	 * in which case we don't do anything.
	 */
	if (vp == NULLVP) {
	    vprint("empty union vnode", vp);
	    panic("union_close empty vnode");
	}
#endif

	ap->a_vp = vp;
	return (VCALL(vp, VOFFSET(vop_close), ap));
}

/*
 * Check access permission on the union vnode.
 * The access check being enforced is to check
 * against both the underlying vnode, and any
 * copied vnode.  This ensures that no additional
 * file permissions are given away simply because
 * the user caused an implicit file copy.
 */
int
union_access(v)
	void *v;
{
	struct vop_access_args /* {
		struct vnodeop_desc *a_desc;
		struct vnode *a_vp;
		int a_mode;
		struct ucred *a_cred;
		struct proc *a_p;
	} */ *ap = v;
	struct union_node *un = VTOUNION(ap->a_vp);
	int error = EACCES;
	struct vnode *vp;
	struct proc *p = ap->a_p;

	if ((vp = un->un_uppervp) != NULLVP) {
		FIXUP(un, p);
		return (VOP_ACCESS(vp, ap->a_mode, ap->a_cred, ap->a_p));
	}

	if ((vp = un->un_lowervp) != NULLVP) {
		vn_lock(vp, LK_EXCLUSIVE | LK_RETRY, p);
		error = VOP_ACCESS(vp, ap->a_mode, ap->a_cred, ap->a_p);
		if (error == 0) {
			struct union_mount *um = MOUNTTOUNIONMOUNT(ap->a_vp->v_mount);

			if (um->um_op == UNMNT_BELOW)
				error = VOP_ACCESS(vp, ap->a_mode,
						um->um_cred, ap->a_p);
		}
		VOP_UNLOCK(vp, 0, p);
		if (error)
			return (error);
	}

	return (error);
}

/*
 * We handle getattr only to change the fsid and
 * track object sizes
 */
int
union_getattr(v)
	void *v;
{
	struct vop_getattr_args /* {
		struct vnode *a_vp;
		struct vattr *a_vap;
		struct ucred *a_cred;
		struct proc *a_p;
	} */ *ap = v;
	int error;
	struct union_node *un = VTOUNION(ap->a_vp);
	struct vnode *vp = un->un_uppervp;
	struct vattr *vap;
	struct vattr va;
	struct proc *p = ap->a_p;

	/*
	 * Some programs walk the filesystem hierarchy by counting
	 * links to directories to avoid stat'ing all the time.
	 * This means the link count on directories needs to be "correct".
	 * The only way to do that is to call getattr on both layers
	 * and fix up the link count.  The link count will not necessarily
	 * be accurate but will be large enough to defeat the tree walkers.
	 */

	vap = ap->a_vap;

	if ((vp = un->un_uppervp) != NULLVP) {
		/*
		 * It's not clear whether VOP_GETATTR is to be
		 * called with the vnode locked or not.  stat() calls
		 * it with (vp) locked, and fstat calls it with
		 * (vp) unlocked.
		 * In the mean time, compensate here by checking
		 * the union_node's lock flag.
		 */
		if (un->un_flags & UN_LOCKED)
			FIXUP(un, p);

		error = VOP_GETATTR(vp, vap, ap->a_cred, ap->a_p);
		if (error)
			return (error);
		union_newsize(ap->a_vp, vap->va_size, VNOVAL);
	}

	if (vp == NULLVP) {
		vp = un->un_lowervp;
	} else if (vp->v_type == VDIR) {
		vp = un->un_lowervp;
		vap = &va;
	} else {
		vp = NULLVP;
	}

	if (vp != NULLVP) {
		error = VOP_GETATTR(vp, vap, ap->a_cred, ap->a_p);
		if (error)
			return (error);
		union_newsize(ap->a_vp, VNOVAL, vap->va_size);
	}

	if ((vap != ap->a_vap) && (vap->va_type == VDIR))
		ap->a_vap->va_nlink += vap->va_nlink;

	ap->a_vap->va_fsid = ap->a_vp->v_mount->mnt_stat.f_fsid.val[0];
	return (0);
}

int
union_setattr(v)
	void *v;
{
	struct vop_setattr_args /* {
		struct vnode *a_vp;
		struct vattr *a_vap;
		struct ucred *a_cred;
		struct proc *a_p;
	} */ *ap = v;
	struct union_node *un = VTOUNION(ap->a_vp);
	struct proc *p = ap->a_p;
	int error;

	/*
	 * Handle case of truncating lower object to zero size,
	 * by creating a zero length upper object.  This is to
	 * handle the case of open with O_TRUNC and O_CREAT.
	 */
	if ((un->un_uppervp == NULLVP) &&
	    /* assert(un->un_lowervp != NULLVP) */
	    (un->un_lowervp->v_type == VREG)) {
		error = union_copyup(un, (ap->a_vap->va_size != 0),
						ap->a_cred, ap->a_p);
		if (error)
			return (error);
	}

	/*
	 * Try to set attributes in upper layer,
	 * otherwise return read-only filesystem error.
	 */
	if (un->un_uppervp != NULLVP) {
		FIXUP(un, p);
		error = VOP_SETATTR(un->un_uppervp, ap->a_vap,
					ap->a_cred, ap->a_p);
		if ((error == 0) && (ap->a_vap->va_size != VNOVAL))
			union_newsize(ap->a_vp, ap->a_vap->va_size, VNOVAL);
	} else {
		error = EROFS;
	}

	return (error);
}

int
union_read(v)
	void *v;
{
	struct vop_read_args /* {
		struct vnode *a_vp;
		struct uio *a_uio;
		int  a_ioflag;
		struct ucred *a_cred;
	} */ *ap = v;
	int error;
	struct vnode *vp = OTHERVP(ap->a_vp);
	int dolock = (vp == LOWERVP(ap->a_vp));
	struct proc *p = curproc;

	if (dolock)
		vn_lock(vp, LK_EXCLUSIVE | LK_RETRY, p);
	else
		FIXUP(VTOUNION(ap->a_vp), p);
	error = VOP_READ(vp, ap->a_uio, ap->a_ioflag, ap->a_cred);
	if (dolock)
		VOP_UNLOCK(vp, 0, p);

	/*
	 * XXX
	 * perhaps the size of the underlying object has changed under
	 * our feet.  take advantage of the offset information present
	 * in the uio structure.
	 */
	if (error == 0) {
		struct union_node *un = VTOUNION(ap->a_vp);
		off_t cur = ap->a_uio->uio_offset;

		if (vp == un->un_uppervp) {
			if (cur > un->un_uppersz)
				union_newsize(ap->a_vp, cur, VNOVAL);
		} else {
			if (cur > un->un_lowersz)
				union_newsize(ap->a_vp, VNOVAL, cur);
		}
	}

	return (error);
}

int
union_write(v)
	void *v;
{
	struct vop_read_args /* {
		struct vnode *a_vp;
		struct uio *a_uio;
		int  a_ioflag;
		struct ucred *a_cred;
	} */ *ap = v;
	int error;
	struct vnode *vp;
	struct union_node *un = VTOUNION(ap->a_vp);
	struct proc *p = curproc;

	vp = UPPERVP(ap->a_vp);
	if (vp == NULLVP)
		panic("union: missing upper layer in write");

	FIXUP(un, p);
	error = VOP_WRITE(vp, ap->a_uio, ap->a_ioflag, ap->a_cred);

	/*
	 * the size of the underlying object may be changed by the
	 * write.
	 */
	if (error == 0) {
		off_t cur = ap->a_uio->uio_offset;

		if (cur > un->un_uppersz)
			union_newsize(ap->a_vp, cur, VNOVAL);
	}

	return (error);
}

int
union_lease(v)
	void *v;
{
	struct vop_lease_args /* {
		struct vnode *a_vp;
		struct proc *a_p;
		struct ucred *a_cred;
		int a_flag;
	} */ *ap = v;
	register struct vnode *vp = OTHERVP(ap->a_vp);

	ap->a_vp = vp;
	return (VCALL(vp, VOFFSET(vop_lease), ap));
}

int
union_ioctl(v)
	void *v;
{
	struct vop_ioctl_args /* {
		struct vnode *a_vp;
		u_long a_command;
		caddr_t  a_data;
		int  a_fflag;
		struct ucred *a_cred;
		struct proc *a_p;
	} */ *ap = v;
	register struct vnode *vp = OTHERVP(ap->a_vp);

	ap->a_vp = vp;
	return (VCALL(vp, VOFFSET(vop_ioctl), ap));
}

int
union_select(v)
	void *v;
{
	struct vop_select_args /* {
		struct vnode *a_vp;
		int  a_which;
		int  a_fflags;
		struct ucred *a_cred;
		struct proc *a_p;
	} */ *ap = v;
	register struct vnode *vp = OTHERVP(ap->a_vp);

	ap->a_vp = vp;
	return (VCALL(vp, VOFFSET(vop_select), ap));
}

int
union_fsync(v)
	void *v;
{
	struct vop_fsync_args /* {
		struct vnode *a_vp;
		struct ucred *a_cred;
		int  a_waitfor;
		struct proc *a_p;
	} */ *ap = v;
	int error = 0;
	struct vnode *targetvp = OTHERVP(ap->a_vp);
	struct proc *p = ap->a_p;

	if (targetvp != NULLVP) {
		int dolock = (targetvp == LOWERVP(ap->a_vp));

		if (dolock)
			vn_lock(targetvp, LK_EXCLUSIVE | LK_RETRY, p);
		else
			FIXUP(VTOUNION(ap->a_vp), p);
		error = VOP_FSYNC(targetvp, ap->a_cred,
					ap->a_waitfor, ap->a_p);
		if (dolock)
			VOP_UNLOCK(targetvp, 0, p);
	}

	return (error);
}

/* a_dvp: directory in which to link
   a_vp: new target of the link
   a_cnp: name for the link
   */
int
union_remove(v)
	void *v;
{
	struct vop_remove_args /* {
		struct vnode *a_dvp;
		struct vnode *a_vp;
		struct componentname *a_cnp;
	} */ *ap = v;
	int error;
	struct union_node *dun = VTOUNION(ap->a_dvp);
	struct union_node *un = VTOUNION(ap->a_vp);
	struct proc *p = ap->a_cnp->cn_proc;

	if (dun->un_uppervp == NULLVP)
		panic("union remove: null upper vnode");

	if (un->un_uppervp != NULLVP) {
		struct vnode *dvp = dun->un_uppervp;
		struct vnode *vp = un->un_uppervp;
		struct componentname *cnp = ap->a_cnp;

		FIXUP(dun, p);
		VREF(dvp);
		dun->un_flags |= UN_KLOCK;
		vput(ap->a_dvp);
		FIXUP(un, p);
		VREF(vp);
		un->un_flags |= UN_KLOCK;
		vput(ap->a_vp);

		if (union_dowhiteout(un, cnp->cn_cred, cnp->cn_proc))
			cnp->cn_flags |= DOWHITEOUT;
		error = VOP_REMOVE(dvp, vp, cnp);
		if (!error)
			union_removed_upper(un);
	} else {
		FIXUP(dun, p);
		error = union_mkwhiteout(
			MOUNTTOUNIONMOUNT(UNIONTOV(dun)->v_mount),
			dun->un_uppervp, ap->a_cnp, un->un_path);
		vput(ap->a_dvp);
		vput(ap->a_vp);
	}

	return (error);
}

/* a_dvp: directory in which to link
   a_vp: new target of the link
   a_cnp: name for the link
   */
int
union_link(v)
	void *v;
{
	struct vop_link_args /* {
		struct vnode *a_dvp;
		struct vnode *a_vp;
		struct componentname *a_cnp;
	} */ *ap = v;
	int error = 0;
	struct union_node *dun;
	struct vnode *dvp;
	struct vnode *vp;
	struct proc *p = ap->a_cnp->cn_proc;

	dun = VTOUNION(ap->a_dvp);

#ifdef DIAGNOSTIC
	if (!(ap->a_cnp->cn_flags & LOCKPARENT)) {
		printf("union_link called without LOCKPARENT set!\n");
		error = EIO; /* need some error code for "caller is a bozo" */
	} else
#endif
	if (ap->a_dvp->v_op != ap->a_vp->v_op) {
		vp = ap->a_vp;
	} else {
		struct union_node *un = VTOUNION(ap->a_vp);

		if (un->un_uppervp == NULLVP) {
			/*
			 * needs to be copied up before we can link it.
			 */
			vn_lock(ap->a_vp, LK_EXCLUSIVE | LK_RETRY, p);
			if (dun->un_uppervp == un->un_dirvp) {
				VOP_UNLOCK(ap->a_dvp, 0, p);
			}
			error = union_copyup(un, 1, ap->a_cnp->cn_cred,
						ap->a_cnp->cn_proc);
			if (dun->un_uppervp == un->un_dirvp) {
				/* During copyup, we dropped the lock on the
				 * dir and invalidated any saved namei lookup
				 * state for the directory we'll be entering
				 * the link in.  We need to re-run the lookup
				 * in that directory to reset any state needed
				 * for VOP_LINK.
				 * Call relookup on the union-layer to
				 * reset the state.
				 */
				vp  = NULLVP;
				if (dun->un_uppervp == NULLVP ||
				    /*
				     * relookup starts with an unlocked node,
				     * and since LOCKPARENT is set returns
				     * the starting directory locked.
				     */
				    (error = relookup(ap->a_dvp,
						      &dvp, ap->a_cnp))) {
					vrele(ap->a_dvp);
					VOP_UNLOCK(ap->a_vp, 0, p);
					return EROFS;
				}
				if (dvp != NULLVP) {
					/* The name we want to create has
					   mysteriously appeared (a race?) */
					error = EEXIST;
					VOP_UNLOCK(ap->a_vp, 0, p);
					goto croak;
				}
			}
			VOP_UNLOCK(ap->a_vp, 0, p);
		}
		vp = un->un_uppervp;
	}

	dvp = dun->un_uppervp;
	if (dvp == NULLVP)
		error = EROFS;

	if (error) {
croak:
		vput(ap->a_dvp);
		return (error);
	}

	FIXUP(dun, p);
	VREF(dvp);
	dun->un_flags |= UN_KLOCK;
	vput(ap->a_dvp);

	return (VOP_LINK(dvp, vp, ap->a_cnp));
}

int
union_rename(v)
	void *v;
{
	struct vop_rename_args  /* {
		struct vnode *a_fdvp;
		struct vnode *a_fvp;
		struct componentname *a_fcnp;
		struct vnode *a_tdvp;
		struct vnode *a_tvp;
		struct componentname *a_tcnp;
	} */ *ap = v;
	int error;

	struct vnode *fdvp = ap->a_fdvp;
	struct vnode *fvp = ap->a_fvp;
	struct vnode *tdvp = ap->a_tdvp;
	struct vnode *tvp = ap->a_tvp;
	struct union_node *unfile = (struct union_node *)0;

	if (fdvp->v_op == union_vnodeop_p) {	/* always true */
		struct union_node *un = VTOUNION(fdvp);
		if (un->un_uppervp == NULLVP) {
			/*
			 * this should never happen in normal
			 * operation but might if there was
			 * a problem creating the top-level shadow
			 * directory.
			 */
			error = EXDEV;
			goto bad;
		}

		fdvp = un->un_uppervp;
		VREF(fdvp);
		vrele(ap->a_fdvp);
	}

	if (fvp->v_op == union_vnodeop_p) {	/* always true */
		unfile = VTOUNION(fvp);
		if (unfile->un_uppervp == NULLVP) {
			/* XXX: should do a copyup */
			error = EXDEV;
			goto bad;
		}

		if (unfile->un_lowervp != NULLVP)
			ap->a_fcnp->cn_flags |= DOWHITEOUT;

		fvp = unfile->un_uppervp;
		VREF(fvp);
		/* vrele(ap->a_fvp); */ /* hold for later */
	}

	if (tdvp->v_op == union_vnodeop_p) {
		struct union_node *un = VTOUNION(tdvp);
		if (un->un_uppervp == NULLVP) {
			/*
			 * this should never happen in normal
			 * operation but might if there was
			 * a problem creating the top-level shadow
			 * directory.
			 */
			error = EXDEV;
			goto badrele;
		}

		tdvp = un->un_uppervp;
		VREF(tdvp);
		un->un_flags |= UN_KLOCK;
		vput(ap->a_tdvp);
	}

	if (tvp != NULLVP && tvp->v_op == union_vnodeop_p) {
		struct union_node *un = VTOUNION(tvp);

		tvp = un->un_uppervp;
		if (tvp != NULLVP) {
			VREF(tvp);
			un->un_flags |= UN_KLOCK;
		}
#if 0
		/* XXX should we toss from the cache? */
		if (un->un_flags & UN_CACHED) {
			un->un_flags &= ~UN_CACHED;
			LIST_REMOVE(un, un_cache);
		}
#endif
		vput(ap->a_tvp);
	}

	error = VOP_RENAME(fdvp, fvp, ap->a_fcnp, tdvp, tvp, ap->a_tcnp);
	if (!error && unfile)
	    union_removed_upper(unfile);
	vrele(ap->a_fvp);
	return error;

badrele:
	vrele(ap->a_fvp);
bad:
	vrele(fdvp);
	vrele(fvp);
	vput(tdvp);
	if (tvp != NULLVP)
		vput(tvp);

	return (error);
}

int
union_mkdir(v)
	void *v;
{
	struct vop_mkdir_args /* {
		struct vnode *a_dvp;
		struct vnode **a_vpp;
		struct componentname *a_cnp;
		struct vattr *a_vap;
	} */ *ap = v;
	struct union_node *un = VTOUNION(ap->a_dvp);
	struct vnode *dvp = un->un_uppervp;
	struct proc *p = ap->a_cnp->cn_proc;

	if (dvp != NULLVP) {
		int error;
		struct vnode *vp;

		FIXUP(un, p);
		VREF(dvp);
		un->un_flags |= UN_KLOCK;
		VOP_UNLOCK(ap->a_dvp, 0, p);
		error = VOP_MKDIR(dvp, &vp, ap->a_cnp, ap->a_vap);
		if (error) {
			vrele(ap->a_dvp);
			return (error);
		}

		error = union_allocvp(
				ap->a_vpp,
				ap->a_dvp->v_mount,
				ap->a_dvp,
				NULLVP,
				ap->a_cnp,
				vp,
				NULLVP,
				1);
		vrele(ap->a_dvp);
		if (error)
			vput(vp);
		return (error);
	}

	vput(ap->a_dvp);
	return (EROFS);
}

int
union_rmdir(v)
	void *v;
{
	struct vop_rmdir_args /* {
		struct vnode *a_dvp;
		struct vnode *a_vp;
		struct componentname *a_cnp;
	} */ *ap = v;
	int error;
	struct union_node *dun = VTOUNION(ap->a_dvp);
	struct union_node *un = VTOUNION(ap->a_vp);
	struct proc *p = ap->a_cnp->cn_proc;

	if (dun->un_uppervp == NULLVP)
		panic("union rmdir: null upper vnode");

	if (un->un_uppervp != NULLVP) {
		struct vnode *dvp = dun->un_uppervp;
		struct vnode *vp = un->un_uppervp;
		struct componentname *cnp = ap->a_cnp;

		FIXUP(dun, p);
		VREF(dvp);
		dun->un_flags |= UN_KLOCK;
		vput(ap->a_dvp);
		FIXUP(un, p);
		VREF(vp);
		un->un_flags |= UN_KLOCK;
		vput(ap->a_vp);

		if (union_dowhiteout(un, cnp->cn_cred, cnp->cn_proc))
			cnp->cn_flags |= DOWHITEOUT;
		error = VOP_RMDIR(dvp, vp, cnp);
		if (!error)
			union_removed_upper(un);
	} else {
		FIXUP(dun, p);
		error = union_mkwhiteout(
			MOUNTTOUNIONMOUNT(UNIONTOV(dun)->v_mount),
			dun->un_uppervp, ap->a_cnp, un->un_path);
		vput(ap->a_dvp);
		vput(ap->a_vp);
	}

	return (error);
}

int
union_symlink(v)
	void *v;
{
	struct vop_symlink_args /* {
		struct vnode *a_dvp;
		struct vnode **a_vpp;
		struct componentname *a_cnp;
		struct vattr *a_vap;
		char *a_target;
	} */ *ap = v;
	struct union_node *un = VTOUNION(ap->a_dvp);
	struct vnode *dvp = un->un_uppervp;
	struct proc *p = ap->a_cnp->cn_proc;

	if (dvp != NULLVP) {
		int error;

		FIXUP(un, p);
		VREF(dvp);
		un->un_flags |= UN_KLOCK;
		vput(ap->a_dvp);
		error = VOP_SYMLINK(dvp, ap->a_vpp, ap->a_cnp,
					ap->a_vap, ap->a_target);
		return (error);
	}

	vput(ap->a_dvp);
	return (EROFS);
}

/*
 * union_readdir works in concert with getdirentries and
 * readdir(3) to provide a list of entries in the unioned
 * directories.  getdirentries is responsible for walking
 * down the union stack.  readdir(3) is responsible for
 * eliminating duplicate names from the returned data stream.
 */
int
union_readdir(v)
	void *v;
{
	struct vop_readdir_args /* {
		struct vnodeop_desc *a_desc;
		struct vnode *a_vp;
		struct uio *a_uio;
		struct ucred *a_cred;
		int *a_eofflag;
		int *a_ncookies;
		u_long **a_cookies;

	} */ *ap = v;
	register struct union_node *un = VTOUNION(ap->a_vp);
	register struct vnode *vp;
	struct proc *p = curproc;
	if ((vp = un->un_uppervp) == NULLVP)
		return (0);

	FIXUP(un, p);
	ap->a_vp = vp;
	return (VCALL(vp, VOFFSET(vop_readdir), ap));
}

int
union_readlink(v)
	void *v;
{
	struct vop_readlink_args /* {
		struct vnode *a_vp;
		struct uio *a_uio;
		struct ucred *a_cred;
	} */ *ap = v;
	int error;
	struct vnode *vp = OTHERVP(ap->a_vp);
	struct proc *p = curproc;

	int dolock = (vp == LOWERVP(ap->a_vp));

	if (dolock)
		vn_lock(vp, LK_EXCLUSIVE | LK_RETRY, p);
	else
		FIXUP(VTOUNION(ap->a_vp), p);
	ap->a_vp = vp;
	error = VCALL(vp, VOFFSET(vop_readlink), ap);
	if (dolock)
		VOP_UNLOCK(vp, 0, p);

	return (error);
}

/*
 * When operations want to vput() a union node yet retain a lock on
 * the upper VP (say, to do some further operations like link(),
 * mkdir(), ...), they set UN_KLOCK on the union node, then call
 * vput() which calls VOP_UNLOCK(, 0, p) and comes here.  union_unlock()
 * unlocks the union node (leaving the upper VP alone), clears the
 * KLOCK flag, and then returns to vput().  The caller then does whatever
 * is left to do with the upper VP, and insures that it gets unlocked.
 *
 * If UN_KLOCK isn't set, then the upper VP is unlocked here.
 */

int
union_abortop(v)
	void *v;
{
	struct vop_abortop_args /* {
		struct vnode *a_dvp;
		struct componentname *a_cnp;
	} */ *ap = v;
	int error;
	struct vnode *vp = OTHERVP(ap->a_dvp);
	struct union_node *un = VTOUNION(ap->a_dvp);
	struct proc *p = ap->a_cnp->cn_proc;

	int islocked = un->un_flags & UN_LOCKED;
	int dolock = (vp == LOWERVP(ap->a_dvp));

	if (islocked) {
		if (dolock)
			vn_lock(vp, LK_EXCLUSIVE | LK_RETRY, p);
		else
			FIXUP(VTOUNION(ap->a_dvp), p);
	}
	ap->a_dvp = vp;
	error = VCALL(vp, VOFFSET(vop_abortop), ap);
	if (islocked && dolock)
		VOP_UNLOCK(vp, 0, p);

	return (error);
}

int
union_inactive(v)
	void *v;
{
	struct vop_inactive_args /* {
		struct vnode *a_vp;
		struct proc *a_p;
	} */ *ap = v;
	struct union_node *un = VTOUNION(ap->a_vp);

	/*
	 * Do nothing (and _don't_ bypass).
	 * Wait to vrele lowervp until reclaim,
	 * so that until then our union_node is in the
	 * cache and reusable.
	 *
	 * NEEDSWORK: Someday, consider inactive'ing
	 * the lowervp and then trying to reactivate it
	 * with capabilities (v_id)
	 * like they do in the name lookup cache code.
	 * That's too much work for now.
	 */

#ifdef UNION_DIAGNOSTIC
	if (un->un_flags & UN_LOCKED)
		panic("union: inactivating locked node");
	if (un->un_flags & UN_ULOCK)
		panic("union: inactivating w/locked upper node");
#endif

	union_diruncache(un);

	VOP_UNLOCK(ap->a_vp, 0, ap->a_p);

	if ((un->un_flags & UN_CACHED) == 0)
		vrecycle(ap->a_vp, (struct simplelock *)0, ap->a_p);

	return (0);
}

int
union_reclaim(v)
	void *v;
{
	struct vop_reclaim_args /* {
		struct vnode *a_vp;
	} */ *ap = v;

	union_freevp(ap->a_vp);

	return (0);
}

int
union_lock(v)
	void *v;
{
	struct vop_lock_args *ap = v;
	struct vnode *vp = ap->a_vp;
	struct union_node *un;
	struct proc *p = ap->a_p;
	int    flags = ap->a_flags;
	int    error = 0;

	vop_generic_lock(ap);
	/*
	 * Need to do real lockmgr-style locking here.
	 * in the mean time, draining won't work quite right,
	 * which could lead to a few race conditions.
	 * the following test was here, but is not quite right, we
	 * still need to take the lock:
	if ((flags & LK_TYPE_MASK) == LK_DRAIN)
		return (0);
	 */
	flags &= ~LK_INTERLOCK;

start:
 	un = VTOUNION(vp);

	if (un->un_uppervp != NULLVP) {
		if (((un->un_flags & UN_ULOCK) == 0) &&
		    (vp->v_usecount != 0)) {
			error = vn_lock(un->un_uppervp, flags, p);
			if (error)
				return (error);
			un->un_flags |= UN_ULOCK;
		}
#ifdef DIAGNOSTIC
		if (un->un_flags & UN_KLOCK) {
			vprint("union: dangling klock", vp);
			panic("union: dangling upper lock (%lx)", vp);
		}
#endif
 	}

	if (un->un_flags & UN_LOCKED) {
#ifdef DIAGNOSTIC
		if (curproc && un->un_pid == curproc->p_pid &&
			    un->un_pid > -1 && curproc->p_pid > -1)
			panic("union: locking against myself");
#endif
		un->un_flags |= UN_WANTED;
		sleep((caddr_t)un, PINOD);
		goto start;
	}

#ifdef DIAGNOSTIC
	if (curproc)
		un->un_pid = curproc->p_pid;
	else
		un->un_pid = -1;
#endif

	un->un_flags |= UN_LOCKED;
	return (0);
}

/*
 * When operations want to vput() a union node yet retain a lock on
 * the upper VP (say, to do some further operations like link(),
 * mkdir(), ...), they set UN_KLOCK on the union node, then call
 * vput() which calls VOP_UNLOCK(, 0, p) and comes here.  union_unlock()
 * unlocks the union node (leaving the upper VP alone), clears the
 * KLOCK flag, and then returns to vput().  The caller then does whatever
 * is left to do with the upper VP, and insures that it gets unlocked.
 *
 * If UN_KLOCK isn't set, then the upper VP is unlocked here.
 */

int
union_unlock(v)
	void *v;
{
	struct vop_lock_args *ap = v;
	struct union_node *un = VTOUNION(ap->a_vp);
	struct proc *p = ap->a_p;

#ifdef DIAGNOSTIC
	if ((un->un_flags & UN_LOCKED) == 0)
		panic("union: unlock unlocked node");
	if (curproc && un->un_pid != curproc->p_pid &&
			curproc->p_pid > -1 && un->un_pid > -1)
		panic("union: unlocking other process's union node");
#endif

	un->un_flags &= ~UN_LOCKED;

	if ((un->un_flags & (UN_ULOCK|UN_KLOCK)) == UN_ULOCK)
		VOP_UNLOCK(un->un_uppervp, 0, p);

	un->un_flags &= ~(UN_ULOCK|UN_KLOCK);

	if (un->un_flags & UN_WANTED) {
		un->un_flags &= ~UN_WANTED;
		wakeup((caddr_t)un);
	}

#ifdef DIAGNOSTIC
	un->un_pid = 0;
#endif

	vop_generic_unlock(v);

	return (0);
}

int
union_bmap(v)
	void *v;
{
	struct vop_bmap_args /* {
		struct vnode *a_vp;
		daddr_t  a_bn;
		struct vnode **a_vpp;
		daddr_t *a_bnp;
		int *a_runp;
	} */ *ap = v;
	int error;
	struct proc *p = curproc;
	struct vnode *vp = OTHERVP(ap->a_vp);
	int dolock = (vp == LOWERVP(ap->a_vp));

	if (dolock)
		vn_lock(vp, LK_EXCLUSIVE | LK_RETRY, p);
	else
		FIXUP(VTOUNION(ap->a_vp), p);
	ap->a_vp = vp;
	error = VCALL(vp, VOFFSET(vop_bmap), ap);
	if (dolock)
		VOP_UNLOCK(vp, 0, p);

	return (error);
}

int
union_print(v)
	void *v;
{
	struct vop_print_args /* {
		struct vnode *a_vp;
	} */ *ap = v;
	struct vnode *vp = ap->a_vp;

	printf("\ttag VT_UNION, vp=%p, uppervp=%p, lowervp=%p\n",
	    vp, UPPERVP(vp), LOWERVP(vp));
	if (UPPERVP(vp))
		vprint("uppervp", UPPERVP(vp));
	if (LOWERVP(vp))
		vprint("lowervp", LOWERVP(vp));
	if (VTOUNION(vp)->un_dircache) {
		struct vnode **vpp;

		for (vpp = VTOUNION(vp)->un_dircache; *vpp != NULLVP; vpp++)
			vprint("dircache:", *vpp);
	}
	return (0);
}

int
union_islocked(v)
	void *v;
{
	struct vop_islocked_args /* {
		struct vnode *a_vp;
	} */ *ap = v;

	return ((VTOUNION(ap->a_vp)->un_flags & UN_LOCKED) ? 1 : 0);
}

int
union_pathconf(v)
	void *v;
{
	struct vop_pathconf_args /* {
		struct vnode *a_vp;
		int a_name;
		register_t *a_retval;
	} */ *ap = v;
	int error;
	struct vnode *vp = OTHERVP(ap->a_vp);
	struct proc *p = curproc;
	int dolock = (vp == LOWERVP(ap->a_vp));

	if (dolock)
		vn_lock(vp, LK_EXCLUSIVE | LK_RETRY, p);
	else
		FIXUP(VTOUNION(ap->a_vp), p);
	ap->a_vp = vp;
	error = VCALL(vp, VOFFSET(vop_pathconf), ap);
	if (dolock)
		VOP_UNLOCK(vp, 0, p);

	return (error);
}

int
union_revoke(v)
	void *v;
{
	struct vop_revoke_args /* {
		struct vnode *a_vp;
		int a_flags;
		struct proc *a_p;
	} */ *ap = v;
	struct vnode *vp = ap->a_vp;
 
	if (UPPERVP(vp))
		VOP_REVOKE(UPPERVP(vp), ap->a_flags);
	if (LOWERVP(vp))
		VOP_REVOKE(LOWERVP(vp), ap->a_flags);
	vgone(vp);

	return (0);
}

int
union_advlock(v)
	void *v;
{
	struct vop_advlock_args /* {
		struct vnodeop_desc *a_desc;
		struct vnode *a_vp;
		caddr_t  a_id;
		int  a_op;
		struct flock *a_fl;
		int  a_flags;
	} */ *ap = v;
	register struct vnode *vp = OTHERVP(ap->a_vp);

	ap->a_vp = vp;
	return (VCALL(vp, VOFFSET(vop_advlock), ap));
}


/*
 * XXX - vop_strategy must be hand coded because it has no
 * vnode in its arguments.
 * This goes away with a merged VM/buffer cache.
 */
int
union_strategy(v)
	void *v;
{
	struct vop_strategy_args /* {
		struct buf *a_bp;
	} */ *ap = v;
	struct buf *bp = ap->a_bp;
	int error;
	struct vnode *savedvp;

	savedvp = bp->b_vp;
	bp->b_vp = OTHERVP(bp->b_vp);

#ifdef DIAGNOSTIC
	if (bp->b_vp == NULLVP)
		panic("union_strategy: nil vp");
	if (((bp->b_flags & B_READ) == 0) &&
	    (bp->b_vp == LOWERVP(savedvp)))
		panic("union_strategy: writing to lowervp");
#endif

	error = VOP_STRATEGY(bp);
	bp->b_vp = savedvp;

	return (error);
}

