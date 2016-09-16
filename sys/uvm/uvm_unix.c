/*	$OpenBSD: uvm_unix.c,v 1.60 2016/09/16 01:09:53 dlg Exp $	*/
/*	$NetBSD: uvm_unix.c,v 1.18 2000/09/13 15:00:25 thorpej Exp $	*/

/*
 * Copyright (c) 1997 Charles D. Cranor and Washington University.
 * Copyright (c) 1991, 1993 The Regents of the University of California.  
 * Copyright (c) 1988 University of Utah.
 *
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * the Systems Programming Group of the University of Utah Computer
 * Science Department.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
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
 * from: Utah $Hdr: vm_unix.c 1.1 89/11/07$
 *      @(#)vm_unix.c   8.1 (Berkeley) 6/11/93
 * from: Id: uvm_unix.c,v 1.1.2.2 1997/08/25 18:52:30 chuck Exp
 */

/*
 * uvm_unix.c: traditional sbrk/grow interface to vm.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <sys/resourcevar.h>
#include <sys/vnode.h>

#include <sys/mount.h>
#include <sys/syscallargs.h>

#include <uvm/uvm.h>

/*
 * sys_obreak: set break
 */

int
sys_obreak(struct proc *p, void *v, register_t *retval)
{
	struct sys_obreak_args /* {
		syscallarg(char *) nsize;
	} */ *uap = v;
	struct vmspace *vm = p->p_vmspace;
	vaddr_t new, old, base;
	int error;

	base = (vaddr_t)vm->vm_daddr;
	new = round_page((vaddr_t)SCARG(uap, nsize));
	if (new < base || (new - base) > p->p_rlimit[RLIMIT_DATA].rlim_cur)
		return (ENOMEM);

	old = round_page(base + ptoa(vm->vm_dsize));

	if (new == old)
		return (0);

	/* grow or shrink? */
	if (new > old) {
		error = uvm_map(&vm->vm_map, &old, new - old, NULL,
		    UVM_UNKNOWN_OFFSET, 0,
		    UVM_MAPFLAG(PROT_READ | PROT_WRITE,
		    PROT_READ | PROT_WRITE | PROT_EXEC, MAP_INHERIT_COPY,
		    MADV_NORMAL, UVM_FLAG_FIXED|UVM_FLAG_COPYONW));
		if (error) {
			uprintf("sbrk: grow %ld failed, error = %d\n",
			    new - old, error);
			return (ENOMEM);
		}
		vm->vm_dsize += atop(new - old);
	} else {
		uvm_deallocate(&vm->vm_map, new, old - new);
		vm->vm_dsize -= atop(old - new);
	}

	return (0);
}

/*
 * uvm_grow: enlarge the "stack segment" to include sp.
 */
void
uvm_grow(struct proc *p, vaddr_t sp)
{
	struct vmspace *vm = p->p_vmspace;
	int si;

	/* For user defined stacks (from sendsig). */
	if (sp < (vaddr_t)vm->vm_maxsaddr)
		return;

	/* For common case of already allocated (from trap). */
#ifdef MACHINE_STACK_GROWS_UP
	if (sp < (vaddr_t)vm->vm_maxsaddr + ptoa(vm->vm_ssize))
#else
	if (sp >= (vaddr_t)vm->vm_minsaddr - ptoa(vm->vm_ssize))
#endif
		return;

	/* Really need to check vs limit and increment stack size if ok. */
#ifdef MACHINE_STACK_GROWS_UP
	si = atop(sp - (vaddr_t)vm->vm_maxsaddr) - vm->vm_ssize + 1;
#else
	si = atop((vaddr_t)vm->vm_minsaddr - sp) - vm->vm_ssize;
#endif
	if (vm->vm_ssize + si <= atop(p->p_rlimit[RLIMIT_STACK].rlim_cur))
		vm->vm_ssize += si;
}

#ifndef SMALL_KERNEL

/*
 * Walk the VA space for a process, invoking 'func' on each present range
 * that should be included in a coredump.
 */
int
uvm_coredump_walkmap(struct proc *p, void *iocookie,
    int (*func)(struct proc *, void *, struct uvm_coredump_state *),
    void *cookie)
{
	struct uvm_coredump_state state;
	struct vmspace *vm = p->p_vmspace;
	struct vm_map *map = &vm->vm_map;
	struct vm_map_entry *entry;
	vaddr_t top;
	int error;

	RBT_FOREACH(entry, uvm_map_addr, &map->addr) {
		state.cookie = cookie;
		state.prot = entry->protection;
		state.flags = 0;

		/* should never happen for a user process */
		if (UVM_ET_ISSUBMAP(entry)) {
			panic("%s: user process with submap?", __func__);
		}

		if (!(entry->protection & PROT_WRITE) &&
		    entry->aref.ar_amap == NULL &&
		    entry->start != p->p_p->ps_sigcode)
			continue;

		/* Don't dump mmaped devices. */
		if (entry->object.uvm_obj != NULL &&
		    UVM_OBJ_IS_DEVICE(entry->object.uvm_obj))
			continue;

		state.start = entry->start;
		state.realend = entry->end;
		state.end = entry->end;

		if (state.start >= VM_MAXUSER_ADDRESS)
			continue;

		if (state.end > VM_MAXUSER_ADDRESS)
			state.end = VM_MAXUSER_ADDRESS;

#ifdef MACHINE_STACK_GROWS_UP
		if ((vaddr_t)vm->vm_maxsaddr <= state.start &&
		    state.start < ((vaddr_t)vm->vm_maxsaddr + MAXSSIZ)) {
			top = round_page((vaddr_t)vm->vm_maxsaddr +
			    ptoa(vm->vm_ssize));
			if (state.end > top)
				state.end = top;

			if (state.start >= state.end)
				continue;
#else
		if (state.start >= (vaddr_t)vm->vm_maxsaddr) {
			top = trunc_page((vaddr_t)vm->vm_minsaddr -
			    ptoa(vm->vm_ssize));
			if (state.start < top)
				state.start = top;

			if (state.start >= state.end)
				continue;
#endif
			state.flags |= UVM_COREDUMP_STACK;
		}

		error = (*func)(p, iocookie, &state);
		if (error)
			return (error);
	}

	return (0);
}

#endif	/* !SMALL_KERNEL */
