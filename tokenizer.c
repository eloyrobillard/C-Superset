#include "9cc.h"

bool is_alnum_(char c)
{
  return isalnum(c) || c == '_';
}

Token *new_token(TK_KIND type, char *str, int len)
{
  Token *token = calloc(1, sizeof(Token));
  token->type = type;
  token->str = str;
  token->len = len;

  return add_token(*token);
}

Token *tokenize(char *p)
{
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
        new_token(TK_IF, p, i);
      else if (i == 3 && strncmp(p, "for", i) == 0)
        new_token(TK_FOR, p, i);
      else if (i == 4 && strncmp(p, "else", i) == 0)
        new_token(TK_ELSE, p, i);
      else if (i == 5 && strncmp(p, "while", i) == 0)
        new_token(TK_WHILE, p, i);
      else if (i == 6 && strncmp(p, "return", i) == 0)
        new_token(TK_RETURN, p, i);
      else
        new_token(TK_IDENT, p, i);
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
        new_token(TK_RESERVED, p++, 1);
    }
    else if (strchr(";(),{}-+*&", *p))
      new_token(TK_RESERVED, p++, 1);
    else if (*p == '!')
    {
      new_token(TK_RESERVED, p, 2);
      p += 2;
    }
    else if (strchr("=<>", *p))
    {
      if (*(p + 1) == '=')
      {
        new_token(TK_RESERVED, p, 2);
        p += 2;
      }
      else
        new_token(TK_RESERVED, p++, 1);
    }
    else if (isdigit(*p))
    {
      new_token(TK_NUM, p, 1)->val = strtol(p, &p, 10);
    }
    else
      error_at(get_token()->str, "トークナイズできません");
  }

  new_token(TK_EOF, p, 1);
  return get_token();
}