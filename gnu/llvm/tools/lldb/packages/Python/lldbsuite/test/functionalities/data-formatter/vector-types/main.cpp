//===-- main.cpp ------------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

typedef float float4 __attribute__((ext_vector_type(4)));
typedef  unsigned char vec __attribute__((ext_vector_type(16)));

int main() {
    float4 f4 = {1.25, 1.25, 2.50, 2.50};
    vec v = (vec)f4;
    return 0; // break here
}
