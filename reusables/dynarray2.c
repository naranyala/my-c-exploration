
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int *data;
  size_t size;
  size_t capacity;
} DynamicArray;

void initArray(DynamicArray *arr, size_t capacity) {
  arr->data = (int *)malloc(capacity * sizeof(int));
  arr->size = 0;
  arr->capacity = capacity;
}

void push(DynamicArray *arr, int value) {
  if (arr->size == arr->capacity) {
    arr->capacity *= 2;
    arr->data = (int *)realloc(arr->data, arr->capacity * sizeof(int));
  }
  arr->data[arr->size++] = value;
}

void freeArray(DynamicArray *arr) {
  free(arr->data);
  arr->size = arr->capacity = 0;
}

int main() {
  DynamicArray arr;
  initArray(&arr, 4);

  for (int i = 0; i < 10; i++)
    push(&arr, i);
  for (int i = 0; i < arr.size; i++)
    printf("%d ", arr.data[i]);
  printf("\n");

  freeArray(&arr);
  return 0;
}
