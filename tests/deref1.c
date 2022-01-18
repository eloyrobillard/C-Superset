int main ()
{
  long x; long y; long *z;
  x = 3;
  y = 5;
  z = &y + 8 + x - x;
  return *z;
}