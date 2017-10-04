//===-- main.cpp ------------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// This test verifies the correct handling of child thread exits.

#include "pseudo_barrier.h"
#include <thread>
#include <csignal>

pseudo_barrier_t g_barrier1;
pseudo_barrier_t g_barrier2;

void *
thread1 ()
{
  // Synchronize with the main thread.
  pseudo_barrier_wait(g_barrier1);

  // Synchronize with the main thread and thread2.
  pseudo_barrier_wait(g_barrier2);

  // Return
  return NULL; // Should not reach here. (thread2 should raise SIGILL)
}

void *
thread2 ()
{
  raise(SIGILL); // Raise SIGILL

  // Synchronize with thread1 and the main thread.
  pseudo_barrier_wait(g_barrier2); // Should not reach here.

  // Return
  return NULL;
}

int main ()
{
  pseudo_barrier_init(g_barrier1, 2);
  pseudo_barrier_init(g_barrier2, 3);

  // Create a thread.
  std::thread thread_1(thread1);

  // Wait for thread1 to start.
  pseudo_barrier_wait(g_barrier1);

  // Create another thread.
  std::thread thread_2(thread2);

  // Wait for thread2 to start.
  // Second thread should crash but first thread and main thread may reach here.
  pseudo_barrier_wait(g_barrier2);

  return 0;
}
