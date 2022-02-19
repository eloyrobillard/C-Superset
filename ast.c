#include "9cc.h"
#include "ast_utils.h"

Node *primary() {
  // 次のトークンが"("なら、"(" expr ")"のはず
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }
  // 配列初期値
  else if (consume("{")) {
    Node *node = new_node(0, NULL, NULL);
    node->arg_list = arg_list("}");
    return node;
  }

  FullType *full_type = get_full_type();
  Type *type = full_type->type;
  Token *ident = full_type->ident;
  if (ident) {
    Node *node = new_node(0, NULL, NULL);
    // 関数呼び出し
    if (consume("("))
      return fn_call(ident);

    // 式内参照
    if (type == NULL) {
      VarInfo *var_info = find_var(ident, scope);
      if (var_info->type) {
        node->offset = var_info->offset;
        node->type = var_info->type;
        node->kind = var_info->kind;
        if (var_info->kind == ND_GVARREF) {
          node->ident = ident->str;
          node->len = ident->len;
        }
      } else
        error_at(ident->str, "識別子 \"%.*s\" が定義されていません", ident->len,
                 ident->str);
    } else if (type->ty == ARRAY)
      return array_assignment(type, ident);
    // 定義か宣言
    else {
      LVar *lvar = new_lvar(ident->str, ident->len, type);
      node->offset = lvar->offset;
      node->type = type;
      node->kind = ND_LVAR;
    }

    return node;
  } else if (type) {
    Node *node = new_node(ND_TYPETK, NULL, NULL);
    node->type = type;
    return node;
  }

  // そうでなければ数値のはず
  return new_node_num(expect_num());
}

Node *unary() {
  if (consume("-"))
    return new_node(ND_SUB, new_node_num(0), primary());
  else if (consume("&")) {
    Node *rhs = unary();
    if (rhs->kind == ND_DEREF)
      return rhs->rhs;
    return new_node(ND_ADDR, NULL, rhs);
  } else if (consume("*")) {
    Node *rhs = unary();
    if (rhs->kind == ND_ADDR)
      return rhs->rhs;
    return new_node(ND_DEREF, NULL, rhs);
  } else if (consume_keyword(TK_SIZEOF))
    return new_node_num(expr_size(unary()));
  else if (consume_keyword(TK_IF))
    return if_expr();

  consume("+");
  return maybe_array_index();
}

Node *mul() {
  Node *node = unary();

  for (;;) {
    if (consume("*"))
      node = new_node(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_node(ND_DIV, node, unary());
    else
      return node;
  }
}

Node *add() {
  Node *node = mul();

  for (;;) {
    if (consume("+"))
      node = handle_add(node);
    else if (consume("-"))
      node = handle_sub(node);
    else
      return node;
  }
}

Node *relational() {
  Node *node = add();

  for (;;) {
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

Node *equality() {
  Node *node = relational();

  for (;;) {
    if (consume("=="))
      node = new_node(ND_EQ, node, relational());
    else if (consume("!="))
      node = new_node(ND_NEQ, node, relational());
    else
      return node;
  }
}

Node *assign() {
  Node *node = equality();
  if (consume("="))
    node = new_node(ND_ASSIGN, node, assign());
  return node;
}

Node *expr() { return assign(); }

Node *stmt() {
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

Node *fn(Type *type, Token *ident) {
  // 新しいスコープを準備
  enter_scope();

  Node *node = fn_params(ident);
  node->type = type;

  exit_scope();
  return node;
}

Node *global(Type *type, Token *ident) {
  Node *node = new_node(ND_GVAR, NULL, NULL);

  GVar *gvar = new_gvar(ident->str, ident->len, type);
  node->type = type;
  node->ident = ident->str;
  node->len = ident->len;

  if (consume("="))
    node = new_node(ND_ASSIGN, node, assign());
  if (!consume(";"))
    error_at(token->str, "';'ではないトークンです");
  return node;
}

void program() {
  int i = 0;
  global_scope = create_global_scope();
  while (!at_eof()) {
    Token *maybe_type = token;
    FullType *full_type = get_full_type();
    Type *type = full_type->type;
    Token *ident = full_type->ident;
    if (type == NULL)
      error(maybe_type->str, "型ではありません");

    if (ident == NULL)
      error_at(maybe_type->next->str, "名前ではありません");

    if (consume("("))
      code[i] = fn(type, ident);
    else
      code[i] = global(type, ident);

    if (code[i++]->kind == ND_RETURN)
      break;
  }
  code[i] = NULL;
}