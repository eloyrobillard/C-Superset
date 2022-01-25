#include "9cc.h"

bool is_alnum_(char c)
{
  return isalnum(c) || c == '_';
}

Token *new_token(Token *cur, TK_KIND type, char *str, int len)
{
  Token *tok = calloc(1, sizeof(Token));
  tok->type = type;
  tok->str = str;
  tok->len = len;
  cur->next = tok;

  return tok;
}

Token *tokenize(char *p)
{
  Token head;
  Token *current = calloc(1, sizeof(Token));
  head.next = current;

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
      while (is_alnum_(*(p + i)))
        i++;

      if (i == 2 && strncmp(p, "if", i) == 0)
        current = new_token(current, TK_IF, p, i);
      else if (i == 3 && strncmp(p, "for", i) == 0)
        current = new_token(current, TK_FOR, p, i);
      else if (i == 3 && strncmp(p, "i64", i) == 0)
        current = new_token(current, TK_I64, p, i);
      else if (i == 3 && (strncmp(p, "i32", i) == 0 || strncmp(p, "int", i) == 0))
        current = new_token(current, TK_I32, p, i);
      else if (i == 4 && strncmp(p, "else", i) == 0)
        current = new_token(current, TK_ELSE, p, i);
      else if (i == 4 && strncmp(p, "long", i) == 0)
        current = new_token(current, TK_I64, p, i);
      else if (i == 5 && strncmp(p, "while", i) == 0)
        current = new_token(current, TK_WHILE, p, i);
      else if (i == 6 && strncmp(p, "return", i) == 0)
        current = new_token(current, TK_RETURN, p, i);
      else if (i == 6 && strncmp(p, "sizeof", i) == 0)
        current = new_token(current, TK_SIZEOF, p, i);
      else
        current = new_token(current, TK_IDENT, p, i);
      p += i;
    }
    else if (*p == '/')
    {
      if (*(p + 1) == '/')
        while (*p != '\n' && *(p++) != EOF)
          ;
      else if (*(p + 1) == '*')
      {
        // "/*/" のような場面をスキップ
        p += 2;
        while ((*p != '*' || *(p + 1) != '/') && *p != EOF)
          p++;
        p += 2;
      }
      else
        current = new_token(current, TK_RESERVED, p++, 1);
    }
    else if (strchr(";(),{}-+*&", *p))
      current = new_token(current, TK_RESERVED, p++, 1);
    else if (*p == '!')
    {
      current = new_token(current, TK_RESERVED, p, 2);
      p += 2;
    }
    else if (strchr("=<>", *p))
    {
      if (*(p + 1) == '=')
      {
        current = new_token(current, TK_RESERVED, p, 2);
        p += 2;
      }
      else
        current = new_token(current, TK_RESERVED, p++, 1);
    }
    else if (isdigit(*p))
    {
      current = new_token(current, TK_NUM, p, 1);
      current->val = strtol(p, &p, 10);
    }
    else
      error_at(token->str, "トークナイズできません");
  }

  current = new_token(current, TK_EOF, p, 1);
  return head.next->next;
}