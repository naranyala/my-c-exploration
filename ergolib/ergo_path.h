#ifndef ERGO_PATH_H
#define ERGO_PATH_H

#include <string.h>

// Get basename (last component of path)
static inline const char *ergo_path_basename(const char *path) {
    const char *slash = strrchr(path, '/');
    return slash ? slash + 1 : path;
}

// Get dirname (everything before last slash)
static inline void ergo_path_dirname(const char *path, char *buf, size_t len) {
    const char *slash = strrchr(path, '/');
    if (!slash) {
        strncpy(buf, ".", len);
        return;
    }
    size_t n = slash - path;
    if (n >= len) n = len - 1;
    strncpy(buf, path, n);
    buf[n] = '\0';
}

// Join two paths safely
static inline void ergo_path_join(const char *a, const char *b, char *buf, size_t len) {
    snprintf(buf, len, "%s/%s", a, b);
}

#endif // ERGO_PATH_H

