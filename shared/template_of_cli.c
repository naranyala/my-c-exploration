
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--help") == 0) {
      printf("Usage: ./program [options]\n");
    } else if (strcmp(argv[i], "--version") == 0) {
      printf("Version 1.0\n");
    } else {
      printf("Unknown option: %s\n", argv[i]);
    }
  }
  return 0;
}
