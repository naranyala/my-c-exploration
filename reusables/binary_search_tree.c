/**
 * Binary Search Tree implementation
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct TreeNode {
  int data;
  struct TreeNode *left;
  struct TreeNode *right;
} TreeNode;

TreeNode *tree_create_node(int data) {
  TreeNode *node = malloc(sizeof(TreeNode));
  if (!node)
    return NULL;

  node->data = data;
  node->left = node->right = NULL;
  return node;
}

TreeNode *tree_insert(TreeNode *root, int data) {
  if (!root)
    return tree_create_node(data);

  if (data < root->data) {
    root->left = tree_insert(root->left, data);
  } else if (data > root->data) {
    root->right = tree_insert(root->right, data);
  }

  return root;
}

TreeNode *tree_search(TreeNode *root, int data) {
  if (!root || root->data == data)
    return root;

  if (data < root->data) {
    return tree_search(root->left, data);
  } else {
    return tree_search(root->right, data);
  }
}

void tree_inorder_traversal(TreeNode *root) {
  if (root) {
    tree_inorder_traversal(root->left);
    printf("%d ", root->data);
    tree_inorder_traversal(root->right);
  }
}

void tree_destroy(TreeNode *root) {
  if (root) {
    tree_destroy(root->left);
    tree_destroy(root->right);
    free(root);
  }
}

// Example usage
int main(void) {
  TreeNode *root = NULL;

  root = tree_insert(root, 50);
  tree_insert(root, 30);
  tree_insert(root, 70);
  tree_insert(root, 20);
  tree_insert(root, 40);
  tree_insert(root, 60);
  tree_insert(root, 80);

  printf("In-order traversal: ");
  tree_inorder_traversal(root);
  printf("\n");

  TreeNode *found = tree_search(root, 40);
  if (found) {
    printf("Found: %d\n", found->data);
  }

  tree_destroy(root);
  return 0;
}
