#include "9cc.h"

unsigned max = 256;
unsigned top;
unsigned current;

Token *tokens;

void init_tokens()
{
  tokens = calloc(max, sizeof(Token));
}

Token *add_token(Token tok)
{
  if (top + 1 >= max)
    tokens = realloc(tokens, (max *= 2) * sizeof(Token));
  tokens[top] = tok;
  return &tokens[top++];
}

Token *get_token()
{
  return &tokens[current];
}

void next_token()
{
  current++;
}