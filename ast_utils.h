#ifndef AST_UTILS_H
#define AST_UTILS_H

#include "9cc.h"

bool at_eof();
Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Node *new_if_node(Node *cond, Node *ifstmt, Node *els);
Scope *create_scope();
Scope *enter_scope();
Scope *exit_scope();
bool consume(char *op);
bool consume_keyword(TK_KIND type);
Type *consume_type();
Token *consume_ident();
void expect(char *op);
int expect_num();
Node *handle_fncall(Node *node, Token *tok);
Node *handle_if();
Node *handle_for();
Node *handle_while();

#endif // AST_UTILS_H