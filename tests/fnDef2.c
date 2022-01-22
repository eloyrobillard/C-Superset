long sum(long va, long vb, long v3, long v4)
{
  v4 = 3;
  v3 = 1;
  return va+vb+v3+v4;
}

int main()
{
  long a; long b; long c; long d;
  a=1; b=2; c=3; d=4;
  return a+b+sum(a*2,b+c,c,d);
}