int main()
{
  int a;
  int *x;
  x = &a;
  int **y;
  y = &x;
  **y = 3;
  return a;
}