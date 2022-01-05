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
      else if (i == 2 && strcmp(p, "if"))
        cur = new_token(TK_IF, cur, p, i);
      else if (i == 3 && strcmp(p, "for"))
        cur = new_token(TK_FOR, cur, p, i);
      else if (i == 5 && strcmp(p, "while"))
        cur = new_token(TK_WHILE, cur, p, i);
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