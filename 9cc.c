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
  TK_TYPE type;
} Token;

typedef struct Tokens
{
  Token *tks;
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
  printf("%d", atoi(tokens.tks->txt));
  for (int i = 1; i < tokens.len; i++)
  {
    operation(tokens.tks[i].txt[0], atoi(tokens.tks[++i].txt));
  }
}


void makeToken(Tokens tokens, char *input, TK_TYPE type)
{
  int len = 0;
  char *res;

  switch (type)
  {
  case DIGIT:
    while (isdigit(*input))
    {
      input++;
      len++;
    }
    res = malloc(len);
    input -= len;
    for (int i = 0; i <= len; i++)
    {
      res[i] = input[i];
    }
    break;

  case PUNCT:
    res = malloc(1);
    res[0] = *input;
    break;

  default:
    return;
  }

  if (tokens.len >= tokens.cap - 1)
  {
    tokens.tks = realloc(tokens.tks, tokens.cap * 2);
    tokens.cap *= 2;
  }
  Token token;
  token.txt = res;
  token.type = type;

  tokens.tks[tokens.len] = token;
  tokens.len++;
}

Tokens tokenize(char *input)
{
  Tokens tokens;
  tokens.tks = malloc(sizeof(char *)),
  tokens.len = 0;
  tokens.cap = 1;

  while (*input)
  {
    if (isdigit(*input))
      makeToken(tokens, input, DIGIT);
    else if (*input == '-' || *input == '+')
      makeToken(tokens, input, PUNCT);

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

  tokenKeisan(tokenize(argv[1]));

  printf("\tret\n");
  return 0;
}
