long sum(long a, long b, long c, long d)
{
  long b = 3;
  long b = 9;
  long b = 5;
  a = b;
  long b = 4;
  long b = 6;
  long b = 2;
  return a + b + c + d;
}

int main() // free comment city
{
  long a;
  long b;
  long c;
  long d;
  a = 1;
  b = 2;
  c = 3;
  d = 4;
  long *x1;
  alloc4(&x1, 1, 2, 4, 8);
  long *x2 = x1 + 2;
  long *y = &c;
  *y = *y + 10 * (x2 - x1);
  return *y + sum(a, b, c, d);
}