#include <stdio.h>

#include "9cc.h"

extern Token *tokenize(char *); //! extern は入力不要
extern Node *expr(); //! extern は入力不要
extern void gen(Node *); //! extern は入力不要

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  usr_in = argv[1];
  token = tokenize(argv[1]);
  Node *tree = expr();

  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("\n");
  printf("main:\n");

  gen(tree);

  // スタックトップに式全体の値が残っているはずなので
  // それをRAXにロードして関数からの返り値とする
  printf("\tpop rax\n");
  printf("\tret\n");
  return 0;
}
