#include "9cc.h"

bool at_eof()
{
  return get_token()->type == TK_EOF;
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
  Token *tok = get_token();
  if (tok->type != TK_RESERVED || strlen(op) != tok->len || memcmp(tok->str, op, tok->len))
    return false;

  next_token();
  return true;
}

bool consume_keyword(TK_KIND type)
{
  if (get_token()->type != type)
    return false;

  next_token();
  return true;
}

Type *consume_type()
{
  Token *tok = get_token();
  Type *type = calloc(1, sizeof(Type));
  switch (tok->len)
  {
  case 3:
  {
    if (memcmp(tok->str, "i64", tok->len) == 0)
      type->ty = I64;
    else if (memcmp(tok->str, "i32", tok->len) == 0 || memcmp(tok->str, "int", tok->len) == 0)
      type->ty = I32;
    else
      return NULL;
    break;
  }
  case 4:
  {
    if (memcmp(tok->str, "long", tok->len) == 0)
      type->ty = I64;
    else
      return NULL;
    break;
  }
  default:
    return NULL;
  }

  next_token();
  return type;
}

Token *consume_ident()
{
  Token *tok = get_token();
  if (tok->type != TK_IDENT)
    return NULL;

  next_token();
  return tok;
}

void expect(char *op)
{
  if (get_token()->type != TK_RESERVED || strlen(op) != get_token()->len || memcmp(get_token()->str, op, get_token()->len))
    error_at(get_token()->str, "'%c'ではありません\n", op);
  next_token();
}

int expect_num()
{
  if (get_token()->type != TK_NUM)
    error_at(get_token()->str, "数ではありません\n");
  int val = get_token()->val;
  next_token();
  return val;
}

Node *handle_fncall(Node *node, Token *tok)
{
  node->kind = ND_FNCALL;
  node->call = calloc(1, sizeof(FnCall));
  node->call->str = tok->str;
  node->call->len = tok->len;
  int i = 0;
  if (!consume(")"))
  {
    int max = 2;
    node->call->args = calloc(max, sizeof(Node *));
    do
    {
      node->call->args[i++] = expr();
      if (i + 1 == max)
      {
        node->call->args = realloc(node->call->args, (max *= 2) * sizeof(Node *));
      }
    } while (consume(","));
    expect(")");
  }
  node->call->argc = i;
  return node;
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
      return handle_fncall(node, tok);
    node->kind = ND_LVAR;

    LVar *lvar = find_lvar(tok);
    if (lvar)
      node->offset = lvar->offset;
    else
      error_at(tok->str, "識別子 \"%.*s\" が定義されていません", tok->len, tok->str);

    return node;
  }

  // そうでなければ数値のはず
  return new_node_num(expect_num());
}

Node *unary()
{
  if (consume("-"))
    return new_node(ND_SUB, new_node_num(0), primary());
  else if (consume("&"))
    return new_node(ND_ADDR, NULL, unary());
  else if (consume("*"))
    return new_node(ND_DEREF, NULL, unary());

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

Type *get_ptr(Type *type)
{
  Type *final_ty = calloc(1, sizeof(Type));
  while (consume("*"))
  {
    final_ty->ty = PTR;
    final_ty->ptr_to = type;
    type = final_ty;
  }
  return type;
}

Node *decl(Type *type)
{
  Token *tok = consume_ident();
  if (tok)
  {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;
    LVar *lvar = find_lvar(tok);
    if (lvar)
      error_at(tok->str, "\"%.*s\" の再定義ができません", tok->len, tok->str);
    else
    {
      LVar *lvar = new_lvar(tok->str, tok->len, type);
      node->offset = lvar->offset;
      node->type = type;
      locals = lvar;
    }
    return node;
  }
  else
    error("名前ではありません");
}

Node *expr()
{
  Type *type = consume_type();
  if (type)
    return decl(get_ptr(type));

  return assign();
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
    node->stmts = calloc(max, sizeof(Node *));
    while (!consume("}"))
    {
      node->stmts[i++] = stmt();
      if (i + 1 == max)
      {
        node->stmts = realloc(node->stmts, max * 2 * sizeof(Node *));
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
    error_at(get_token()->str, "';'ではないトークンです");
  return node;
}

Node *fn()
{
  Token *maybe_kata = get_token();
  if (!consume_type())
    error_at(maybe_kata->str, "型ではありません");

  Token *tok = consume_ident();
  if (tok == NULL)
    error("名前ではありません");

  expect("(");
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_FNDEF;
  node->def = calloc(1, sizeof(FnDef));
  node->def->name = tok->str;
  node->def->len = tok->len;

  int i = 0;
  if (!consume(")"))
  {
    int max = 2;
    node->def->params = calloc(max, sizeof(Node *));
    do
    {
      Token *tok = get_token();
      Type *type = consume_type();
      if (type == NULL)
        error_at(tok->str, "型ではありません");

      Node *param = decl(type);

      // TODO handle shadowing

      node->def->params[i++] = param;
      if (i + 1 == max)
      {
        node->def->params = realloc(node->def->params, (max *= 2) * sizeof(Node *));
      }
    } while (consume(","));
    expect(")");
  }
  node->def->paramc = i;

  node->def->body = stmt();

  if (node->def->body->kind != ND_BLOCK)
    error("関数に中身はない");

  return node;
}

void program()
{
  int i = 0;
  while (!at_eof())
  {
    code[i] = fn();
    if (code[i++]->kind == ND_RETURN)
      break;
  }
  code[i] = NULL;
}