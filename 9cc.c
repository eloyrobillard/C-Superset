#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef enum charType
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

typedef struct Tokens
{
  Token **tks;
  int len;
  int cap;
} Tokens;

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
    if (operator!= '+' && operator!= '-')
    {
      fprintf(stderr, "予期しない文字です: '%c'\n", operator);
      return 1;
    }
    input++;
    operation(operator, strtol(input, &input, 10));
  }

  return 0;
}

void tokenKeisan(Tokens tokens)
{
  printf("\tmov rax, %d\n", atoi(tokens.tks[0]->txt));
  for (int i = 1; i < tokens.len; i++)
  {
    char operator= tokens.tks[i]->txt[0];
    i++;
    operation(operator, atoi(tokens.tks[i]->txt));
  }
}

int makeToken(Tokens *tokens, char *input, TK_TYPE type)
{
  int len = 0;

  Token *token = malloc(sizeof(Token));
  token->type = type;

  switch (type)
  {
  case DIGIT:
    while (isdigit(*input))
    {
      input++;
      len++;
    }
    token->txt = malloc(len);
    input -= len;
    for (int i = 0; i < len; i++)
    {
      token->txt[i] = input[i];
    }

    token->len = len;
    break;

  case PUNCT:
    token->txt = malloc(sizeof(char));
    token->txt[0] = *input;
    token->len = 1;
    break;

  default:
    return 0;
  }

  if (tokens->len >= tokens->cap - 1)
  {
    tokens->tks = realloc(tokens->tks, sizeof(Token *) * tokens->cap * 2);
    tokens->cap *= 2;
  }

  tokens->tks[tokens->len] = token;
  tokens->len++;

  return token->len;
}

Tokens tokenize(char *input)
{
  Tokens tokens;
  tokens.tks = malloc(sizeof(Token *)),
  tokens.len = 0;
  tokens.cap = 1;

  while (*input)
  {
    if (isdigit(*input))
      input += makeToken(&tokens, input, DIGIT);
    else if (*input == '-' || *input == '+')
      input += makeToken(&tokens, input, PUNCT);
    else
      input++;
  }

  return tokens;
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

  Tokens tokens = tokenize(argv[1]);
  tokenKeisan(tokens);

  printf("\tret\n");
  return 0;
}
