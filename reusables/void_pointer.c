/**
 * Generic vector implementation using void pointers
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  void **data;
  size_t size;
  size_t capacity;
  size_t element_size;
} Vector;

Vector *vector_create(size_t initial_capacity, size_t element_size) {
  Vector *vec = malloc(sizeof(Vector));
  if (!vec)
    return NULL;

  vec->data = malloc(initial_capacity * sizeof(void *));
  if (!vec->data) {
    free(vec);
    return NULL;
  }

  vec->size = 0;
  vec->capacity = initial_capacity;
  vec->element_size = element_size;
  return vec;
}

void vector_destroy(Vector *vec) {
  if (vec) {
    for (size_t i = 0; i < vec->size; i++) {
      free(vec->data[i]);
    }
    free(vec->data);
    free(vec);
  }
}

int vector_push_back(Vector *vec, const void *element) {
  if (vec->size >= vec->capacity) {
    size_t new_capacity = vec->capacity * 2;
    void **new_data = realloc(vec->data, new_capacity * sizeof(void *));
    if (!new_data)
      return 0;

    vec->data = new_data;
    vec->capacity = new_capacity;
  }

  void *new_element = malloc(vec->element_size);
  if (!new_element)
    return 0;

  memcpy(new_element, element, vec->element_size);
  vec->data[vec->size++] = new_element;
  return 1;
}

void *vector_get(const Vector *vec, size_t index) {
  if (index >= vec->size)
    return NULL;
  return vec->data[index];
}

// Example usage with different data types
int main(void) {
  // Integer vector
  Vector *int_vec = vector_create(5, sizeof(int));
  for (int i = 0; i < 10; i++) {
    vector_push_back(int_vec, &i);
  }

  for (size_t i = 0; i < int_vec->size; i++) {
    int *value = vector_get(int_vec, i);
    printf("int_vec[%zu] = %d\n", i, *value);
  }
  vector_destroy(int_vec);

  // Double vector
  Vector *double_vec = vector_create(5, sizeof(double));
  for (double d = 0.5; d < 5.0; d += 0.5) {
    vector_push_back(double_vec, &d);
  }

  for (size_t i = 0; i < double_vec->size; i++) {
    double *value = vector_get(double_vec, i);
    printf("double_vec[%zu] = %.2f\n", i, *value);
  }
  vector_destroy(double_vec);

  return 0;
}
