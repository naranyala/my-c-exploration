
// list.h
#ifndef LIST_H
#define LIST_H

#include <stdlib.h>

typedef struct Node {
  void *data;
  struct Node *next;
} Node;

static inline Node *list_push(Node *head, void *data) {
  Node *node = malloc(sizeof(Node));
  node->data = data;
  node->next = head;
  return node;
}

static inline void list_free(Node *head) {
  while (head) {
    Node *tmp = head;
    head = head->next;
    free(tmp);
  }
}

#endif
