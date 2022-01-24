long john()
{
  long a;
  a=1;
  {
    long a;
    a = 36;
  }
  return a + bar(a);
}

int main()
{
  long var;
  var = john();
  long *p;
  p = &var;
  return 2 + *p;
}