#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  void **data;
  size_t size;
  size_t capacity;
} GenericStack;

GenericStack *stack_create() {
  GenericStack *stack = malloc(sizeof(GenericStack));
  stack->capacity = 8;
  stack->size = 0;
  stack->data = malloc(stack->capacity * sizeof(void *));
  return stack;
}

void stack_push(GenericStack *stack, void *item) {
  if (stack->size >= stack->capacity) {
    stack->capacity *= 2;
    stack->data = realloc(stack->data, stack->capacity * sizeof(void *));
  }
  stack->data[stack->size++] = item;
}

void *stack_pop(GenericStack *stack) {
  if (stack->size == 0)
    return NULL;
  return stack->data[--stack->size];
}

void *stack_peek(GenericStack *stack) {
  if (stack->size == 0)
    return NULL;
  return stack->data[stack->size - 1];
}

int stack_is_empty(GenericStack *stack) { return stack->size == 0; }

void stack_free(GenericStack *stack) {
  free(stack->data);
  free(stack);
}

// Example usage with different types
int main() {
  GenericStack *stack = stack_create();

  // Push integers (as pointers)
  int *num1 = malloc(sizeof(int));
  int *num2 = malloc(sizeof(int));
  *num1 = 100;
  *num2 = 200;

  stack_push(stack, num1);
  stack_push(stack, num2);

  printf("Popped: %d\n", *(int *)stack_pop(stack));
  printf("Popped: %d\n", *(int *)stack_pop(stack));

  // Push strings
  char *str1 = "Hello";
  char *str2 = "World";

  stack_push(stack, str1);
  stack_push(stack, str2);

  printf("Popped: %s\n", (char *)stack_pop(stack));
  printf("Popped: %s\n", (char *)stack_pop(stack));

  stack_free(stack);
  free(num1);
  free(num2);
  return 0;
}
