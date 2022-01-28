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
  // 配列初期値
  else if (consume("{"))
  {
    Node *node = new_node(ND_ARRLIT, NULL, NULL);
    node->arg_list = arg_list();
    return node;
  }

  Type *type = consume_type();
  if (type)
    type = get_ptr(type);
  Token *tok = consume_ident();
  if (type)
    type = get_ar(type);
  if (tok)
  {
    Node *node = new_node(0, NULL, NULL);
    if (consume("("))
      return handle_fncall(node, tok);
    node->kind = ND_LVAR;

    // 式内参照
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
    // 定義か宣言
    else
    {
      LVar *lvar = new_lvar(tok->str, tok->len, type);
      node->offset = lvar->offset;
      node->type = type;
    }

    return node;
  }
  else if (type)
  {
    Node *node = new_node(ND_TYPETK, NULL, NULL);
    node->type = type;
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
  else if (consume_keyword(TK_IF))
    return if_expr();

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
        error_at(token->str, "ポインタの右辺値を用いる加算は無効です");
      if (node->type && node->type->ty == PTR)
        node = new_node(ND_ADD, node, new_node(ND_MUL, new_node_num(type_size(node->type->ptr_to)), rhs));
      else
        node = new_node(ND_ADD, node, rhs);
    }
    else if (consume("-"))
    {
      Node *rhs = mul();
      if (node->type && node->type->ty == PTR && rhs->type && rhs->type->ty == PTR)
        node = new_node(ND_DIV, new_node(ND_SUB, node, rhs), new_node_num(type_size(node->type->ptr_to)));
      else if ((node->type && node->type->ty == PTR) || (rhs->type && rhs->type->ty == PTR))
        error_at(token->str, "ポインタと整数間の減算は無効です");
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
    Node *node = new_node(ND_LVAR, NULL, NULL);
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
  if (consume("{"))
    return block();
  else if (consume_keyword(TK_FOR))
    return handle_for();
  else if (consume_keyword(TK_WHILE))
    return handle_while();
  else if (consume_keyword(TK_IF))
    return if_stmt();

  Node *node;

  if (consume_keyword(TK_RETURN))
    node = new_node(ND_RETURN, expr(), NULL);
  else
    node = expr();

  if (!consume(";"))
    error_at(token->str, "';'ではないトークンです");
  return node;
}

Node *fn()
{
  Token *maybe_kata = token;
  if (!consume_type())
    error_at(maybe_kata->str, "型ではありません");

  Token *tok = consume_ident();
  if (tok == NULL)
    error("名前ではありません");

  // 新しいスコープを準備
  enter_scope();

  expect("(");
  Node *node = new_node(ND_FNDEF, NULL, NULL);
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