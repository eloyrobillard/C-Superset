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
  int altitude;
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
    node->altitude = 0;
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
  node->altitude = 1 + (node->right->altitude > node->left->altitude ? node->right->altitude : node->left->altitude);
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
  node->altitude = 1 + (node->right->altitude > node->left->altitude ? node->right->altitude : node->left->altitude);
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

void print_op(Node *node, char ***top)
{
  switch (node->type)
  {
  case ND_ADD:
    if (node->right->type != ND_NUM)
      printf("\tadd %s, %s\n", *(*top)--, *(*top)--);
    else
      printf("\tadd %s, %ld\n", *(*top)--, node->right->val);
    break;
  case ND_SUB:
    if (node->right->type != ND_NUM)
      printf("\tsub %s, %s\n", *(*top)--, *(*top)--);
    else
      printf("\tsub %s, %ld\n", *(*top)--, node->right->val);
    break;
  case ND_MUL:
    if (node->right->type != ND_NUM)
      printf("\timul %s, %s\n", *(*top)--, *(*top)--);
    else
      printf("\timul %s, %ld\n", *(*top)--, node->right->val);
    break;
  case ND_DIV:
    printf("\tdiv %s\n", *(*top)--);
    break;
  }
}

void solve_tree(Node *tree, char ***top)
{
  if (tree->right->altitude > tree->left->altitude)
  {
    solve_tree(tree->right, top);
    solve_tree(tree->left, top);
    print_op(tree, top);
  }
  else
  {
    solve_tree(tree->left, top);
    solve_tree(tree->right, top);
    print_op(tree, top);
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
  char **regs = calloc(root->altitude, sizeof(char*));
  regs[0] = "rax";
  for (int i = 1; i < root->altitude; i++) 
  {
    regs[i][0] = 'r';
    regs[i][1] = i - 1 + '0';
  }
  char **top = &regs[root->altitude - 1];
  solve_tree(root, &top);

  printf("\tret\n");
  return 0;
}