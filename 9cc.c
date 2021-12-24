#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

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

void error(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
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
    error("'%c'ではありません\n", op);
  token = token->next;
}

long expect_num()
{
  if (token->type != TK_NUM)
    error("数ではありません\n");
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
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (*p == '-' || *p == '+')
      cur = new_token(TK_RESERVED, cur, p++);
    else if (isdigit(*p))
    {
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
    }
    else
      error("トークナイズできません");
  }

  new_token(TK_EOF, cur, p);
  return head.next;
}

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  token = tokenize(argv[1]);

  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("\n");
  printf("main:\n");

  printf("\tmov rax, %ld\n", expect_num());
  while (!at_eof())
  {
    if (consume('+'))
    {
      printf("\tadd rax, %ld\n", expect_num());
    }

    expect('-');
    printf("\tsub rax, %ld\n", expect_num());
  }

  printf("\tret\n");
  return 0;
}
