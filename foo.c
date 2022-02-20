#include <stdio.h>
#include <stdlib.h>

void ialloc4(long **p, long a, long b, long c, long d)
{
  long *ar = *p;
  *p = malloc(sizeof(long) * 4);
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

long foo(long a, long b, long c, long d, long e, long f, long g, long h, long i, long j, long k)
{
  printf("%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld\n", a, b, c, d, e, f, g, h, i, j, k);
  return 0;
}

long baz(long a, long b, long c, long d, long e, long f, long g)
{
  printf("%ld %ld %ld %ld %ld %ld %ld\n", a, b, c, d, e, f, g);
  return c;
}

long bar(long a)
{
  printf("%ld\n", a);
  return a;
}

long hoge()
{
  return 2;
}