#include <math.h>
#include "9cc.h"

int type_size(Type *type)
{
  switch (type->ty)
  {
    case ARRAY:
      return type->array_size * type_size(type->ptr_to);
    case PTR:
    case I64:
      return 8;
    case I32:
      return 8;
  }
}

int expr_size(Node *node)
{
  if (node->kind == ND_NUM)
  {
    long i_lim = pow(2, 31);
    if (node->val > i_lim - 1 || node->val < -i_lim)
      return 8;
    return 4;
  }
  else if (node->kind == ND_LVAR || node->kind == ND_TYPETK)
    return type_size(node->type);

  int lsize = expr_size(node->lhs);
  int rsize = expr_size(node->rhs);
  return lsize > rsize ? lsize : rsize;
}