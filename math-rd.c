#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

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
  while (**p && **p != ')')
  {
    node = get_expr(node, p);
  }
  if (**p == ')')
    (*p)++;

  return node;
}

void solve_tree(Node *tree, bool left)
{
  if (tree->type == ND_NUM)
  {
    left ? printf("\tmov rax, %ld\n", tree->val) : printf("\tmov rbx, %ld\n", tree->val);
    return;
  }

  if (tree->left->type == ND_NUM && tree->right->type != ND_NUM)
  {
    solve_tree(tree->right, false);
    solve_tree(tree->left, true);
  }
  else
  {
    solve_tree(tree->left, true);
    solve_tree(tree->right, false);
  }
  switch (tree->type)
  {
  case ND_ADD:
    left ? printf("\tadd rax, rbx\n") : printf("\tadd rbx, rax\n");
    break;
  case ND_SUB:
    left ? printf("\tsub rax, rbx\n") : printf("\tsub rbx, rax\n");
    break;
  case ND_MUL:
    left ? printf("\timul rax, rbx\n") : printf("\timul rbx, rax\n");
    break;
  case ND_DIV:
    left ? printf("\tdiv rax\n") : printf("\tdiv rbx\n");
    break;
  }
}

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    fprintf(stderr, "引数の個数が合ってない");
    exit(1);
  }

  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("\n");
  printf("main:\n");

  Node *root = make_tree(++argv);
  solve_tree(root, true);

  printf("\tret\n");
  return 0;
}