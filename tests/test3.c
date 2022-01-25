long weird_op(long a, long b, long c, long d, long e, long f, long g)
{
  foo(a, b, c, d, e, f, g, 0, 0, 0, 0);
  return a * b * c * d / (e * f * g);
}

long fred(long a, long b, long c, long d, long e, long f, long g, long h, long i, long j, long k)
{
  foo(sizeof sizeof a, b, c, d, e, f, g, h, i, j, k);
  return a + b + c + d + e + f + g + h + i + j + k + weird_op(3 * (b < e), 2, 4 * g, 1, 1, 2, 2);
}

int main()
{
  long *p;
  alloc4(&p, 0,1,2,3);
  long *p2 = p + 2;
  long a = 1;
  long b = 2;
  long c = p2 - p + 1;
  long d = 4;
  long e = 5;
  long f = 6;
  long g = 7;
  long h = 8;
  long i = 9;
  long j = 10;
  long k = 11;

  a = if (a > g)
  {
    long a = 10
  } else a;
  long out = j - c + g + fred(a * 2, b, c + 1, d, e, f, g, h, i, j, k);
  foo(a, b, c, d, e, f, g, h, i, j, k);
  return out;
}
