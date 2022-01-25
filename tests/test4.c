long fib(long n)
{
  return if (n == 0) 0 else if (n == 1) 1 else fib(n - 2) + fib(n - 1);
}

int main()
{
  return fib(10);
}