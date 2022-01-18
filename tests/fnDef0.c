long bar(long);

long john()
{
  long a;
  a=1;
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