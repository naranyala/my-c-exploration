/* ergo_path.h
 * Cross-platform path utilities (Unix-style paths; Windows-aware).
 */

#ifndef ERGO_PATH_H
#define ERGO_PATH_H

#include <stddef.h>  /* size_t */

#ifdef __cplusplus
extern "C" {
#endif

/* Join multiple path components into a single path (NULL-terminated args).
 * Returns malloc'd string. Caller must free().
 */
char *ergo_path_join(const char *first, ...);

/* Return pointer to last component (like basename). Does NOT allocate. */
const char *ergo_path_basename(const char *path);

/* Return 1 if path is absolute, 0 otherwise. */
int ergo_path_is_absolute(const char *path);

#ifdef __cplusplus
}
#endif

#endif /* ERGO_PATH_H */

#ifdef ERGO_PATH_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

static char ergo_path_separator(void) {
#if defined(_WIN32) || defined(_WIN64)
    return '\\';
#else
    return '/';
#endif
}

static int ergo_path_is_sep(char c) {
    return (c == '/') ||
#if defined(_WIN32) || defined(_WIN64)
           (c == '\\');
#else
           0;
#endif
}

char *ergo_path_join(const char *first, ...) {
    if (!first) return NULL;

    size_t total = strlen(first) + 1;
    va_list args;
    va_start(args, first);
    const char *arg;
    while ((arg = va_arg(args, const char *)) != NULL) {
        total += 1 + strlen(arg);  // +1 for separator
    }
    va_end(args);

    char *result = (char *)malloc(total);
    if (!result) return NULL;

    strcpy(result, first);
    va_start(args, first);
    while ((arg = va_arg(args, const char *)) != NULL) {
        size_t len = strlen(result);
        if (len > 0 && !ergo_path_is_sep(result[len - 1])) {
            result[len] = ergo_path_separator();
            result[len + 1] = '\0';
        }
        strcat(result, arg);
    }
    va_end(args);

    return result;
}

const char *ergo_path_basename(const char *path) {
    if (!path) return ".";
    const char *last = path;
    const char *p = path;
    while (*p) {
        if (ergo_path_is_sep(*p)) {
            last = p + 1;
        }
        ++p;
    }
    return last;
}

int ergo_path_is_absolute(const char *path) {
    if (!path || !*path) return 0;
#if defined(_WIN32) || defined(_WIN64)
    if (path[1] == ':' && (path[2] == '/' || path[2] == '\\'))
        return 1;  // C:\...
    if (ergo_path_is_sep(path[0]) && ergo_path_is_sep(path[1]))
        return 1;  // UNC path
#endif
    return ergo_path_is_sep(path[0]);
}

#endif /* ERGO_PATH_IMPLEMENTATION */
