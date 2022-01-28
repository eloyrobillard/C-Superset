#include "ast_utils.h"

bool at_eof()
{
  return token->type == TK_EOF;
}

LVar *new_lvar(char *name, int len, Type *type)
{
  LVar *lvar = calloc(1, sizeof(LVar));
  lvar->next = scope->locals;
  lvar->name = name;
  lvar->len = len;
  lvar->type = type;

  lvar->offset = (scope->locals ? /* type_size(scope->locals->type) */ 8 + scope->locals->offset : 8);

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
  node->scope = scope;
  return node;
}

Node *new_node_num(int val)
{
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  node->scope = scope;
  return node;
}

bool consume(char *op)
{
  Token *tok = token;
  if (tok->type != TK_RESERVED || strlen(op) != tok->len || memcmp(tok->str, op, tok->len))
    return false;

  token = token->next;
  return true;
}

bool consume_keyword(TK_KIND type)
{
  if (token->type != type)
    return false;

  token = token->next;
  return true;
}

Type *get_ar(Type *type)
{
  if (consume("["))
  {
    Type *final_ty = calloc(1, sizeof(Type));
    final_ty->ty = ARRAY;
    final_ty->ptr_to = type;
    final_ty->array_size = expect_num();
    expect("]");
    return get_ar(final_ty);
  }
  return type;
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
  Type *type = calloc(1, sizeof(Type));
  switch (token->type)
  {
  case TK_I32:
  {
    type->ty = I32;
    break;
  }
  case TK_I64:
  {
    type->ty = I64;
    break;
  }
  default:
    return NULL;
  }

  token = token->next;
  return type;
}

Token *consume_ident()
{
  Token *tok = token;
  if (tok->type != TK_IDENT)
    return NULL;

  token = token->next;
  return tok;
}

void expect(char *op)
{
  if (token->type != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
    error_at(token->str, "'%c'ではありません\n", op);
  token = token->next;
}

int expect_num()
{
  if (token->type != TK_NUM)
    error_at(token->str, "数ではありません\n");
  int val = token->val;
  token = token->next;
  return val;
}

void expect_keyword(TK_KIND type, const char *fmt)
{
  if (token->type != type)
    error_at(token->str, fmt);

  token = token->next;
}

ArgList *arg_list(char *terminator)
{
  ArgList *arg_list = calloc(1, sizeof(ArgList));
  int i = 0;
  if (!consume(terminator))
  {
    int max = 2;
    arg_list->args = calloc(max, sizeof(Node *));
    do
    {
      Node *arg = expr();
      arg_list->args[i++] = arg;
      if (i + 1 == max)
      {
        arg_list->args = realloc(arg_list->args, (max *= 2) * sizeof(Node *));
      }
    } while (consume(","));
    expect(terminator);
  }
  arg_list->argc = i;
  return arg_list;
}

Node *handle_fncall(Node *node, Token *tok)
{
  node->kind = new_node(ND_FNCALL, NULL, NULL);
  node->arg_list = arg_list(")");
  return node;
}

Node *block()
{
  enter_scope();
  Node *node = calloc(1, sizeof(Node));
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
  exit_scope();
  return node;
}

Node *final_block()
{
  if (consume("{"))
  {
    enter_scope();
    Node *node = new_node(ND_FINBLOCK, NULL, NULL);

    int i = 0;
    size_t max = 2;
    node->stmts = calloc(max, sizeof(Node *));

    // 表現ブロックのデバッグ用
    MaybeExpr *maybe_expr;
    bool is_final = false;
    Token *tok; // １つ前のトークンを指参照
    while (!consume("}"))
    {
      maybe_expr = try_expr();
      node->stmts[i++] = maybe_expr->node;
      tok = token;
      if (maybe_expr->is_expr && !consume(";"))
      {
        is_final = true;
        expect("}");
        break;
      }
      else if (i + 1 == max)
      {
        node->stmts = realloc(node->stmts, max * 2 * sizeof(Node *));
        max <<= 1;
      }
    }
    if (!is_final)
      error_at(tok->str, "戻し表現にセミコロンをつけてはいけません");
    node->stmts[i] = NULL;
    exit_scope();
    return node;
  }
  return expr();
}

MaybeExpr *try_expr()
{
  MaybeExpr *maybe_expr = calloc(1, sizeof(MaybeExpr));
  if (consume("{"))
  {
    maybe_expr->node = block();
    return maybe_expr;
  }
  else if (consume_keyword(TK_FOR))
  {
    maybe_expr->node = handle_for();
    return maybe_expr;
  }
  else if (consume_keyword(TK_WHILE))
  {
    maybe_expr->node = handle_while();
    return maybe_expr;
  }
  else if (consume_keyword(TK_IF))
  {
    maybe_expr->node = if_stmt();
    return maybe_expr;
  }

  if (consume_keyword(TK_RETURN))
  {
    maybe_expr->node = new_node(ND_RETURN, expr(), NULL);
    if (!consume(";"))
      error_at(token->str, "';'ではないトークンです");
  }
  else
  {
    maybe_expr->is_expr = true;
    maybe_expr->node = expr();
  }

  return maybe_expr;
}

Node *if_stmt()
{
  Node *node = new_node(ND_IF, NULL, NULL);
  expect("(");
  Node *if_node = new_node(0, expr(), NULL);
  expect(")");
  if_node->rhs = stmt();
  node->lhs = if_node;

  if (consume_keyword(TK_ELSE))
    node->rhs = stmt();
  return node;
}

Node *if_expr()
{
  Node *node = new_node(ND_IF, NULL, NULL);
  expect("(");
  Node *if_node = new_node(0, expr(), NULL);
  expect(")");
  if_node->rhs = final_block();
  node->lhs = if_node;

  expect_keyword(TK_ELSE, "else条件は必須です");
  node->rhs = final_block();
  return node;
}

Node *handle_for()
{
  Node *node = new_node(ND_FOR, NULL, NULL);
  Node *n0 = new_node(0, NULL, new_node_num(1));
  node->lhs = n0;
  Node *n1 = new_node(0, NULL, NULL);
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
  Node *node = new_node(ND_WHILE, NULL, NULL);
  expect("(");
  node->lhs = expr();
  expect(")");
  node->rhs = stmt();
  return node;
}
