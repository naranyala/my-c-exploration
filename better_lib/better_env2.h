// better_env.h
#ifndef BETTER_ENV_H
#define BETTER_ENV_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

// Get string (returns default if not set)
const char* b_getenv_str(const char* name, const char* default_value);

// Get integer (returns default on parse fail or missing)
long b_getenv_int(const char* name, long default_value);

// Get boolean: "1", "true", "yes", "on" → 1; else 0
int b_getenv_bool(const char* name, int default_value);

#ifdef __cplusplus
}
#endif

#ifdef BETTER_ENV_IMPL

const char* b_getenv_str(const char* name, const char* default_value) {
    if (!name) return default_value;
    const char* val = getenv(name);
    return val ? val : default_value;
}

long b_getenv_int(const char* name, long default_value) {
    const char* val = b_getenv_str(name, NULL);
    if (!val) return default_value;
    char* end;
    long result = strtol(val, &end, 10);
    if (end == val || *end != '\0') return default_value; // invalid
    return result;
}

int b_getenv_bool(const char* name, int default_value) {
    const char* val = b_getenv_str(name, NULL);
    if (!val) return default_value;
    // Case-insensitive comparison (simple)
    if (val[0] == '1' && val[1] == '\0') return 1;
    if (val[0] == '0' && val[1] == '\0') return 0;
    if (strcasecmp(val, "true") == 0) return 1;
    if (strcasecmp(val, "false") == 0) return 0;
    if (strcasecmp(val, "yes") == 0) return 1;
    if (strcasecmp(val, "no") == 0) return 0;
    if (strcasecmp(val, "on") == 0) return 1;
    if (strcasecmp(val, "off") == 0) return 0;
    return default_value;
}

#endif // BETTER_ENV_IMPL

#endif // BETTER_ENV_H
