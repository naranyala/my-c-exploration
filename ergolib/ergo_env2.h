/* ergo_env.h
 * Safe, typed helpers for environment variable access.
 */

#ifndef ERGO_ENV_H
#define ERGO_ENV_H

#include <stddef.h>  /* size_t */

#ifdef __cplusplus
extern "C" {
#endif

/* Get string env var; return default_val if not set or empty. */
const char *ergo_getenv_str(const char *name, const char *default_val);

/* Get integer env var; return default_val on error or missing. */
int ergo_getenv_int(const char *name, int default_val);

/* Get boolean env var: "1", "true", "yes" (case-insensitive) → 1; else 0. */
int ergo_getenv_bool(const char *name, int default_val);

/* Check if environment variable is set (even if empty). */
int ergo_env_is_set(const char *name);

#ifdef __cplusplus
}
#endif

#endif /* ERGO_ENV_H */

#ifdef ERGO_ENV_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static int ergo_strieq(const char *a, const char *b) {
    if (!a || !b) return 0;
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b))
            return 0;
        ++a; ++b;
    }
    return *a == *b;
}

const char *ergo_getenv_str(const char *name, const char *default_val) {
    if (!name) return default_val;
    const char *val = getenv(name);
    return (val && val[0] != '\0') ? val : default_val;
}

int ergo_getenv_int(const char *name, int default_val) {
    const char *val = ergo_getenv_str(name, NULL);
    if (!val) return default_val;

    char *end;
    long result = strtol(val, &end, 10);
    if (end == val || *end != '\0' || result < INT_MIN || result > INT_MAX)
        return default_val;
    return (int)result;
}

int ergo_getenv_bool(const char *name, int default_val) {
    const char *val = ergo_getenv_str(name, NULL);
    if (!val) return default_val;

    if (ergo_strieq(val, "1") ||
        ergo_strieq(val, "true") ||
        ergo_strieq(val, "yes") ||
        ergo_strieq(val, "on"))
        return 1;
    return 0;
}

int ergo_env_is_set(const char *name) {
    if (!name) return 0;
    return getenv(name) != NULL;
}

#endif /* ERGO_ENV_IMPLEMENTATION */
