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
      continue; // 不正な文字を検出するため
    }

    if ('a' <= *p && *p <= 'z' || *p == '_')
    {
      int i = 1;
      while (is_alnum(*(p + i)))
        i++;

      switch (i)
      {
      case 2:
        if (strcmp(p, "if") == 0)
        {
          cur = new_token(TK_IF, cur, p, i);
          break;
        }
      case 3:
        if (strcmp(p, "for") == 0)
        {
          cur = new_token(TK_FOR, cur, p, i);
          break;
        }
      case 5:
        if (strcmp(p, "while") == 0)
        {
          cur = new_token(TK_WHILE, cur, p, i);
          break;
        }
      case 6:
        if (strcmp(p, "return") == 0)
        {
          cur = new_token(TK_RETURN, cur, p, i);
          break;
        }
      default:
        cur = new_token(TK_IDENT, cur, p, i);
        break;
      }
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