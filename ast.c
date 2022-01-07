#include "9cc.h"

bool at_eof()
{
  return token->type == TK_EOF;
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

Node *new_if_node(Node *cond, Node *ifstmt, Node *els)
{
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_IF;
  node->lhs = new_node((NodeKind)NULL, cond, ifstmt);
  node->rhs = els;
  return node;
}

bool consume(char *op)
{
  if (token->type != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
    return false;

  token = token->next;
  return true;
}

bool consume_keyword(TK_TYPE type)
{
  if (token->type != type)
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
    if (consume("("))
    {
      expect(")");
      node->kind = ND_FNCALL;
      node->call = calloc(1, sizeof(FnCall));
      node->call->str = tok->str;
      node->call->len = tok->len;
      return node;
    }
    node->kind = ND_LVAR;

    LVar *lvar = find_lvar(tok);
    if (lvar)
      node->offset = lvar->offset;
    else
    {
      LVar *lvar = new_lvar(tok->str, tok->len);
      node->offset = lvar->offset;
      locals = lvar;
    }

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

Node *handle_if()
{
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_IF;
  expect("(");
  Node *if_node = calloc(1, sizeof(Node));
  if_node->lhs = expr();
  expect(")");
  if_node->rhs = stmt();
  node->lhs = if_node;

  if (consume_keyword(TK_ELSE))
    node->rhs = stmt();
  return node;
}

Node *handle_for()
{
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_FOR;
  Node *n0 = calloc(1, sizeof(Node));
  node->lhs = n0;
  n0->rhs = new_node_num(1);
  Node *n1 = calloc(1, sizeof(Node));
  node->rhs = n1;
  expect("(");
  if (!consume(";"))
  {
    n0->lhs = expr();
    expect(";");
  }
  if (!consume(";"))
  {
    n0->rhs = expr();
    expect(";");
  }
  if (!consume(")"))
  {
    n1->lhs = expr();
    expect(")");
  }
  n1->rhs = stmt();
  return node;
}

Node *handle_while()
{
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_WHILE;
  expect("(");
  node->lhs = expr();
  expect(")");
  node->rhs = stmt();
  return node;
}

Node *stmt()
{
  Node *node;

  if (consume("{"))
  { 
    node = calloc(1, sizeof(Node));
    node->kind = ND_BLOCK;

    int i = 0;
    size_t max = 2;
    node->stmts = calloc(max, sizeof(Node*));
    while (!consume("}"))
    {
      node->stmts[i++] = stmt();
      if (i + 1 == max)
      {
        node->stmts = realloc(node->stmts, max * 2 * sizeof(Node*));
        max <<= 1;
      }
    }
    node->stmts[i] = NULL;
    return node;
  }
  else if (consume_keyword(TK_RETURN))
  {
    node = calloc(1, sizeof(Node));
    node->kind = ND_RETURN;
    node->lhs = expr();
  }
  else if (consume_keyword(TK_IF))
    return handle_if();
  else if (consume_keyword(TK_FOR))
    return handle_for();
  else if (consume_keyword(TK_WHILE))
    return handle_while();
  else
    node = expr();

  if (!consume(";"))
    error_at(token->str, "';'ではないトークンです");
  return node;
}

void program()
{
  int i = 0;
  while (!at_eof())
  {
    code[i] = stmt();
    if (code[i++]->kind == ND_RETURN)
      break;
  }
  code[i] = NULL;
}