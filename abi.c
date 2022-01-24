#include "9cc.h"

int size_of(Type *type)
{
  switch (type->ty)
  {
  case PTR:
  case I64:
    return 8;
  case I32:
    return 4;
  }
}