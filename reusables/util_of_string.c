
#include <ctype.h>
#include <stdio.h>
#include <string.h>

void strToUpper(char *str) {
  for (int i = 0; str[i]; i++)
    str[i] = toupper(str[i]);
}

int main() {
  char text[] = "hello world";
  strToUpper(text);
  printf("%s\n", text);
  return 0;
}
