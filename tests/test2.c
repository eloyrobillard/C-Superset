long sum(long a, long b, long c, long d)
{
  c = 1;
  d = 3;
  if (a < b)
  {
    long a = 3;
    long *x;
    x = &a;
    long **y;
    y = &x;
    **y = 5;
    return a + b + c + d;
  }
  return a + b + c + d;
}

int main()
{
  long a = 1;
  long b = 2;
  long c = 3;
  long d = 4;
  return a + b + sum(a * 2, b + c, c, d);
}