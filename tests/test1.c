long global = 5;

long sum(long a, long b, long c, long d)
{
  long b = 3;
  long b = 9;
  long b = 5;
  a = global;
  long b = 4;
  long b = 6;
  long b = 2;
  foo(a, b, c, d, global, 0, 0, 0, 0, 0, 0, 0);
  return a + b + c + d;
}

int main() // free comment city
{
  long a[] = {1, 2, 3, sizeof 4};
  int x1[4];
  ialloc4(&x1, 1, 2, 4, 8);
  int *x2 = x1 + 2;
  long *y = &*&*&a[2];
  *y = *y + 10 * (x2 - x1);
  foo(a[0], a[1], a[2], a[3], *y, global, 0, 0, 0, 0, 0, 0);
  return *y + sum(a[0], a[1], a[2], a[3]);
  // 23 + 5 + 2 + 23 + 4 = 57
}