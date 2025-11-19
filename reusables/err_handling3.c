/**
 * Error handling template
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  SUCCESS = 0,
  ERROR_NULL_POINTER,
  ERROR_MEMORY_ALLOCATION,
  ERROR_FILE_IO,
  ERROR_INVALID_INPUT
} ErrorCode;

const char *error_message(ErrorCode code) {
  switch (code) {
  case SUCCESS:
    return "Success";
  case ERROR_NULL_POINTER:
    return "Null pointer error";
  case ERROR_MEMORY_ALLOCATION:
    return "Memory allocation error";
  case ERROR_FILE_IO:
    return "File I/O error";
  case ERROR_INVALID_INPUT:
    return "Invalid input error";
  default:
    return "Unknown error";
  }
}

void print_error(ErrorCode code, const char *context) {
  fprintf(stderr, "Error: %s (Context: %s)\n", error_message(code), context);
}

void *safe_malloc(size_t size, const char *context) {
  void *ptr = malloc(size);
  if (!ptr) {
    print_error(ERROR_MEMORY_ALLOCATION, context);
    exit(EXIT_FAILURE);
  }
  return ptr;
}

FILE *safe_fopen(const char *filename, const char *mode, const char *context) {
  FILE *file = fopen(filename, mode);
  if (!file) {
    fprintf(stderr, "Error opening file '%s': %s (Context: %s)\n", filename,
            strerror(errno), context);
    exit(EXIT_FAILURE);
  }
  return file;
}

// Example usage with error handling
int process_data(int *data, size_t size) {
  if (!data) {
    print_error(ERROR_NULL_POINTER, "process_data");
    return ERROR_NULL_POINTER;
  }

  if (size == 0) {
    print_error(ERROR_INVALID_INPUT, "process_data - zero size");
    return ERROR_INVALID_INPUT;
  }

  // Process data here
  for (size_t i = 0; i < size; i++) {
    printf("Data[%zu] = %d\n", i, data[i]);
  }

  return SUCCESS;
}

int main(void) {
  // Example of safe memory allocation
  int *data = safe_malloc(10 * sizeof(int), "main - data array");

  for (int i = 0; i < 10; i++) {
    data[i] = i * 10;
  }

  ErrorCode result = process_data(data, 10);
  if (result != SUCCESS) {
    // Handle error
    free(data);
    return EXIT_FAILURE;
  }

  free(data);
  return EXIT_SUCCESS;
}
