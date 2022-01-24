long weird_op(long a, long b, long c, long d, long e, long f, long g)
{
  foo(a, b, c, d, e, f, g, 0, 0, 0, 0);
  return a * b * c * d / (e * f * g);
}

long fred(long a, long b, long c, long d, long e, long f, long g, long h, long i, long j, long k)
{
  foo(a, b, c, d, e, f, g, h, i, j, k);
  return a + b + c + d + e + f + g + h + i + j + k + weird_op(3 * (b < e), 2, 4 * g, 1, 1, 2, 2);
}

int main()
{
  long a;
  long b;
  long c;
  long d;
  long e;
  long f;
  long g;
  long h;
  long i;
  long j;
  long k;
  a = 1;
  b = 2;
  c = 3;
  d = 4;
  e = 5;
  f = 6;
  g = 7;
  h = 8;
  i = 9;
  j = 10;
  k = 11;
  if (a < g)
  {
    long a;
    a = 100;
  }
  long out;
  out = j - c + g + fred(a * 2, b, c + 1, d, e, f, g, h, i, j, k);
  foo(a, b, c, d, e, f, g, h, i, j, k);
  return out;
}
