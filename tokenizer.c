#include "9cc.h"

bool is_alnum(char c)
{
  return isalnum(c) || c == '_';
}

Token *new_token(TK_TYPE type, Token *cur, char *str, int len)
{
  Token *token = calloc(1, sizeof(Token));
  token->type = type;
  token->str = str;
  token->len = len;
  cur->next = token;

  return token;
}

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

    if ('a' <= *p && *p <= 'z' || *p == '_')
    {
      int i = 1;
      while (is_alnum(*(p + i)))
        i++;

      if (i == 6 && strcmp(p, "return"))
        cur = new_token(TK_RETURN, cur, p, i);
      else
        cur = new_token(TK_IDENT, cur, p, i);
      p += i;
    }
    else if (strchr("-+*/();", *p))
      cur = new_token(TK_RESERVED, cur, p++, 1);
    else if (*p == '!')
    {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
    }
    else if (strchr("=<>", *p))
    {
      if (*(p + 1) == '=')
      {
        cur = new_token(TK_RESERVED, cur, p, 2);
        p += 2;
      }
      else
        cur = new_token(TK_RESERVED, cur, p++, 1);
    }
    else if (isdigit(*p))
    {
      cur = new_token(TK_NUM, cur, p, 1);
      cur->val = strtol(p, &p, 10);
    }
    else
      error_at(token->str, "トークナイズできません");
  }

  new_token(TK_EOF, cur, p, 1);
  return head.next;
}