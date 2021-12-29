#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/*
 * expr    = mul ("+" mul | "-" mul)*
 * mul     = primary ("*" primary | "/" primary)*
 * primary = num | "(" expr ")"
 */

// 抽象構文木のノードの種類
typedef enum
{
  ND_ADD, //* +
  ND_SUB, //* -
  ND_MUL, //* *
  ND_DIV, //* /
  ND_NUM, //* 整数
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node
{
  NodeKind kind; //! ノードの型
  Node *lhs;     //! 左辺
  Node *rhs;     //! 右辺
  int val;       //! kindがND_NUMの場合のみ使う
};

Node *new_node(NodeKind kind, Node *lhs, Node *rhs)
{
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val)
{
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

typedef enum TK_Type
{
  TK_RESERVED,
  TK_NUM,
  TK_EOF
} TK_TYPE;

typedef struct Token Token;
struct Token
{
  TK_TYPE type;
  struct Token *next;
  long val;  // for int tokens
  char *str; // for all tokens
};

Token *token;
char *usr_in;

void error_at(char *loc, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);

  int off = loc - usr_in;
  fprintf(stderr, "%s\n", usr_in);
  fprintf(stderr, "%*s", off, " "); // pos個の空白を出力
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

bool consume(char op)
{
  if (token->type != TK_RESERVED || token->str[0] != op)
    return false;
  token = token->next;
  return true;
}

void expect(char op)
{
  if (token->type != TK_RESERVED || token->str[0] != op)
    error_at(token->str, "'%c'ではありません\n", op);
  token = token->next;
}

long expect_num()
{
  if (token->type != TK_NUM)
    error_at(token->str, "数ではありません\n");
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof()
{
  return token->type == TK_EOF;
}

Token *new_token(TK_TYPE type, Token *cur, char *str)
{
  Token *token = calloc(1, sizeof(Token));
  token->type = type;
  token->str = str;
  cur->next = token;

  return token;
}

Token *tokenize(char *p)
{
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p)
  {
    // 空白文字をスキップ
    if (isspace(*p))
    {
      p++;
      continue;
    }

    if (*p == '-' || *p == '+' || *p == '*' || *p == '/' || *p == '(' || *p == ')')
      cur = new_token(TK_RESERVED, cur, p++);
    else if (isdigit(*p))
    {
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
    }
    else
      error_at(token->str, "トークナイズできません");
  }

  new_token(TK_EOF, cur, p);
  return head.next;
}

Node *expr();

Node *primary()
{
  // 次のトークンが"("なら、"(" expr ")"のはず
  if (consume('('))
  {
    Node *node = expr();
    expect(')');
    return node;
  }

  // そうでなければ数値のはず
  return new_node_num(expect_num());
}

Node *unary()
{
  if (consume('-'))
    return new_node(ND_SUB, new_node_num(0), primary());

  consume('+');
  return primary();
}

Node *mul()
{
  Node *node = primary();

  for (;;)
  {
    if (consume('*'))
      node = new_node(ND_MUL, node, unary());
    else if (consume('/'))
      node = new_node(ND_DIV, node, unary());
    else
      return node;
  }
}

Node *expr()
{
  Node *node = mul();

  for (;;)
  {
    if (consume('+'))
      node = new_node(ND_ADD, node, mul());
    else if (consume('-'))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
  }
}

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
