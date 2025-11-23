
#ifndef BETTER_ENV_H
#define BETTER_ENV_H

#include <stdlib.h>
#include <string.h>

// Get env var or default
static inline const char* getenv_or(const char *key, const char *def) {
    const char *val = getenv(key);
    return val ? val : def;
}

// Parse env var as int
static inline int getenv_int(const char *key, int def) {
    const char *val = getenv(key);
    return val ? atoi(val) : def;
}

// Parse env var as bool (1/0)
static inline int getenv_bool(const char *key, int def) {
    const char *val = getenv(key);
    if (!val) return def;
    return (strcmp(val, "1") == 0 || strcasecmp(val, "true") == 0);
}

#endif // BETTER_ENV_H
