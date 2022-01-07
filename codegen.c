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

void gen(Node *node)
{
  switch (node->kind)
  {
  case ND_FNCALL:
  {
    // NOTE intentional memory leak (faster + less wordy)
    char *call = calloc(node->call->len, sizeof(char));
    strncpy(call, node->call->str, node->call->len);
    char *args[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
    for (int i = node->call->argc - 1; i >= 0; i--)
    {
      printf("\tpush %s\n", args[i]);
      printf("\tmov %s, %d\n", args[i], node->call->args[i]);
    }
    printf("\tcall %s\n", call);
    for (int i = 0; i < node->call->argc; i++)
    {
      printf("\tpop %s\n", args[i]);
    }
    return;
  }
  case ND_BLOCK:
    for (int i = 0; node->stmts[i]; i++)
    {
      gen(node->stmts[i]);
      printf("\tpop rax\n");
    }
    return;
  case ND_RETURN:
    gen(node->lhs);
    printf("\tpop rax\n");
    // エピローグ
    // 最後の式の結果がRAXに残っているのでそれが返り値になる
    printf("\tmov rsp, rbp\n");
    printf("\tpop rbp\n");
    printf("\tret\n");
    return;
  case ND_IF:
    gen(node->lhs->lhs);
    printf("\tpop rax\n");
    printf("\tcmp rax, 0\n");
    if (node->rhs)
    {
      printf("\tje .Lelse%ld\n", (long)node);
      gen(node->lhs->rhs);
      printf("\tjmp .Lend%ld\n", (long)node);
      printf(".Lelse%ld:\n", (long)node);
      gen(node->rhs);
    }
    else
    {
      printf("\tje .Lend%ld\n", (long)node);
      gen(node->lhs->rhs);
    }
    printf(".Lend%ld:\n", (long)node);
    return;
  case ND_WHILE:
    printf(".Lbegin%ld:\n", (long)node);
    gen(node->lhs);
    printf("\tpop rax\n");
    printf("\tcmp rax, 0\n");
    printf("\tje .Lend%ld\n", (long)node);
    gen(node->rhs);
    printf("\tjmp .Lbegin%ld\n", (long)node);
    printf(".Lend%ld:\n", (long)node);
    return;
  case ND_FOR:
    if (node->lhs->lhs)
      gen(node->lhs->lhs);
    printf(".Lbegin%ld:\n", (long)node);
    gen(node->lhs->rhs);
    printf("\tpop rax\n");
    printf("\tcmp rax, 0\n");
    printf("\tje .Lend%ld\n", (long)node);
    gen(node->rhs->rhs);
    if (node->rhs->lhs)
      gen(node->rhs->lhs);
    printf("\tjmp .Lbegin%ld\n", (long)node);
    printf(".Lend%ld:\n", (long)node);
    return;
  case ND_NUM:
    printf("\tpush %d\n", node->val);
    return;
  case ND_LVAR:
    gen_lval(node);

    printf("\tpop rax\n");
    printf("\tmov rax, [rax]\n");
    printf("\tpush rax\n");
    return;
  case ND_ASSIGN:
    gen_lval(node->lhs);
    gen(node->rhs); // 代入値の処理

    printf("\tpop rdi\n");        // 代入値をゲット
    printf("\tpop rax\n");        // 変数のアドレスをゲット
    printf("\tmov [rax], rdi\n"); // アドレスに代入する
    printf("\tpush rdi\n");       // なんで？
    return;
  default:
    break;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("\tpop rdi\n");
  printf("\tpop rax\n");

  switch (node->kind)
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