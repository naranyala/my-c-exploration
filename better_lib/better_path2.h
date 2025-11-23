#include <string.h>

/* better_path.h - join, basename, dirname, is_absolute, normalize */
static inline void bpath_join(char* dst, size_t n, const char* a, const char* b) {
    int has_slash = a[strlen(a)-1] == '/' || a[strlen(a)-1] == '\\';
    snprintf(dst, n, "%s%s%s", a, has_slash ? "" : "/", b);
}

static inline const char* bpath_basename(const char* path) {
    const char* p = strrchr(path, '/');
    #ifdef _WIN32
    const char* b = strrchr(path, '\\'); if (b && b > p) p = b;
    #endif
    return p ? p + 1 : path;
}
