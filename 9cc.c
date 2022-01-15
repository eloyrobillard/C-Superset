#include "9cc.h"

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  FILE *fp = fopen(argv[1], "r");

  int in_max = 256;
  char *input = calloc(256, sizeof(char));
  char ch = fgetc(fp);
  for (int i = 0; ch != EOF; i++)
  {
    input[i] = ch;
    if (i+1 == in_max)
      input = realloc(input, sizeof(char) * (in_max *= 2));
    ch = fgetc(fp);
  }

  usr_in = input;
  token = tokenize(input);
  program();

  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("\n");

  for (int i = 0; code[i]; i++)
    gen(code[i]);

  fclose(fp);
  return 0;
}
