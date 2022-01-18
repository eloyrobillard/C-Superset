#include <stdio.h>

int foo(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j, int k)
{
  printf("%d %d %d %d %d %d %d %d %d %d %d\n", a, b, c, d, e, f, g, h, i, j, k);
}

int baz(int a, int b, int c, int d, int e, int f, int g)
{
  printf("%d %d %d %d %d %d %d\n", a, b, c, d, e, f, g);
  return c;
}

int bar(int a)
{
  printf("%d\n", a);
  return a;
}

int hoge()
{
  return 2;
}