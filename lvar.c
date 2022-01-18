#include "9cc.h"

LVar *new_lvar(char *name, int len)
{
  LVar *lvar = calloc(1, sizeof(LVar));
  lvar->next = locals;
  lvar->name = name;
  lvar->len = len;
  lvar->offset = 8 + (locals ? locals->offset : 0);
  return lvar;
}

LVar *find_lvar(Token *tok)
{
  for (LVar *var = locals; var; var = var->next)
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
      return var;
  return NULL;
}