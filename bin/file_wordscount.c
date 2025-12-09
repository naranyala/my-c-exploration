
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  // Check if file path is provided
  if (argc != 2) {
    printf("Usage: %s <file_path>\n", argv[0]);
    return 1;
  }

  FILE *file = fopen(argv[1], "r");
  if (file == NULL) {
    perror("Error opening file");
    return 1;
  }

  int lines = 0, words = 0, characters = 0;
  int in_word = 0; // Flag to check if currently inside a word
  char ch;

  while ((ch = fgetc(file)) != EOF) {
    characters++;

    if (ch == '\n') {
      lines++;
    }

    if (ch == ' ' || ch == '\t' || ch == '\n') {
      in_word = 0;
    } else if (in_word == 0) {
      in_word = 1;
      words++;
    }
  }

  fclose(file);

  printf("File: %s\n", argv[1]);
  printf("Lines: %d\n", lines);
  printf("Words: %d\n", words);
  printf("Characters: %d\n", characters);

  return 0;
}
