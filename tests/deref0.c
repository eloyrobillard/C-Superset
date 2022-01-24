int main ()
{
  // this is a comment
  long x = 3;
  long *y = &x;
  *y = *y + 20;
  return *y;
}