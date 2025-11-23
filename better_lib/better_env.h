
/* better_env.h */

#ifndef BETTER_ENV_H
#define BETTER_ENV_H

#include <stdlib.h>
#include <string.h>

static inline const char *benv_get(const char *name,
                                   const char *defval)
{
    const char *v = getenv(name);
    return v ? v : defval;
}

static inline int benv_set(const char *name, const char *val) {
#ifdef _WIN32
    return _putenv_s(name, val) == 0;
#else
    return setenv(name, val, 1) == 0;
#endif
}

static inline int benv_unset(const char *name) {
#ifdef _WIN32
    return _putenv_s(name, "") == 0;
#else
    return unsetenv(name) == 0;
#endif
}

#endif
