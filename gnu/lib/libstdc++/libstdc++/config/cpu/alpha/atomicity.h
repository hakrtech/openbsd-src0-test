// Low-level functions for atomic operations: Alpha version  -*- C++ -*-

// Copyright (C) 1999, 2000, 2001 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
// USA.

// As a special exception, you may use this file as part of a free software
// library without restriction.  Specifically, if other files instantiate
// templates or use macros or inline functions from this file, or you compile
// this file and link it with other files to produce an executable, this
// file does not by itself cause the resulting executable to be covered by
// the GNU General Public License.  This exception does not however
// invalidate any other reasons why the executable file might be covered by
// the GNU General Public License.

#ifndef _BITS_ATOMICITY_H
#define _BITS_ATOMICITY_H	1

/* @@@ With gas we can play nice .subsection games to get the
   non-predicted branch pointing forward.  But Digital assemblers
   don't understand those directives.  This isn't a terribly
   important issue, so just ignore it.  */

typedef int _Atomic_word;

static inline _Atomic_word
__attribute__ ((__unused__))
__exchange_and_add (volatile _Atomic_word* __mem, int __val)
{
  register int __result, __tmp;

  __asm__ __volatile__ (
      "\n$Lxadd_%=:\n\t"
      "ldl_l  %0,%3\n\t"
      "addl   %0,%4,%1\n\t"
      "stl_c  %1,%2\n\t"
      "beq    %1,$Lxadd_%=\n\t"
      "mb"
      : "=&r"(__result), "=&r"(__tmp), "=m"(*__mem)
      : "m" (*__mem), "r"(__val));

  return __result;
}

static inline void
__attribute__ ((__unused__))
__atomic_add (volatile _Atomic_word* __mem, int __val)
{
  register _Atomic_word __result;

  __asm__ __volatile__ (
      "\n$Ladd_%=:\n\t"
      "ldl_l  %0,%2\n\t"
      "addl   %0,%3,%0\n\t"
      "stl_c  %0,%1\n\t"
      "beq    %0,$Ladd_%=\n\t"
      "mb"
      : "=&r"(__result), "=m"(*__mem)
      : "m" (*__mem), "r"(__val));
}

#endif /* atomicity.h */
