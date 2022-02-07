#include "ast_utils.h"

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

Node *array_assignment(Type *type, Token *ident)
{
  Node *node = new_node(ND_ARR, NULL, NULL);
  if (consume("="))
  {
    Node *maybe_arglist = primary();
    if (maybe_arglist->arg_list)
      node->arg_list = maybe_arglist->arg_list;
    else
      error_at(token->str, "配列の初期値ではありません");
  }
  LVar *lvar = new_lvar(ident->str, ident->len, type);
  node->offset = lvar->offset;
  node->type = type;
  if (type->array_size == 0)
    node->type->array_size = node->arg_list->argc;
  return node;
}

Node *fn_call(Token *ident)
{
  Node *node = new_node(ND_FNCALL, NULL, NULL);
  node->arg_list = arg_list(")");
  node->arg_list->str = ident->str;
  node->arg_list->len = ident->len;
  return node;
}

Node *maybe_array_index()
{
  Node *prim = primary();
  // 配列要素の参照
  if (consume("["))
  {
    Node *place = new_node_num(0);
    do
    {
      place = new_node(
          ND_ADD, new_node(ND_MUL, new_node_num(10), place), expr());
      expect("]");
    } while (consume("["));
    // *(p + k)
    prim = new_node(ND_DEREF, NULL, new_node(ND_ADD, prim, new_node(ND_MUL, new_node_num(type_size(prim->type->elem_type)), place)));
  }
  return prim;
}

Node *fn_params(Token *ident)
{
  expect("(");
  Node *node = new_node(ND_FNDEF, NULL, NULL);
  node->def = calloc(1, sizeof(FnDef));
  node->def->name = ident->str;
  node->def->len = ident->len;

  int i = 0;
  if (!consume(")"))
  {
    int max = 2;
    node->def->params = calloc(max, sizeof(Node *));
    do
    {
      Token *tok = token;
      Type *type = consume_type();
      if (type == NULL)
        error_at(tok->str, "型ではありません");

      Node *param = decl(type);

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
    error("関数に中身は必要");

  return node;
}

Node *decl(Type *type)
{
  Token *tok = consume_ident();
  if (tok)
  {
    Node *node = new_node(ND_LVAR, NULL, NULL);
    LVar *lvar = new_lvar(tok->str, tok->len, type);
    node->offset = lvar->offset;
    node->type = type;
    return node;
  }
  else
    error("名前ではありません");
}