long sum(long a, long b, long c, long d)
{
  c = 1;
  d = 3;
  return a+b+c+d;
}

int main()
{
  long a; long b; long c; long d;
  a=1; b=2; c=3; d=4;
  return a+b+sum(a*2,b+c,c,d);
}