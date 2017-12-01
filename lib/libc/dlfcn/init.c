/*	$OpenBSD: init.c,v 1.6 2017/12/01 23:30:05 guenther Exp $ */
/*
 * Copyright (c) 2014,2015 Philip Guenther <guenther@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */


#define _DYN_LOADER

#include <sys/types.h>
#include <sys/syscall.h>

#ifndef PIC
#include <sys/mman.h>
#endif

#include <tib.h>
#include <limits.h>		/* NAME_MAX */
#include <link.h>
#include <stdlib.h>		/* atexit */
#include <string.h>
#include <unistd.h>

#include "init.h"

#define MAX(a,b)	(((a)>(b))?(a):(b))

#ifdef TIB_EXTRA_ALIGN
# define TIB_ALIGN	MAX(__alignof__(struct tib), TIB_EXTRA_ALIGN)
#else
# define TIB_ALIGN	__alignof__(struct tib)
#endif

/* XXX should be in an include file shared with csu */
char	***_csu_finish(char **_argv, char **_envp, void (*_cleanup)(void));

/* provide definition for this */
int	_pagesize = 0;

/*
 * In dynamicly linked binaries environ and __progname are overriden by
 * the definitions in ld.so.
 */
char	**environ __attribute__((weak)) = NULL;
char	*__progname __attribute__((weak)) = NULL;


#ifndef PIC
struct dl_phdr_info	_static_phdr_info = { .dlpi_name = "a.out" };

static inline void early_static_init(char **_argv, char **_envp);
static inline void setup_static_tib(Elf_Phdr *_phdr, int _phnum);

/* provided by the linker */
extern Elf_Ehdr __executable_start[] __attribute__((weak));
#endif /* PIC */

/*
 * extract useful bits from the auxiliary vector and either
 * a) register ld.so's cleanup in dynamic links, or
 * b) init __progname, environ, and the TIB in static links.
 */
char ***
_csu_finish(char **argv, char **envp, void (*cleanup)(void))
{
	AuxInfo	*aux;
#ifndef PIC
	Elf_Phdr *phdr = NULL;
	int phnum = 0;

	/* static libc in a static link? */
	if (cleanup == NULL)
		early_static_init(argv, envp);
#endif /* !PIC */

	/* Extract useful bits from the auxiliary vector */
	while (*envp++ != NULL)
		;
	for (aux = (void *)envp; aux->au_id != AUX_null; aux++) {
		switch (aux->au_id) {
		case AUX_pagesz:
			_pagesize = aux->au_v;
			break;
#ifndef PIC
		case AUX_base:
			_static_phdr_info.dlpi_addr = aux->au_v;
			break;
		case AUX_phdr:
			phdr = (void *)aux->au_v;
			_static_phdr_info.dlpi_phdr = phdr;
			break;
		case AUX_phnum:
			phnum = aux->au_v;
			_static_phdr_info.dlpi_phnum = phnum;
			break;
#endif /* !PIC */
		}
	}

#ifndef PIC
	if (cleanup == NULL && phdr == NULL && __executable_start != NULL) {
		/*
		 * Static non-PIE processes don't get an AUX vector,
		 * so find the phdrs through the ELF header
		 */
		phdr = (void *)((char *)__executable_start +
		    __executable_start->e_phoff);
		phnum = __executable_start->e_phnum;
	}
	/* static libc in a static link? */
	if (cleanup == NULL)
		setup_static_tib(phdr, phnum);
#endif /* !PIC */

	if (cleanup != NULL)
		atexit(cleanup);

	return &environ;
}

#ifndef PIC
/*
 * static libc in a static link?  Then disable kbind and set up
 * __progname and environ
 */
static inline void
early_static_init(char **argv, char **envp)
{
	static char progname_storage[NAME_MAX+1];

	/* disable kbind */
	syscall(SYS_kbind, (void *)NULL, (size_t)0, (long long)0);

	environ = envp;

	/* set up __progname */
	if (*argv != NULL) {		/* NULL ptr if argc = 0 */
		const char *p = strrchr(*argv, '/');

		if (p == NULL)
			p = *argv;
		else
			p++;
		strlcpy(progname_storage, p, sizeof(progname_storage));
	}
	__progname = progname_storage;
}

/*
 * static TLS handling
 */
#define ELF_ROUND(x,malign)	(((x) + (malign)-1) & ~((malign)-1))

/* for static binaries, the location and size of the TLS image */
static void		*static_tls;
static size_t		static_tls_fsize;

size_t			_static_tls_size = 0;
int			_static_tls_align;
int			_static_tls_align_offset;

static inline void
setup_static_tib(Elf_Phdr *phdr, int phnum)
{
	struct tib *tib;
	char *base;
	int i;

	_static_tls_align = TIB_ALIGN;
	if (phdr != NULL) {
		for (i = 0; i < phnum; i++) {
			if (phdr[i].p_type != PT_TLS)
				continue;
			if (phdr[i].p_memsz == 0)
				break;
			if (phdr[i].p_memsz < phdr[i].p_filesz)
				break;		/* invalid */
			if (phdr[i].p_align > getpagesize())
				break;		/* nope */
			_static_tls_align = MAX(phdr[i].p_align, TIB_ALIGN);
#if TLS_VARIANT == 1
			/*
			 * Variant 1 places the data after the TIB.  If the
			 * TLS alignment is larger than the TIB alignment
			 * then we may need to pad in front of the TIB to
			 * place the TLS data on the proper alignment.
			 * Example: p_align=16 sizeof(TIB)=52 align(TIB)=4
			 * - need to offset the TIB 12 bytes from the start
			 * - to place ths TLS data at offset 64
			 */
			_static_tls_size = phdr[i].p_memsz;
			_static_tls_align_offset =
			    ELF_ROUND(sizeof(struct tib), _static_tls_align) -
			    sizeof(struct tib);
#elif TLS_VARIANT == 2
			/*
			 * Variant 2 places the data before the TIB
			 * so we need to round up the size to the
			 * TLS data alignment TIB's alignment.
			 * Example A: p_memsz=24 p_align=16 align(TIB)=8
			 * - need to allocate 32 bytes for TLS as compiler
			 * - will give the first TLS symbol an offset of -32
			 * Example B: p_memsz=4 p_align=4 align(TIB)=8
			 * - need to allocate 8 bytes so that the TIB is
			 * - properly aligned
			 */
			_static_tls_size = ELF_ROUND(phdr[i].p_memsz,
			    phdr[i].p_align);
			_static_tls_align_offset = ELF_ROUND(_static_tls_size,
			    _static_tls_align) - _static_tls_size;
#endif
			if (phdr[i].p_vaddr != 0 && phdr[i].p_filesz != 0) {
				static_tls = (void *)phdr[i].p_vaddr +
				    _static_phdr_info.dlpi_addr;
				static_tls_fsize = phdr[i].p_filesz;
			}
			break;
		}
	}

	base = mmap(NULL, _static_tls_size + _static_tls_align_offset
	    + sizeof *tib, PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE, -1, 0);

	tib = _static_tls_init(base, NULL);
	tib->tib_tid = getthrid();
	TCB_SET(TIB_TO_TCB(tib));
#if ! TCB_HAVE_MD_GET
	_libc_single_tcb = TIB_TO_TCB(tib);
#endif
}

struct tib *
_static_tls_init(char *base, void *thread)
{
	struct tib *tib;

	base += _static_tls_align_offset;
# if TLS_VARIANT == 1
	tib = (struct tib *)base;
	base += sizeof(struct tib);
# elif TLS_VARIANT == 2
	tib = (struct tib *)(base + _static_tls_size);
# endif

	if (_static_tls_size) {
		if (static_tls != NULL)
			memcpy(base, static_tls, static_tls_fsize);
		memset(base + static_tls_fsize, 0,
		    _static_tls_size - static_tls_fsize);
	}

	TIB_INIT(tib, NULL, thread);
	return tib;
}
#endif /* !PIC */
