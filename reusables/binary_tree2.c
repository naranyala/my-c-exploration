
#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
  int data;
  struct Node *left;
  struct Node *right;
} Node;

Node *createNode(int data) {
  Node *n = (Node *)malloc(sizeof(Node));
  n->data = data;
  n->left = n->right = NULL;
  return n;
}

Node *insert(Node *root, int data) {
  if (!root)
    return createNode(data);
  if (data < root->data)
    root->left = insert(root->left, data);
  else
    root->right = insert(root->right, data);
  return root;
}

void inorder(Node *root) {
  if (!root)
    return;
  inorder(root->left);
  printf("%d ", root->data);
  inorder(root->right);
}

int main() {
  Node *root = NULL;
  root = insert(root, 20);
  root = insert(root, 10);
  root = insert(root, 30);

  inorder(root);
  printf("\n");
  return 0;
}
