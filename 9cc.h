#ifndef NCC_H
#define NCC_H

/*
* program    = stmt*
* stmt       = expr ";"
* expr       = assign
* assign     = equality ("=" assign)?
* equality   = relational ("==" relational | "!=" relational)*
* relational = add ("<" add | "<=" add | ">" add | ">=" add)*
* add        = mul ("+" mul | "-" mul)*
* mul        = unary ("*" unary | "/" unary)*
* unary      = ("+" | "-")? primary
* primary    = num | ident | "(" expr ")"
 */

typedef enum TK_Type
{
  TK_RESERVED,
  TK_IDENT, // 識別子
  TK_NUM,
  TK_EOF
} TK_TYPE;

typedef struct Token Token;
struct Token
{
  TK_TYPE type;
  struct Token *next;
  long val;  // for int tokens
  char *str; // for all tokens
  int len;
};

Token *token;
char *usr_in;

// 抽象構文木のノードの種類
typedef enum
{
  ND_ASSIGN,
  ND_LVAR,
  ND_EQ,
  ND_NEQ,
  ND_LESS,
  ND_LEQ,
  ND_ADD, //* +
  ND_SUB, //* -
  ND_MUL, //* *
  ND_DIV, //* /
  ND_NUM, //* 整数
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node
{
  NodeKind kind; //* ノードの型
  Node *lhs;     //! 左辺
  Node *rhs;     //? 右辺
  int val;       //! kindがND_NUMの場合のみ使う
  int offset;    //* 変数の場合
};

#endif // NCC_H