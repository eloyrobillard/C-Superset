#ifndef AST_UTILS_H
#define AST_UTILS_H

#include "9cc.h"

typedef struct VarInfo VarInfo;

struct VarInfo {
  int offset;
  Type *type;
  NodeKind kind;
};

typedef struct MaybeExpr MaybeExpr;

struct MaybeExpr {
  bool is_expr;
  Node *node;
};

typedef struct FullType FullType;
struct FullType {
  Type *type;
  Token *ident;
};

bool at_eof();
LVar *new_lvar(char *name, int len, Type *);
GVar *new_gvar(char *name, int len, Type *);
VarInfo *find_var(Token *tok, Scope *);
LVar *find_lvar(Token *tok, Scope *);
GVar *find_gvar(Token *tok);
Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Node *new_if_node(Node *cond, Node *ifstmt, Node *els);
GlobalScope *create_global_scope();
Scope *create_scope();
Scope *enter_scope();
Scope *exit_scope();
bool consume(char *op);
bool consume_keyword(TK_KIND type);
Type *get_ar(Type *type);
Type *get_ptr(Type *type);
FullType *get_full_type();
Type *consume_type();
Token *consume_ident();
void expect(char *op);
int expect_num();
void expect_keyword(TK_KIND, const char*);
// arithmetic.c
Node *handle_add(Node *node);
Node *handle_sub(Node *node);
// grammars.c
Node *block();
Node *final_block();
MaybeExpr *try_expr();
Node *if_expr();
Node *if_stmt();
ArgList *arg_list(char *terminator);
Node *handle_for();
Node *handle_while();
Node *fn_call(Token *ident);
Node *array_assignment(Type *type, Token *ident);
Node *maybe_array_index();
Node *fn_params(Token *ident);
Node *decl();

#endif // AST_UTILS_H