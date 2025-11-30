
#include <stdio.h>
#include <stdlib.h>

#define MAX 100

typedef struct {
  int arr[MAX];
  int top;
} Stack;

void init(Stack *s) { s->top = -1; }

int isEmpty(Stack *s) { return s->top == -1; }

int isFull(Stack *s) { return s->top == MAX - 1; }

void push(Stack *s, int val) {
  if (!isFull(s))
    s->arr[++(s->top)] = val;
}

int pop(Stack *s) {
  if (!isEmpty(s))
    return s->arr[(s->top)--];
  return -1; // empty stack
}

int main() {
  Stack s;
  init(&s);

  push(&s, 10);
  push(&s, 20);
  printf("Popped: %d\n", pop(&s));

  return 0;
}
