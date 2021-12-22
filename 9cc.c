#include <stdio.h>
#include <stdlib.h>

void operation(char operator, long num)
{
  switch (operator)
  {
  case '+':
    printf("\tadd rax, %ld\n", num);
    break;
  case '-':
    printf("\tsub rax, %ld\n", num);
    break;
  }
}

int generateKeisan(char *input)
{
  printf("\tmov rax, %ld\n", strtol(input, &input, 10));
  while (*input)
  {
    char operator= * input;
    if (operator != '+' && operator != '-') {
      fprintf(stderr, "予期しない文字です: '%c'\n", operator);
      return 1;
    }
    input++;
    operation(operator, strtol(input, &input, 10));
  }

  return 0;
}

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("\n");
  printf("main:\n");
  if (generateKeisan(argv[1]))
    return 1;
  printf("\tret\n");
  return 0;
}
