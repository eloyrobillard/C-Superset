#include "9cc.h"

Node *handle_add(Node *node) {
  Node *rhs = mul();
  if (rhs->type && (rhs->type->ty == PTR || rhs->type->ty == ARRAY))
    error_at(token->str, "ポインタの右辺値を用いる加算は無効です");
  if (node->type && node->type->ty == PTR)
    return new_node(
        ND_ADD, node,
        new_node(ND_MUL, new_node_num(type_size(node->type->ptr_to)), rhs));
  else if (node->kind == ND_ADDR && node->rhs->type &&
           node->rhs->type->ty == ARRAY)
    return new_node(ND_ADD, node,
                    new_node(ND_MUL,
                             new_node_num(type_size(node->rhs->type->ptr_to)),
                             rhs));
  else
    return new_node(ND_ADD, node, rhs);
}

bool is_ptr_or_array(Node *node) {
  return (node->type && (node->type->ty == PTR || node->type->ty == ARRAY)) ||
         (node->kind == ND_ADDR && node->rhs->type &&
          node->rhs->type->ty == ARRAY);
}

Node *handle_sub(Node *node) {
  Node *rhs = mul();
  if (is_ptr_or_array(node) && is_ptr_or_array(rhs))
    return new_node(ND_DIV, new_node(ND_SUB, node, rhs),
                    new_node_num(type_size(node->type->ptr_to)));
  else if (is_ptr_or_array(node) || is_ptr_or_array(rhs))
    error_at(token->str, "ポインタと整数間の減算は無効です");
  else
    return new_node(ND_SUB, node, rhs);
}