long glob[10];

long sum()
{
  int a = glob[0];
  int b = glob[1];
  int c = glob[2];
  int d = glob[3];
  return if (glob[0] * 2 == glob[1]) 5
  else if (a < b)
  {
    long a = 3;
    long *x;
    x = &a;
    long **y = &x;
    **y = 5;
    a + b + c + d
  }
  else a + b + c + d;
}

int main()
{
  glob[0] = 1;
  glob[1] = 2;
  glob[2] = 3;
  glob[3] = 4;
  foo(glob[0], glob[1], glob[2], glob[3], glob, 0, 0, 0, 0, 0, 0, 0);
  return glob[0] + glob[1] + sum();
}