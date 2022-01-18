long sum(long v1, long v2, long v3, long v4)
{
  return v1+v2+v3+v4;
}

int main()
{
  long a; long b; long c; long d;
  a=1; b=2; c=3; d=4;
  return a + sum(a,b,c,d);
}