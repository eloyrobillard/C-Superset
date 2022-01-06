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
 * stmt       = expr ";"
 *            | "{" stmt* "}"
 *            | "return" expr ";"
 *            | "if" "(" expr ")" stmt ("else" stmt)?
 *            | "for" "(" expr? ";" expr? ";" expr? ")" stmt
 *            | "while" "(" expr ")" stmt
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
  // 指定されたキーワード
  TK_RETURN,
  TK_IF,
  TK_ELSE,
  TK_FOR,
  TK_WHILE,

  TK_RESERVED,
  TK_IDENT, // 識別子
  TK_NUM,
  TK_EOF
} TK_TYPE;

typedef enum
{
  // 指定されたキーワード
  ND_RETURN = 1,
  ND_IF,
  ND_FOR,
  ND_WHILE,

  ND_ASSIGN,
  ND_LVAR,
  ND_BLOCK,

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
typedef struct Node Node;

// 抽象構文木のノードの型
struct Node
{
  NodeKind kind; //* ノードの型
  Node *lhs;     //! 左辺
  Node *rhs;     //? 右辺
  int val;       //! kindがND_NUMの場合のみ使う
  int offset;    //* 変数の場合
  Node **stmts;
};

// グローバル関数
// tokenizer.c
Token *tokenize(char *);
LVar *new_lvar(char *name, int len);
LVar *find_lvar(Token *tok);
// ast.c
bool at_eof();
Node *expr();
Node *stmt();
void gen(Node *);
void program();
// error.c
void error(char *, ...);
void error_at(char *loc, const char *fmt, ...);

// グローバル変数
Node *code[100];
Token *token;
char *usr_in;
LVar *locals;

#endif // NCC_H