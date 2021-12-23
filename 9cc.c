#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef enum TK_Type
{
  DIGIT,
  ALPHA,
  PUNCT,
} TK_TYPE;

typedef struct Token
{
  char *txt;
  int len;
  TK_TYPE type;
} Token;

typedef struct TokenList
{
  Token **tks;
  int len;
  int cap;
} TokenList;

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

void token_keisan(TokenList tkls)
{
  printf("\tmov rax, %d\n", atoi(tkls.tks[0]->txt));
  for (int i = 1; i < tkls.len; i++)
  {
    char operator= tkls.tks[i]->txt[0];
    i++;
    operation(operator, atoi(tkls.tks[i]->txt));
  }
}

Token *new_token(char *txt, int len, TK_TYPE type)
{
  Token *token = malloc(sizeof(Token));
  token->txt = txt;
  token->len = len;
  token->type = type;

  return token;
}

int fill_token(TokenList *tkls, char *input, TK_TYPE type)
{
  int len = 0;

  Token *token;

  switch (type)
  {
  case DIGIT:
    while (isdigit(*input))
    {
      input++;
      len++;
    }

    char *txt = malloc(len);
    input -= len;
    for (int i = 0; i < len; i++)
    {
      txt[i] = input[i];
    }

    token = new_token(txt, len, type);
    break;

  case PUNCT:
    token = new_token(input, 1, type);
    break;

  default:
    return 0;
  }

  if (tkls->len >= tkls->cap - 1)
  {
    tkls->tks = realloc(tkls->tks, sizeof(Token *) * tkls->cap * 2);
    tkls->cap *= 2;
  }

  tkls->tks[tkls->len] = token;
  tkls->len++;

  return token->len;
}

TokenList tokenize(char *input)
{
  TokenList tkls;
  tkls.tks = malloc(sizeof(Token *)),
  tkls.len = 0;
  tkls.cap = 1;

  while (*input)
  {
    if (isdigit(*input))
      input += fill_token(&tkls, input, DIGIT);
    else if (*input == '-' || *input == '+')
      input += fill_token(&tkls, input, PUNCT);
    else
      input++;
  }

  return tkls;
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

  TokenList tkls = tokenize(argv[1]);
  token_keisan(tkls);

  printf("\tret\n");
  return 0;
}
