/* ergo_path.h */
#pragma once
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <unistd.h>

static inline char* path_join(const char *a, const char *b) {
    size_t na = strlen(a), nb = strlen(b);
    int need_slash = a[na-1] != '/' && a[na-1] != '\\';
    char *p = malloc(na + need_slash + nb + 1);
    memcpy(p, a, na);
    if (need_slash) p[na++] = '/';
    memcpy(p + na, b, nb + 1);
    return p;
}

static inline char* path_dirname(const char *path) {
    char *s = strdup(path);
    return dirname(s);  /* note: modifies input, returns pointer into it */
}

static inline char* path_basename(const char *path) {
    char *s = strdup(path);
    return basename(s);
}

static inline int path_exists(const char *path) {
    return access(path, F_OK) == 0;
}

