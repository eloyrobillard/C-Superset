long john()
{
  long a = 1;
  {
    long a = 36;
  }
  /*
  a = 11;
  */
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