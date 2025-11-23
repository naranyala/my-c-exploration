#ifndef ERGO_ENV_H
#define ERGO_ENV_H

#include <stdlib.h>
#include <string.h>

// Get environment variable or default
static inline const char *ergo_env_get(const char *name, const char *def) {
    const char *val = getenv(name);
    return val ? val : def;
}

// Set environment variable
static inline int ergo_env_set(const char *name, const char *value, int overwrite) {
    return setenv(name, value, overwrite);
}

// Unset environment variable
static inline int ergo_env_unset(const char *name) {
    return unsetenv(name);
}

#endif // ERGO_ENV_H

