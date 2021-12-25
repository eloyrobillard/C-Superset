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

Node *make_tree(char **);

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
  node = make_tree(p);
  return node;
}

Node *get_mul(char **p)
{
  Node *node = calloc(1, sizeof(Node));
  node->str = *p;
  node->left = get_prim(p);

  while (isspace(**p))
    (*p)++;
  if (**p == '*')
    node->type = ND_MUL;
  else if (**p == '/')
    node->type = ND_DIV;
  else
    return node->left;

  (*p)++;
  while (isspace(**p))
    (*p)++;
  node->right = get_prim(p);
  return node;
}

Node *get_expr(Node *left, char **p)
{
  Node *node = calloc(1, sizeof(Node));
  node->str = *p;
  node->left = left;

  while (isspace(**p))
    (*p)++;
  if (**p == '+')
    node->type = ND_ADD;
  else if (**p == '-')
    node->type = ND_SUB;
  else
    return node->left;

  (*p)++;
  while (isspace(**p))
    (*p)++;
  node->right = get_mul(p);
  return node;
}

Node *make_tree(char **p)
{
  while (isspace(**p))
    (*p)++;

  Node *node = get_mul(p);
  while (**p)
  {
    node = get_expr(node, p);
  }

  return node;
}

long solve_tree(Node *tree)
{
  switch (tree->type)
  {
  case ND_NUM:
    return tree->val;
  case ND_ADD:
    return solve_tree(tree->left) + solve_tree(tree->left);
  case ND_SUB:
    return solve_tree(tree->left) - solve_tree(tree->left);
  case ND_MUL:
    return solve_tree(tree->left) * solve_tree(tree->left);
  case ND_DIV:
    return solve_tree(tree->left) / solve_tree(tree->left);
  }
}

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    fprintf(stderr, "引数の個数が合ってない");
    exit(1);
  }

  Node *root = make_tree(++argv);

  return solve_tree(root);
}