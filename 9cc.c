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
  printf("main:\n");

  // プロローグ
  // 変数26個分の領域を確保する
  printf("\tpush rbp\n");
  printf("\tmov rbp, rsp\n");
  printf("\tsub rsp, 208\n");

  for (int i = 0; code[i]; i++) {
    gen(code[i]);

    // 式の評価結果としてスタックに一つの値が残っている
    // はずなので、スタックが溢れないようにポップしておく
    printf("\tpop rax\n");
  }

  // エピローグ
  // 最後の式の結果がRAXに残っているのでそれが返り値になる
  printf("\tmov rsp, rbp\n");
  printf("\tpop rbp\n");
  printf("\tret\n");

  fclose(fp);
  return 0;
}
