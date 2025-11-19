/**
 * Singly linked list template
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
  int data;
  struct Node *next;
} Node;

Node *create_node(int data) {
  Node *new_node = malloc(sizeof(Node));
  if (!new_node)
    return NULL;

  new_node->data = data;
  new_node->next = NULL;
  return new_node;
}

void list_append(Node **head, int data) {
  Node *new_node = create_node(data);
  if (!new_node)
    return;

  if (*head == NULL) {
    *head = new_node;
    return;
  }

  Node *current = *head;
  while (current->next != NULL) {
    current = current->next;
  }
  current->next = new_node;
}

void list_prepend(Node **head, int data) {
  Node *new_node = create_node(data);
  if (!new_node)
    return;

  new_node->next = *head;
  *head = new_node;
}

void list_print(const Node *head) {
  const Node *current = head;
  while (current != NULL) {
    printf("%d -> ", current->data);
    current = current->next;
  }
  printf("NULL\n");
}

void list_destroy(Node *head) {
  Node *current = head;
  while (current != NULL) {
    Node *temp = current;
    current = current->next;
    free(temp);
  }
}

// Example usage
int main(void) {
  Node *head = NULL;

  list_append(&head, 10);
  list_append(&head, 20);
  list_prepend(&head, 5);
  list_append(&head, 30);

  list_print(head);
  list_destroy(head);

  return 0;
}
