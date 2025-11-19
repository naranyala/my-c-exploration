
#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
  int data;
  struct Node *next;
} Node;

// Insert at beginning
Node *insert(Node *head, int value) {
  Node *newNode = (Node *)malloc(sizeof(Node));
  newNode->data = value;
  newNode->next = head;
  return newNode;
}

// Print list
void printList(Node *head) {
  Node *temp = head;
  while (temp != NULL) {
    printf("%d -> ", temp->data);
    temp = temp->next;
  }
  printf("NULL\n");
}

int main() {
  Node *head = NULL;
  head = insert(head, 10);
  head = insert(head, 20);
  head = insert(head, 30);

  printList(head);

  // Free memory
  Node *temp;
  while (head) {
    temp = head;
    head = head->next;
    free(temp);
  }

  return 0;
}
