//===-- main.c --------------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <stdio.h>

int a(int);
int b(int);
int c(int);
const char *print_string = "aaaaaaaaaa\n";

int a(int val)
{
    int return_value = val;  // basic break at the start of b

    if (val <= 1)
    {
        return_value =  b(val); // break here to stop in a before calling b
    }
    else if (val >= 3)
    {
        return_value = c(val);
    }

    return return_value;
}

int b(int val)
{
    int rc = c(val); // thread step-out while stopped at "c(2)"
    return rc;
}

int c(int val)
{
    return val + 3; // Find the line number of function "c" here.
}

int complex (int first, int second, int third)
{
    return first + second + third;  // Step in targeting complex should stop here
}

int main (int argc, char const *argv[])
{
    int A1 = a(1); // frame select 2, thread step-out while stopped at "c(1)"
    printf("a(1) returns %d\n", A1);
    
    int B2 = b(2);
    printf("b(2) returns %d\n", B2);
    
    int A3 = a(3); // frame select 1, thread step-out while stopped at "c(3)"
    printf("a(3) returns %d\n", A3);
    
    int A4 = complex (a(1), b(2), c(3)); // Stop here to try step in targeting b.

    int A5 = complex (a(2), b(3), c(4)); // Stop here to try step in targeting complex.

    int A6 = complex (a(4), b(5), c(6)); // Stop here to step targeting b and hitting breakpoint.

    int A7 = complex (a(5), b(6), c(7)); // Stop here to make sure bogus target steps over.

    printf ("I am using print_string: %s.\n", print_string);
    return 0;
}
