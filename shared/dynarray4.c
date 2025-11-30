/**
 * Dynamic array template
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int *data;
  size_t size;
  size_t capacity;
} DynamicArray;

DynamicArray *da_create(size_t initial_capacity) {
  DynamicArray *da = malloc(sizeof(DynamicArray));
  if (!da)
    return NULL;

  da->data = malloc(initial_capacity * sizeof(int));
  if (!da->data) {
    free(da);
    return NULL;
  }

  da->size = 0;
  da->capacity = initial_capacity;
  return da;
}

void da_destroy(DynamicArray *da) {
  if (da) {
    free(da->data);
    free(da);
  }
}

int da_push_back(DynamicArray *da, int value) {
  if (da->size >= da->capacity) {
    size_t new_capacity = da->capacity * 2;
    int *new_data = realloc(da->data, new_capacity * sizeof(int));
    if (!new_data)
      return 0;

    da->data = new_data;
    da->capacity = new_capacity;
  }

  da->data[da->size++] = value;
  return 1;
}

int da_get(const DynamicArray *da, size_t index) {
  if (index >= da->size) {
    fprintf(stderr, "Index out of bounds\n");
    return 0;
  }
  return da->data[index];
}

// Example usage
int main(void) {
  DynamicArray *da = da_create(10);

  for (int i = 0; i < 20; i++) {
    da_push_back(da, i * 10);
  }

  for (size_t i = 0; i < da->size; i++) {
    printf("da[%zu] = %d\n", i, da_get(da, i));
  }

  da_destroy(da);
  return 0;
}
