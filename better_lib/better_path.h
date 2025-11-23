
/* better_path.h */

#ifndef BETTER_PATH_H
#define BETTER_PATH_H

#include <string.h>

static inline const char *bpath_basename(const char *path) {
    const char *p = strrchr(path, '/');
#ifdef _WIN32
    const char *q = strrchr(path, '\\');
    if (q && (!p || q > p)) p = q;
#endif
    return p ? p + 1 : path;
}

static inline const char *bpath_ext(const char *path) {
    const char *b = bpath_basename(path);
    const char *dot = strrchr(b, '.');
    return dot ? dot + 1 : "";
}

static inline void bpath_join(char *out, size_t outsz,
                              const char *a, const char *b) {
    snprintf(out, outsz, "%s/%s", a, b);
}

#endif
