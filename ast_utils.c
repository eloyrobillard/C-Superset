#include "ast_utils.h"

bool at_eof()
{
  return get_token()->type == TK_EOF;
}

LVar *new_lvar(char *name, int len, Type *type)
{
  LVar *lvar = calloc(1, sizeof(LVar));
  lvar->next = scope->locals;
  lvar->name = name;
  lvar->len = len;
  lvar->type = type;
  lvar->offset = 8 + (scope->locals ? scope->locals->offset : 0);

  scope->locals = lvar;
  scope->localc++;
  return lvar;
}

LVar *find_lvar(Token *tok, Scope *scope)
{
  for (LVar *var = scope->locals; var; var = var->next)
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
      return var;
  return NULL;
}

Scope *create_scope()
{
  Scope *inner_scope = calloc(1, sizeof(Scope));
  inner_scope->parent = scope;
  // 内スコープは外スコープの変数を含む
  if (inner_scope->parent)
    inner_scope->locals = scope->locals;
  inner_scope->childm = 2;
  inner_scope->children = calloc(2, sizeof(Scope *));
  return inner_scope;
}

Scope *enter_scope()
{
  Scope *inner_scope = create_scope();
  if (scope->childc + 1 >= scope->childm)
    scope->children = realloc(scope->children, sizeof(Scope *) * (scope->childm *= 2));
  scope->children[scope->childc++] = inner_scope;
  scope = inner_scope;
  return scope;
}

Scope *exit_scope()
{
  if (scope->parent == NULL)
    error("グローバルスコープからの脱出は無効です");
  return (scope = scope->parent);
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

Type *get_ptr(Type *type)
{
  if (consume("*"))
  {
    Type *final_ty = calloc(1, sizeof(Type));
    final_ty->ty = PTR;
    final_ty->ptr_to = type;
    return get_ptr(final_ty);
  }
  return type;
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
      Node *arg = expr();
      node->call->args[i++] = arg;
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
