/*    $OpenBSD: kmem.h,v 1.5 1998/01/26 04:13:44 dgregor Exp $    */
/*
 * Copyright (C) 1993-1997 by Darren Reed.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that this notice is preserved and due credit is given
 * to the original author and the contributors.
 * $Id: kmem.h,v 1.5 1998/01/26 04:13:44 dgregor Exp $
 */

#ifndef	__KMEM_H__
#define	__KMEM_H__

#ifndef	__P
# ifdef	__STDC__
#  define	__P(x)	x
# else
#  define	__P(x)	()
# endif
#endif
extern	int	openkmem __P((void));
extern	int	kmemcpy __P((char *, long, int));

#define	KMEM	"/dev/kmem"

#endif /* __KMEM_H__ */
