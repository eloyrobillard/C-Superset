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
  case ND_FNDEF:
  {
    printf("\n");
    printf("%.*s:\n", node->def->len, node->def->name);

    // プロローグ
    // 変数26個分の領域を確保する
    printf("\tpush rbp\n");
    printf("\tmov rbp, rsp\n");
    printf("\tsub rsp, 288\n");

    char *args[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
    for (int i = 0; i < node->def->paramc; i++)
    {
      // 引数のアドレスの確保
      gen_lval(node->def->params[i]);
      printf("\tpop rax\n");
      // 引数の値の獲得
      if (i < 6)
        printf("\tmov [rax], %s\n", args[i]); // アドレスに代入する
      else
      {
        printf("\tpush rdi\n");
        printf("\tmov rdi, [rbp+%d]\n", 16 + 8 * (i-6));
        printf("\tmov [rax], rdi\n");
        printf("\tpop rdi\n");
      }
      // printf("\tpush [rax]\n");  // スタックで値を提供する
    }

    int i = 0;
    while (node->def->body->stmts[i+1])
    {
      gen(node->def->body->stmts[i++]);

      // 式の評価結果としてスタックに一つの値が残っている
      // はずなので、スタックが溢れないようにポップしておく
      printf("\tpop rax\n");
    }

    // NOTE: 関数にリターンがない際に備え
    gen(node->def->body->stmts[i]);
    if (node->def->body->stmts[i]->kind != ND_RETURN)
    {
      // エピローグ
      // 最後の式の結果がRAXに残っているのでそれが返り値になる
      printf("\tmov rax, 0\n");
      printf("\tmov rsp, rbp\n");
      printf("\tpop rbp\n");
      printf("\tret\n");
    }

    return;
  }
  case ND_FNCALL:
  {
    char *args[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
    // 引数渡し
    // int regc = node->call->argc > 6 ? 6 : node->call->argc;
    // for (int i = regc - 1; i >= 0; i--)
    //   printf("\tpush %s\n", args[i]);

    int i = node->call->argc - 1;
    if (node->call->argc > 6)
      for (; i >= 6; i--)
        gen(node->call->args[i]);

    for (; i >= 0; i--)
    {
      gen(node->call->args[i]);
      printf("\tpop rax\n");
      printf("\tmov %s, rax\n", args[i]);
    }

    // RSPが１６倍数のアドレスを参照している約束
    // printf("\tpush rax\n");
    // printf("\tpush rdx\n");
    // printf("\tpush r8\n");
    // printf("\tmov rax, rsp\n");
    // printf("\txor rdx, rdx\n");
    // printf("\tmov r8, 16\n");
    // printf("\tcqo\n");
    // printf("\tdiv r8\n");
    // printf("\tcmp rdx, 0\n");
    // printf("\tpop r8\n");
    // printf("\tpop rdx\n");
    // printf("\tpop rax\n");
    // printf("\tjz .Lend%ld\n", (long)node);
    // printf("\tadd rsp, 8\n");
    // printf("\tjmp .Lend%ld\n", (long)node);
    // printf(".Lend%ld:\n", (long)node);

    printf("\tcall %.*s\n", node->call->len, node->call->str);
    // for (int j = 0; j < regc; j++)
    //   printf("\tpop %s\n", args[j]);
    for (int j = node->call->argc - 1; j >= 6; j--)
      printf("\tpop rdi\n");
    // スタックに入ってる引数を捨てるために
    printf("\tpush rax\n"); // callの返し値をスタックに保存
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
  {
    gen(node->lhs);
    printf("\tpop rax\n");
    // エピローグ
    // 最後の式の結果がRAXに残っているのでそれが返り値になる
    printf("\tmov rsp, rbp\n");
    printf("\tpop rbp\n");
    printf("\tret\n");
    return;
  }
  case ND_IF:
  {
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
  }
  case ND_WHILE:
  {
    printf(".Lbegin%ld:\n", (long)node);
    gen(node->lhs);
    printf("\tpop rax\n");
    printf("\tcmp rax, 0\n");
    printf("\tje .Lend%ld\n", (long)node);
    gen(node->rhs);
    printf("\tjmp .Lbegin%ld\n", (long)node);
    printf(".Lend%ld:\n", (long)node);
    return;
  }
  case ND_FOR:
  {
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
  }
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
  {
    gen_lval(node->lhs);
    gen(node->rhs); // 代入値の処理

    printf("\tpop rdi\n");        // 代入値をゲット
    printf("\tpop rax\n");        // 変数のアドレスをゲット
    printf("\tmov [rax], rdi\n"); // アドレスに代入する
    printf("\tpush rdi\n");       // スタックで値を提供する
    return;
  }
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