/* Origin: PR c/179 from Gray Watson <gray@256.com>, adapted as a testcase
   by Joseph Myers <jsm28@cam.ac.uk>.  */
/* { dg-do compile } */
/* { dg-options "-O2 -Wuninitialized" } */
extern void foo (int *);
extern void bar (int);

void
baz (void)
{
  int i;
  if (i) /* { dg-warning "uninit" "uninit i warning" { xfail *-*-* } } */
    bar (i);
  foo (&i);
}
