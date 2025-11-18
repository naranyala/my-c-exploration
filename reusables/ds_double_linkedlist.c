
#include <stdio.h>
#include <stdlib.h>

typedef struct DoublyNode {
    int data;
    struct DoublyNode *prev;
    struct DoublyNode *next;
} DoublyNode;

// Create a new node
DoublyNode *createDoublyNode(int data) {
    DoublyNode *newNode = (DoublyNode *)malloc(sizeof(DoublyNode));
    newNode->data = data;
    newNode->prev = NULL;
    newNode->next = NULL;
    return newNode;
}

// Insert a node at the end
void insertDoublyEnd(DoublyNode **head, int data) {
    DoublyNode *newNode = createDoublyNode(data);
    if (*head == NULL) {
        *head = newNode;
        return;
    }
    DoublyNode *current = *head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = newNode;
    newNode->prev = current;
}

// Delete a node by value
void deleteDoublyNode(DoublyNode **head, int data) {
    if (*head == NULL) return;
    DoublyNode *current = *head;
    while (current != NULL && current->data != data) {
        current = current->next;
    }
    if (current == NULL) return;
    if (current->prev != NULL) {
        current->prev->next = current->next;
    } else {
        *head = current->next;
    }
    if (current->next != NULL) {
        current->next->prev = current->prev;
    }
    free(current);
}
