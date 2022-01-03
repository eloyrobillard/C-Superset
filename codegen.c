#include "9cc.h"

void gen(Node *tree)
{
  if (tree->kind == ND_NUM)
  {
    printf("\tpush %d\n", tree->val);
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