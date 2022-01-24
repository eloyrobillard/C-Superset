#include "9cc.h"
#include "ast_utils.h"

Node *primary()
{
  // 次のトークンが"("なら、"(" expr ")"のはず
  if (consume("("))
  {
    Node *node = expr();
    expect(")");
    return node;
  }

  Type *type = get_ptr(consume_type());
  Token *tok = consume_ident();
  if (tok)
  {
    Node *node = calloc(1, sizeof(Node));
    if (consume("("))
      return handle_fncall(node, tok);
    node->kind = ND_LVAR;

    if (type == NULL)
    {
      LVar *lvar = find_lvar(tok, scope);
      if (lvar)
      {
        node->offset = lvar->offset;
        node->type = lvar->type;
      }
      else
        error_at(tok->str, "識別子 \"%.*s\" が定義されていません", tok->len, tok->str);
    }
    else
    {
      LVar *lvar = new_lvar(tok->str, tok->len, type);
      node->offset = lvar->offset;
      node->type = type;
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
  else if (consume("&"))
    return new_node(ND_ADDR, NULL, unary());
  else if (consume("*"))
    return new_node(ND_DEREF, NULL, unary());
  else if (consume_keyword(TK_SIZEOF))
    return new_node_num(expr_size(unary()));

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
    {
      Node *rhs = mul();
      if (rhs->type && rhs->type->ty == PTR)
        error_at(get_token()->str, "ポインタの右辺値を用いる加算は無効です");
      if (node->type && node->type->ty == PTR)
        node = new_node(ND_ADD, node, new_node(ND_MUL, new_node_num(size_of(node->type->ptr_to)), rhs));
      else
        node = new_node(ND_ADD, node, rhs);
    }
    else if (consume("-"))
    {
      Node *rhs = mul();
      if (node->type && node->type->ty == PTR && rhs->type && rhs->type->ty == PTR)
        node = new_node(ND_DIV, new_node(ND_SUB, node, rhs), new_node_num(size_of(node->type->ptr_to)));
      else if ((node->type && node->type->ty == PTR) || (rhs->type && rhs->type->ty == PTR))
        error_at(get_token()->str, "ポインタと整数間の減算は無効です");
      else
        node = new_node(ND_SUB, node, rhs);
    }
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

Node *decl(Type *type)
{
  Token *tok = consume_ident();
  if (tok)
  {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;
    LVar *lvar = new_lvar(tok->str, tok->len, type);
    node->offset = lvar->offset;
    node->type = type;
    return node;
  }
  else
    error("名前ではありません");
}

Node *expr()
{

  return assign();
}

Node *stmt()
{
  Node *node;

  if (consume("{"))
  {
    enter_scope();

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
    exit_scope();
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

  // 新しいスコープを準備
  enter_scope();

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

  exit_scope();
  return node;
}

void program()
{
  int i = 0;
  // グローバルスコープを作成
  scope = create_scope();
  while (!at_eof())
  {
    code[i] = fn();
    if (code[i++]->kind == ND_RETURN)
      break;
  }
  code[i] = NULL;
}