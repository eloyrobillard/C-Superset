#include "9cc.h"

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  FILE *fp = fopen(argv[1], "r");

  char *input = calloc(255, sizeof(char));
  fread(input, 1, 255, fp);

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
