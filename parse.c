#include "9cc.h"

// Reports an error and exit.
void error(char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void error_at(char *loc, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);

  int off = loc - usr_in;
  fprintf(stderr, "%s\n", usr_in);
  fprintf(stderr, "%*s", off, " "); // pos個の空白を出力
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

bool at_eof()
{
  return token->type == TK_EOF;
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
  lvar->name = name;
  lvar->len = len;
  // オフセットの計算

  return lvar;
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

    if ('a' <= *p && *p <= 'z')
    {
      int i = 1;
      while('a' <= *(p+i) && *(p+i) <= 'z')
        i++;
      char *name = calloc(i, sizeof(char));
      memcpy(name, p, i);
      locals->next = new_lvar(name, 1);
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

Node *new_node(NodeKind kind, Node *lhs, Node *rhs)
{
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val)
{
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

bool consume(char *op)
{
  if (token->type != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
    return false;

  token = token->next;
  return true;
}

Token *consume_ident()
{
  if (token->type != TK_IDENT)
    return NULL;

  Token *tok = token;
  token = token->next;
  return tok;
}

void expect(char *op)
{
  if (token->type != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
    error_at(token->str, "'%c'ではありません\n", op);
  token = token->next;
}

long expect_num()
{
  if (token->type != TK_NUM)
    error_at(token->str, "数ではありません\n");
  int val = token->val;
  token = token->next;
  return val;
}

Node *primary()
{
  // 次のトークンが"("なら、"(" expr ")"のはず
  if (consume("("))
  {
    Node *node = expr();
    expect(")");
    return node;
  }

  Token *tok = consume_ident();
  if (tok)
  {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;
    node->offset = (tok->str[0] - 'a' + 1) * 8;
    return node;
  }

  // そうでなければ数値のはず
  return new_node_num(expect_num());
}

Node *unary()
{
  if (consume("-"))
    return new_node(ND_SUB, new_node_num(0), primary());

  consume("+");
  return primary();
}

Node *mul()
{
  Node *node = unary();

  for (;;)
  {
    if (consume("*"))
      node = new_node(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_node(ND_DIV, node, unary());
    else
      return node;
  }
}

Node *add()
{
  Node *node = mul();

  for (;;)
  {
    if (consume("+"))
      node = new_node(ND_ADD, node, mul());
    else if (consume("-"))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
  }
}

Node *relational()
{
  Node *node = add();

  for (;;)
  {
    if (consume("<"))
      node = new_node(ND_LESS, node, add());
    else if (consume("<="))
      node = new_node(ND_LEQ, node, add());
    else if (consume(">"))
      node = new_node(ND_LESS, add(), node);
    else if (consume(">="))
      node = new_node(ND_LEQ, add(), node);
    else
      return node;
  }
}

Node *equality()
{
  Node *node = relational();

  for (;;)
  {
    if (consume("=="))
      node = new_node(ND_EQ, node, relational());
    else if (consume("!="))
      node = new_node(ND_NEQ, node, relational());
    else
      return node;
  }
}

Node *assign()
{
  Node *node = equality();
  if (consume("="))
    node = new_node(ND_ASSIGN, node, assign());
  return node;
}

Node *expr()
{
  return assign();
}

Node *stmt()
{
  Node *node = expr();
  expect(";");
  return node;
}

void program()
{
  int i = 0;
  while (!at_eof())
    code[i++] = stmt();
  code[i] = NULL;
}