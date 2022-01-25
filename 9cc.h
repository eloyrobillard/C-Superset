#ifndef NINE_CC_H
#define NINE_CC_H

#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/*
 * program    = fn*
 * fn         = TYPE ident "(" (ident ("," ident)*)* ")" BLOCK
 * stmt       = expr ";"
 *            | BLOCK
 *            | "return" expr ";"
 *            | "if" "(" expr ")" stmt ("else" stmt)?
 *            | "for" "(" expr? ";" expr? ";" expr? ")" stmt
 *            | "while" "(" expr ")" stmt
 * expr       = assign
 * assign     = equality
 *            | equality "=" assign
 * equality   = relational ("==" relational | "!=" relational)*
 * relational = add ("<" add | "<=" add | ">" add | ">=" add)*
 * add        = mul ("+" mul | "-" mul)*
 * mul        = unary ("*" unary | "/" unary)*
 * unary      = "sizeof" unary
 *            | "if" "(" expr ")" (FINAL_BLOCK / expr) "else" (FINAL_BLOCK / expr)
 *            | ("+" | "-")? primary
 *            | "&" unary
 *            | "*" unary
 *            | unary 
 * primary    = num
 *            | "{" expr ("," expr)* "}"
 *            | TYPE
 *            | TYPE? ident ("[" num "]")*
 *            | ident ("(" (expr ("," expr)*)? ")")?
 *            | "(" expr ")"
 *
 * BLOCK      = "{" stmt* "}"
 * FINAL_BLOCK = "{" stmt* expr "}"
 * TYPE       = "i64" / "long" / "i32" / "int"
 */

typedef enum TK_Kind
{
  // 型名
  TK_I32 = 1, TK_I64,

  // 指定されたキーワード
  TK_SIZEOF,
  TK_RETURN,
  TK_IF,
  TK_ELSE,
  TK_FOR,
  TK_WHILE,
  TK_TYPE,

  TK_RESERVED,
  TK_IDENT, // 識別子
  TK_NUM,
  TK_EOF
} TK_KIND;

typedef enum NodeKind
{
  // 指定されたキーワード
  ND_RETURN = 1,
  ND_IF,
  ND_FOR,
  ND_WHILE,

  ND_TYPETK,
  ND_LVAR,
  ND_ASSIGN,
  ND_FNCALL,
  ND_FNDEF,
  ND_FINBLOCK,
  ND_BLOCK,

  ND_ADDR, // 参照（&）
  ND_DEREF,

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
  TK_KIND type;
  long val;  // for int tokens
  char *str; // for all tokens
  int len;
  Token *next;
};

typedef struct Type Type;

struct Type
{
  enum
  {
    I32,
    I64,
    PTR,
    ARRAY,
  } ty;
  Type *ptr_to;
  size_t array_size;
};

typedef struct LVar LVar;

// ローカル変数の型
struct LVar
{
  LVar *next; // 次の変数かNULL
  char *name; // 変数の名前
  int len;    // 名前の長さ
  int offset; // RBPからのオフセット
  Type *type;
};

// 抽象構文木のノードの種類
typedef struct Node Node;
typedef struct FnCall FnCall;

struct FnCall
{
  char *str;
  int len;
  Node **args;
  int argc;
};

typedef struct FnDef FnDef;
struct FnDef
{
  char *name;
  int len; // 関数名の長さ
  int paramc;
  Node **params;
  Node *body;
};

typedef struct Scope Scope;

struct Scope
{
  Scope *parent;
  Scope **children;
  int childc;
  int childm;
  LVar *locals;
  int localc;
};

// 抽象構文木のノードの型
struct Node
{
  NodeKind kind; //* ノードの型
  Node *lhs;     //! 左辺
  Node *rhs;     //? 右辺
  union
  {
    int val;    //! kindがND_NUMの場合のみ使う
    int offset; //* 変数の場合
  };
  union
  {
    FnCall *call;
    FnDef *def;
    Node **stmts;
    Type *type;
  };
};

// グローバル関数
// tokenizer.c
Token *tokenize(char *);
LVar *new_lvar(char *name, int len, Type *);
LVar *find_lvar(Token *tok, Scope *);
// ast.c
Node *primary();
Node *expr();
Node *stmt();
void gen(Node *);
void program();
Type *get_ptr(Type *);
// debug.c
void error(char *, ...);
void error_at(char *loc, const char *fmt, ...);
void warning(char *, ...);
void warning_at(char *loc, const char *fmt, ...);
// tokens.c
void init_tokens();
Token *add_token(Token);
void next_token();
Token *get_token();
// abi.c
int type_size(Type *);
int expr_size(Node *);

// グローバル変数
Node *code[100];
char *usr_in;
Scope *scope;
FnDef *fns;
Token *token;

#endif // NINE_CC_H