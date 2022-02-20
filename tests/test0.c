long john()
{
  int a = 1;
  {
    int a = 36;
  }
  int b[2];
  *b = 1;
  *(b + 1) = 2;
  int *k = b;
  bar(b[0]);
  bar(*(k+1));
  int c = *k + *(k + 1);
  bar(c);
  long p[] = {1, 2, 4, 8};
  long *q = p + 2;
  bar(*q);
  q = p + c; // p + 3 -> p[3] = 8
  return bar(*q) + a;
}

int main()
{
  long var;
  var = john();
  long *p;
  p = &var;
  // ポインタ算術、xを参照
  return 2 + *p;
}