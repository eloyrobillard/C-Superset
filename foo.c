#include <stdio.h>
#include <stdlib.h>

void ialloc4(int **p, int a, int b, int c, int d)
{
  int *ar = *p;
  *p = malloc(sizeof(int) * 4);
  ar[0] = a;
  ar[1] = b;
  ar[2] = c;
  ar[3] = d;
}

void alloc4(long **p, long a, long b, long c, long d)
{
  long *ar = *p;
  *p = malloc(sizeof(long) * 4);
  ar[0] = a;
  ar[1] = b;
  ar[2] = c;
  ar[3] = d;
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