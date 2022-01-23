int main ()
{
  long x; long y; long *z;
  x = 3;
  y = 5;
  // ポインタ算術、xを参照
  z = &y + 8 + x - x;
  return *z;
}