/**
 * File I/O operations template
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256

int count_lines(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Error opening file");
    return -1;
  }

  int count = 0;
  char buffer[MAX_LINE_LENGTH];

  while (fgets(buffer, sizeof(buffer), file)) {
    count++;
  }

  fclose(file);
  return count;
}

int write_to_file(const char *filename, const char *content) {
  FILE *file = fopen(filename, "w");
  if (!file) {
    perror("Error opening file for writing");
    return 0;
  }

  if (fprintf(file, "%s", content) < 0) {
    perror("Error writing to file");
    fclose(file);
    return 0;
  }

  fclose(file);
  return 1;
}

void read_file_lines(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Error opening file");
    return;
  }

  char buffer[MAX_LINE_LENGTH];
  int line_num = 1;

  while (fgets(buffer, sizeof(buffer), file)) {
    // Remove newline character
    buffer[strcspn(buffer, "\n")] = 0;
    printf("Line %d: %s\n", line_num++, buffer);
  }

  fclose(file);
}

// Example usage
int main(void) {
  const char *filename = "test.txt";

  // Write to file
  if (write_to_file(filename,
                    "Hello, World!\nThis is a test file.\nThird line.")) {
    printf("File written successfully\n");
  }

  // Read and display file
  printf("\nFile contents:\n");
  read_file_lines(filename);

  // Count lines
  int line_count = count_lines(filename);
  printf("\nTotal lines: %d\n", line_count);

  return 0;
}
