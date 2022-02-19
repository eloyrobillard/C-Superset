#include "ast_utils.h"

bool at_eof() { return token->type == TK_EOF; }

GVar *new_gvar(char *name, int len, Type *type) {
  GVar *gvar = calloc(1, sizeof(GVar));
  gvar->next = global_scope->globals;
  gvar->name = name;
  gvar->len = len;
  gvar->type = type;

  global_scope->globals = gvar;
  global_scope->globalc++;
  return gvar;
}

LVar *new_lvar(char *name, int len, Type *type) {
  LVar *lvar = calloc(1, sizeof(LVar));
  lvar->next = scope->locals;
  lvar->name = name;
  lvar->len = len;
  lvar->type = type;

  lvar->offset = (scope->locals ? (scope->locals->type->ty == ARRAY
                                       ? type_size(scope->locals->type) + 8
                                       : 8) +
                                      scope->locals->offset
                                : 8);

  scope->locals = lvar;
  scope->localc++;
  return lvar;
}

VarInfo *find_var(Token *tok, Scope *scope) {
  VarInfo *var_info = calloc(1, sizeof(VarInfo));
  LVar *lvar = find_lvar(tok, scope);
  if (lvar == NULL) {
    GVar *gvar = find_gvar(tok);
    if (gvar) {
      var_info->type = gvar->type;
      var_info->kind = ND_GVARREF;
    }
  } else {
    var_info->offset = lvar->offset;
    var_info->type = lvar->type;
    var_info->kind = ND_LVAR;
  }
  return var_info;
}

GVar *find_gvar(Token *tok) {
  for (GVar *var = global_scope->globals; var; var = var->next)
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
      return var;
  return NULL;
}

LVar *find_lvar(Token *tok, Scope *scope) {
  for (LVar *var = scope->locals; var; var = var->next)
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
      return var;
  return NULL;
}

GlobalScope *create_global_scope() { return calloc(1, sizeof(GlobalScope)); }

Scope *create_scope() {
  Scope *inner_scope = calloc(1, sizeof(Scope));
  inner_scope->parent = scope;
  // 内スコープは外スコープの変数を含む
  if (inner_scope->parent)
    inner_scope->locals = scope->locals;
  inner_scope->childm = 2;
  inner_scope->children = calloc(2, sizeof(Scope *));
  return inner_scope;
}

Scope *enter_scope() {
  Scope *inner_scope = create_scope();
  if (scope) {
    if (scope->childc + 1 >= scope->childm)
      scope->children =
          realloc(scope->children, sizeof(Scope *) * (scope->childm *= 2));
    scope->children[scope->childc++] = inner_scope;
  }
  scope = inner_scope;
  return scope;
}

Scope *exit_scope() { return (scope = scope->parent); }

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  node->scope = scope;
  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  node->scope = scope;
  return node;
}

bool consume(char *op) {
  Token *tok = token;
  if (tok->type != TK_RESERVED || strlen(op) != tok->len ||
      memcmp(tok->str, op, tok->len))
    return false;

  token = token->next;
  return true;
}

bool consume_keyword(TK_KIND type) {
  if (token->type != type)
    return false;

  token = token->next;
  return true;
}

Type *get_ar(Type *type) {
  if (consume("[")) {
    Type *rec_ty = calloc(1, sizeof(Type));
    rec_ty->ty = ARRAY;
    rec_ty->ptr_to = type;
    if (!consume("]")) {
      rec_ty->array_size = expect_num();
      expect("]");
    }
    Type *final_ty = get_ar(rec_ty);
    final_ty->elem_type = type;
    return final_ty;
  }
  return type;
}

Type *get_ptr(Type *type) {
  if (consume("*")) {
    Type *rec_ty = calloc(1, sizeof(Type));
    rec_ty->ty = PTR;
    rec_ty->ptr_to = type;
    return get_ptr(rec_ty);
  }
  return type;
}

FullType *get_full_type() {
  Type *type = consume_type();
  if (type)
    type = get_ptr(type);
  Token *ident = consume_ident();
  if (type)
    type = get_ar(type);
  FullType *full_type = calloc(1, sizeof(FullType));
  full_type->type = type;
  full_type->ident = ident;
  return full_type;
}

Type *consume_type() {
  Type *type = calloc(1, sizeof(Type));
  switch (token->type) {
  case TK_I32: {
    type->ty = I32;
    break;
  }
  case TK_I64: {
    type->ty = I64;
    break;
  }
  default:
    return NULL;
  }

  token = token->next;
  return type;
}

Token *consume_ident() {
  Token *tok = token;
  if (tok->type != TK_IDENT)
    return NULL;

  token = token->next;
  return tok;
}

void expect(char *op) {
  if (token->type != TK_RESERVED || strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
    error_at(token->str, "'%c'ではありません\n", op);
  token = token->next;
}

int expect_num() {
  if (token->type != TK_NUM)
    error_at(token->str, "数ではありません\n");
  int val = token->val;
  token = token->next;
  return val;
}

void expect_keyword(TK_KIND type, const char *fmt) {
  if (token->type != type)
    error_at(token->str, fmt);

  token = token->next;
}
