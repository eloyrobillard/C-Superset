#include <stdio.h>

#include "9cc.h"

void gen_lval(Node *node)
{
  // 変数の確認
  if (node->kind != ND_LVAR)
    error("代入の左辺値が変数ではありません");
  // アドレスの計算
  printf("\tmov rax, rbp\n");
  printf("\tsub rax, %d\n", node->offset);
  // アドレスをプッシュ
  printf("\tpush rax\n");
}

void gen(Node *tree)
{
  switch (tree->kind)
  {
  case ND_NUM:
    printf("\tpush %d\n", tree->val);
    return;
  case ND_LVAR:
    gen_lval(tree);

    printf("\tpop rax\n");
    printf("\tmov rax, [rax]\n");
    printf("\tpush rax\n");
    return;
  case ND_ASSIGN:
    gen_lval(tree->lhs);
    gen(tree->rhs); // 代入値の処理

    printf("\tpop rdi\n");  // 代入値をゲット
    printf("\tpop rax\n");  // 変数のアドレスをゲット
    printf("\tmov [rax], rdi\n"); // アドレスに代入する
    printf("\tpush rdi\n"); // なんで？
    return;
  }

  gen(tree->lhs);
  gen(tree->rhs);

  printf("\tpop rdi\n");
  printf("\tpop rax\n");

  switch (tree->kind)
  {
  case ND_EQ:
    printf("\tcmp rax, rdi\n");
    printf("\tsete al\n");
    printf("\tmovzb rax, al\n");
    break;
  case ND_NEQ:
    printf("\tcmp rax, rdi\n");
    printf("\tsetne al\n");
    printf("\tmovzb rax, al\n");
    break;
  case ND_LESS:
    printf("\tcmp rax, rdi\n");
    printf("\tsetl al\n");
    printf("\tmovzb rax, al\n");
    break;
  case ND_LEQ:
    printf("\tcmp rax, rdi\n");
    printf("\tsetle al\n");
    printf("\tmovzb rax, al\n");
    break;
  case ND_ADD:
    printf("\tadd rax, rdi\n");
    break;
  case ND_SUB:
    printf("\tsub rax, rdi\n");
    break;
  case ND_MUL:
    printf("\timul rax, rdi\n");
    break;
  case ND_DIV:
    printf("\tcqo\n");
    printf("\tidiv rdi\n");
    break;
  default:
    break;
  }

  printf("\tpush rax\n");
}