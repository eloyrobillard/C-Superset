#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

/*
 * expr    = mul ("+" mul | "-" mul)*
 * mul     = primary ("*" primary | "/" primary)*
 * primary = num | "(" expr ")"
 */

typedef enum NodeType
{
  ND_NUM,
  ND_ADD,
  ND_SUB,
  ND_MUL,
  ND_DIV,
} NodeType;

typedef struct Node Node;
struct Node
{
  Node *left;
  Node *right;
  NodeType type;
  char *str;
  long val;
};

Node *get_prim(char **p)
{
  Node *node = calloc(1, sizeof(Node));
  node->str = *p;
  if (isdigit(**p))
  {
    node->type = ND_NUM;
    node->val = strtol(*p, p, 10);
    return node;
  }
  (*p)++;
  while (isspace(**p))
    (*p)++;
  node = get_expr(p);
  return node;
}

Node *get_mul(char **p)
{
  Node *node = calloc(1, sizeof(Node));
  node->str = *p;
  node->left = get_prim(p);
  while (**p)
  {
    if (**p == '*')
      node->type = ND_MUL;
    else if (**p == '/')
      node->type = ND_DIV;
  }
  if (**p)
    node->right = get_prim(p);
  else
    node = node->left;
  return node;
}

Node *get_expr(char **p)
{
  Node *node = calloc(1, sizeof(Node));
  node->str = *p;
  node->left = get_mul(p);
  while (**p)
  {
    if (**p == '+')
      node->type = ND_ADD;
    else if (**p == '-')
      node->type = ND_SUB;
  }
  if (**p)
    node->right = get_mul(p);
  else
    node = node->left;
  return node;
}

Node *make_tree(char *p)
{
  Node root;
  Node *cur = &root;

  while (*p && *p != ')')
  {
    if (isspace(*p))
    {
      p++;
      continue;
    }

    root.left = get_expr(&p);
    // else error("無効な入力");
  }

  return cur->left;
}

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    fprintf(stderr, "引数の個数が合ってない");
    exit(1);
  }

  Node *root = make_tree(argv[1]);
}