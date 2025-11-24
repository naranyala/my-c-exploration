#include <stdlib.h>

#ifndef CPATH_H
#define CPATH_H

int path_join(char *out, size_t outsz, const char *a, const char *b);
int path_dirname(const char *path, char *out, size_t outsz);
int path_basename(const char *path, char *out, size_t outsz);

#ifdef CPATH_IMPLEMENTATION
#include <string.h>

static char sep() {
#if _WIN32
    return '\\';
#else
    return '/';
#endif
}

int path_join(char *out, size_t outsz, const char *a, const char *b) {
    size_t la = strlen(a), lb = strlen(b);
    if (la + lb + 2 > outsz) return 0;

    strcpy(out, a);
    if (out[la-1] != sep()) out[la] = sep(), out[++la] = 0;
    strcat(out, b);
    return 1;
}

int path_basename(const char *path, char *out, size_t outsz) {
    const char *p = strrchr(path, sep());
    p = p ? p+1 : path;
    if (strlen(p)+1 > outsz) return 0;
    strcpy(out, p);
    return 1;
}

int path_dirname(const char *path, char *out, size_t outsz) {
    const char *p = strrchr(path, sep());
    if (!p) return 0;
    size_t len = p - path;
    if (len+1 > outsz) return 0;
    strncpy(out, path, len);
    out[len] = 0;
    return 1;
}

#endif // CPATH_IMPLEMENTATION
#endif

