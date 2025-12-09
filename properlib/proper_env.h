// proper_env.h
#ifndef PROPER_ENV_H
#define PROPER_ENV_H

#include <stdlib.h>
#include <string.h>

// Get env var as string; return def if missing or empty
static inline const char *proper_env_get_str(const char *name,
                                             const char *def) {
  if (!name)
    return def;
  const char *val = getenv(name);
  return (val && val[0]) ? val : def;
}

// Get env var as integer (base 10); return def if missing/invalid
static inline int proper_env_get_int(const char *name, int def) {
  if (!name)
    return def;
  const char *val = getenv(name);
  if (!val || !val[0])
    return def;
  char *end;
  long l = strtol(val, &end, 10);
  if (*end != '\0' || end == val)
    return def;
  if (l < INT32_MIN || l > INT32_MAX)
    return def;
  return (int)l;
}

// Get env var as boolean: "1", "true", "yes" → true; else false
static inline int proper_env_get_bool(const char *name, int def) {
  const char *val = proper_env_get_str(name, NULL);
  if (!val)
    return def;
  if (strcmp(val, "1") == 0 || strcmp(val, "true") == 0 ||
      strcmp(val, "TRUE") == 0 || strcmp(val, "yes") == 0 ||
      strcmp(val, "YES") == 0) {
    return 1;
  }
  return 0;
}

#endif // PROPER_ENV_H
