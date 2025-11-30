#include <stdio.h>
#include <stdlib.h>

// 1. Define the Structure
// We bundle the data, the current size, and the capacity together.
typedef struct {
  int *data;       // Pointer to the array in the Heap
  size_t size;     // Number of elements currently used
  size_t capacity; // Total slots available before we need to resize
} Vector;

// 2. Initialize the Vector
// Returns 1 on success, 0 on failure
int vector_init(Vector *v, size_t initial_capacity) {
  v->size = 0;
  v->capacity = initial_capacity;
  v->data = malloc(initial_capacity * sizeof(int));

  // ALWAYS check if malloc failed
  if (v->data == NULL) {
    return 0;
  }
  return 1;
}

// 3. Add Element (The "Logic" Part)
// Checks if we are full -> Doubles capacity -> Reallocates memory -> Adds item
int vector_push_back(Vector *v, int value) {
  if (v->size >= v->capacity) {
    // Double the capacity (Logic: minimize frequent reallocations)
    size_t new_capacity = v->capacity * 2;
    int *new_data = realloc(v->data, new_capacity * sizeof(int));

    if (new_data == NULL) {
      return 0; // Allocation failed, data remains untouched
    }

    v->data = new_data;
    v->capacity = new_capacity;
    printf("--- [Log] Resized vector capacity to %zu ---\n", new_capacity);
  }

  v->data[v->size++] = value;
  return 1;
}

// 4. Cleanup (Crucial in C)
void vector_free(Vector *v) {
  free(v->data);
  v->data = NULL;
  v->size = 0;
  v->capacity = 0;
}

int main() {
  Vector my_numbers;

  // Initialize
  if (!vector_init(&my_numbers, 2)) {
    fprintf(stderr, "Memory allocation failed!\n");
    return 1;
  }

  // Add numbers to trigger resizing
  for (int i = 0; i < 5; i++) {
    printf("Adding %d...\n", i * 10);
    vector_push_back(&my_numbers, i * 10);
  }

  // Print results
  printf("Final List: ");
  for (size_t i = 0; i < my_numbers.size; i++) {
    printf("%d ", my_numbers.data[i]);
  }
  printf("\n");

  // Clean up memory
  vector_free(&my_numbers);

  return 0;
}
