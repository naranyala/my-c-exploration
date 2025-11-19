
#include <stdio.h>
#include <stdlib.h>

int main() {
  FILE *fp = fopen("example.txt", "w");
  if (fp == NULL) {
    perror("Error opening file");
    return 1;
  }

  fprintf(fp, "Hello, file!\n");
  fclose(fp);

  fp = fopen("example.txt", "r");
  if (fp == NULL) {
    perror("Error opening file");
    return 1;
  }

  char buffer[100];
  while (fgets(buffer, sizeof(buffer), fp) != NULL) {
    printf("%s", buffer);
  }

  fclose(fp);
  return 0;
}
