
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *concatenateStrings(const char *str1, const char *str2) {
  char *result = (char *)malloc(strlen(str1) + strlen(str2) + 1);
  if (result == NULL) {
    perror("Memory allocation failed");
    exit(EXIT_FAILURE);
  }
  strcpy(result, str1);
  strcat(result, str2);
  return result;
}
