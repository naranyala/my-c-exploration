
#include <stdio.h>
#include <string.h>

int main() {
  FILE *fp = fopen("config.ini", "r");
  if (!fp)
    return 1;

  char line[256];
  while (fgets(line, sizeof(line), fp)) {
    char key[128], value[128];
    if (sscanf(line, "%127[^=]=%127s", key, value) == 2) {
      printf("Key: %s, Value: %s\n", key, value);
    }
  }

  fclose(fp);
  return 0;
}
