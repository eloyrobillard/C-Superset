#ifndef NCC_H
#define NCC_H

#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/*
 * program    = stmt*
 * stmt       = "return"? expr ";"
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
  TK_RETURN,
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

typedef struct LVar LVar;

// ローカル変数の型
struct LVar {
  LVar *next; // 次の変数かNULL
  char *name; // 変数の名前
  int len;    // 名前の長さ
  int offset; // RBPからのオフセット
};

// 抽象構文木のノードの種類
typedef enum
{
  ND_RETURN,
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

// グローバル関数
Token *tokenize(char *);
Node *expr();
Node *stmt();
void gen(Node *);
void error(char *, ...);
void program();
bool at_eof();

// グローバル変数
Node *code[100];
Token *token;
char *usr_in;
LVar *locals;

#endif // NCC_H