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
    Node *node = new_node(0, NULL, NULL);
    node->arg_list = arg_list("}");
    return node;
  }

  Type *type = consume_type();
  if (type)
    type = get_ptr(type);
  Token *ident = consume_ident();
  if (type)
    type = get_ar(type);
  if (ident)
  {
    Node *node = new_node(0, NULL, NULL);
    // 関数呼び出し
    if (consume("("))
      return fn_call(ident);

    node->kind = ND_LVAR;

    // 式内参照
    if (type == NULL)
    {
      LVar *lvar = find_lvar(ident, scope);
      if (lvar)
      {
        node->offset = lvar->offset;
        node->type = lvar->type;
      }
      else
        error_at(ident->str, "識別子 \"%.*s\" が定義されていません", ident->len, ident->str);
    }
    else if (type->ty == ARRAY)
      return array_assignment(type, ident);
    // 定義か宣言
    else
    {
      LVar *lvar = new_lvar(ident->str, ident->len, type);
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
  {
    Node *rhs = unary();
    if (rhs->kind == ND_DEREF)
      return rhs->rhs;
    return new_node(ND_ADDR, NULL, rhs);
  }
  else if (consume("*"))
  {
    Node *rhs = unary();
    if (rhs->kind == ND_ADDR)
      return rhs->rhs;
    return new_node(ND_DEREF, NULL, rhs);
  }
  else if (consume_keyword(TK_SIZEOF))
    return new_node_num(expr_size(unary()));
  else if (consume_keyword(TK_IF))
    return if_expr();

  consume("+");
  return maybe_array_index();
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
      node = handle_add(node);
    else if (consume("-"))
      node = handle_sub(node);
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

Node *fn(Token *ident)
{
  // 新しいスコープを準備
  enter_scope();

  Node *node = fn_params(ident);

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
    Token *maybe_type = token;
    if (!consume_type())
      error_at(maybe_type->str, "型ではありません");

    Token *ident = consume_ident();
    if (ident == NULL)
      error("名前ではありません");

    if (consume("("))
      code[i] = fn(ident);
    // else
      // code[i] = global(ident);

    if (code[i++]->kind == ND_RETURN)
      break;
  }
  code[i] = NULL;
}