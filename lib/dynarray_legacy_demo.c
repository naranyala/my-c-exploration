#include "dynarray.h"
#include <stdio.h>

// Example with integers
void int_example() {
  printf("=== Integer Array Example ===\n");

  dynarray(int) numbers = {0};

  // Append some numbers
  for (int i = 0; i < 10; i++) {
    dynarray_append(&numbers, i * i);
  }

  // Print all elements
  printf("Numbers: ");
  dynarray_foreach(&numbers, ptr) { printf("%d ", *ptr); }
  printf("\n");

  // Access elements
  printf("First: %d, Last: %d, Length: %zu\n", dynarray_first(&numbers),
         dynarray_last(&numbers), dynarray_len(&numbers));

  // Insert and remove
  dynarray_insert(&numbers, 3, 999);
  dynarray_remove(&numbers, 5);

  printf("After insert/remove: ");
  for (size_t i = 0; i < dynarray_len(&numbers); i++) {
    printf("%d ", dynarray_get(&numbers, i));
  }
  printf("\n");

  dynarray_free(&numbers);
}

// Example with strings
void string_example() {
  printf("\n=== String Array Example ===\n");

  dynarray(const char *) words = {0};

  dynarray_append(&words, "hello");
  dynarray_append(&words, "world");
  dynarray_append(&words, "dynamic");
  dynarray_append(&words, "arrays");

  printf("Words: ");
  dynarray_foreach(&words, ptr) { printf("%s ", *ptr); }
  printf("\n");

  // Pop last element
  const char *last = dynarray_pop(&words);
  printf("Popped: %s\n", last);
  printf("Remaining length: %zu\n", dynarray_len(&words));

  dynarray_free(&words);
}

// Example with custom struct
void struct_example() {
  printf("\n=== Struct Array Example ===\n");

  typedef struct {
    int x, y;
    char name[16];
  } Point;

  dynarray(Point) points = {0};

  Point p1 = {10, 20, "origin"};
  Point p2 = {30, 40, "target"};

  dynarray_append(&points, p1);
  dynarray_append(&points, p2);

  printf("Points:\n");
  dynarray_foreach(&points, ptr) {
    printf("  %s: (%d, %d)\n", ptr->name, ptr->x, ptr->y);
  }

  dynarray_free(&points);
}

// Bulk operations example
void bulk_example() {
  printf("\n=== Bulk Operations Example ===\n");

  dynarray(int) source = {0};
  dynarray(int) dest = {0};

  // Fill source array
  for (int i = 0; i < 5; i++) {
    dynarray_append(&source, i + 100);
  }

  // Bulk copy from source to dest
  dynarray_append_array(&dest, source.data, source.len);

  printf("Destination array: ");
  dynarray_foreach(&dest, ptr) { printf("%d ", *ptr); }
  printf("\n");

  dynarray_free(&source);
  dynarray_free(&dest);
}

int main() {
  int_example();
  string_example();
  struct_example();
  bulk_example();
  return 0;
}
