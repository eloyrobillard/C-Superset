#include <stdio.h>
#include <stdlib.h>

void ialloc4(int **p, int a, int b, int c, int d)
{
  (*p) = malloc(sizeof(int) * 4);
  (*p)[0] = a;
  (*p)[1] = b;
  (*p)[2] = c;
  (*p)[3] = d;
}

void alloc4(long **p, long a, long b, long c, long d)
{
  (*p) = malloc(sizeof(long) * 4);
  (*p)[0] = a;
  (*p)[1] = b;
  (*p)[2] = c;
  (*p)[3] = d;
}

int foo(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j, int k)
{
  printf("%d %d %d %d %d %d %d %d %d %d %d\n", a, b, c, d, e, f, g, h, i, j, k);
  return 0;
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