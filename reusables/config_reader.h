
// config.h
#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <string.h>

static inline char *config_get(const char *filename, const char *key, char *buf,
                               size_t buflen) {
  FILE *f = fopen(filename, "r");
  if (!f)
    return NULL;
  while (fgets(buf, buflen, f)) {
    char k[64], v[256];
    if (sscanf(buf, "%63[^=]=%255[^\n]", k, v) == 2) {
      if (strcmp(k, key) == 0) {
        fclose(f);
        return strdup(v);
      }
    }
  }
  fclose(f);
  return NULL;
}

#endif
