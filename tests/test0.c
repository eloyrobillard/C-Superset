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
  // ポインタ算術、xを参照
  /* z = &y + 8 + x; */
  return 2 + *p;
}