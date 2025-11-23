// better_path.h
#ifndef BETTER_PATH_H
#define BETTER_PATH_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Join paths: b_path_join(buf, sizeof(buf), "dir", "sub", "file.txt", NULL);
// Returns number of written bytes (excluding null), or -1 on overflow.
ptrdiff_t b_path_join(char* buf, size_t buf_size, ...);

// Create directory and all parent dirs (like mkdir -p)
// Returns 0 on success, -1 on error.
int b_mkdir_p(const char* path);

// Get base name (e.g., "/a/b.txt" → "b.txt")
// Result is a pointer into the input string (or "." if empty).
const char* b_basename(const char* path);

// Get directory name (e.g., "/a/b.txt" → "/a")
// Returns static buffer (NOT THREAD-SAFE); use only once per statement.
const char* b_dirname(const char* path);

#ifdef __cplusplus
}
#endif

#ifdef BETTER_PATH_IMPL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <errno.h>

#ifndef S_ISDIR
#define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
#endif

#if defined(_WIN32) || defined(_WIN64)
#define PATH_SEP '\\'
#define PATH_SEP_STR "\\"
#else
#define PATH_SEP '/'
#define PATH_SEP_STR "/"
#endif

ptrdiff_t b_path_join(char* buf, size_t buf_size, ...) {
    if (!buf || buf_size == 0) return -1;
    buf[0] = '\0';
    size_t total = 0;

    va_list args;
    va_start(args, buf_size);
    const char* part;
    while ((part = va_arg(args, const char*)) != NULL) {
        if (!part[0]) continue; // skip empty

        size_t part_len = strlen(part);
        // Trim leading slashes
        while (part_len > 0 && (part[0] == '/' || part[0] == '\\')) {
            part++;
            part_len--;
        }
        // Trim trailing slashes
        while (part_len > 0 && (part[part_len - 1] == '/' || part[part_len - 1] == '\\')) {
            part_len--;
        }
        if (part_len == 0) continue;

        if (total > 0) {
            // Add separator if not at start
            if (total + 1 >= buf_size) goto overflow;
            buf[total] = PATH_SEP;
            total++;
        }

        if (total + part_len >= buf_size) goto overflow;
        memcpy(buf + total, part, part_len);
        total += part_len;
    }
    va_end(args);

    if (total >= buf_size) goto overflow;
    buf[total] = '\0';
    return (ptrdiff_t)total;

overflow:
    va_end(args);
    if (buf_size > 0) buf[0] = '\0';
    return -1;
}

// Internal: create single dir
static int b_mkdir_one(const char* path) {
#if defined(_WIN32) || defined(_WIN64)
    return _mkdir(path);
#else
    return mkdir(path, 0755);
#endif
}

int b_mkdir_p(const char* path) {
    if (!path) return -1;
    size_t len = strlen(path);
    if (len == 0) return -1;

    char* temp = malloc(len + 1);
    if (!temp) return -1;
    strcpy(temp, path);

    // Normalize: replace backslashes on Windows
#if defined(_WIN32) || defined(_WIN64)
    for (char* p = temp; *p; p++) {
        if (*p == '/') *p = '\\';
    }
#endif

    char* p = temp;
    // Skip leading separator
    if (*p == PATH_SEP) p++;

    while (*p) {
        // Find next separator
        char* sep = strchr(p, PATH_SEP);
        if (sep) *sep = '\0';

        if (strlen(temp) > 0) {
            struct stat st;
            if (stat(temp, &st) != 0) {
                if (b_mkdir_one(temp) != 0) {
                    int saved_errno = errno;
                    free(temp);
                    errno = saved_errno;
                    return -1;
                }
            } else if (!S_ISDIR(st.st_mode)) {
                free(temp);
                errno = ENOTDIR;
                return -1;
            }
        }

        if (!sep) break;
        *sep = PATH_SEP;
        p = sep + 1;
    }

    free(temp);
    return 0;
}

const char* b_basename(const char* path) {
    if (!path || !*path) return ".";
    const char* last_slash = NULL;
    const char* p = path;
    while (*p) {
        if (*p == '/' || *p == '\\') last_slash = p;
        p++;
    }
    if (!last_slash) return path;
    if (!last_slash[1]) return "."; // trailing slash
    return last_slash + 1;
}

static char b_dirname_buf[4096]; // NOT thread-safe
const char* b_dirname(const char* path) {
    if (!path || !*path) return ".";
    size_t len = strlen(path);
    if (len >= sizeof(b_dirname_buf)) return ".";

    strcpy(b_dirname_buf, path);
    // Strip trailing slashes
    while (len > 0 && (b_dirname_buf[len - 1] == '/' || b_dirname_buf[len - 1] == '\\')) {
        b_dirname_buf[--len] = '\0';
    }
    if (len == 0) return ".";

    // Find last slash
    char* last = NULL;
    for (char* p = b_dirname_buf; *p; p++) {
        if (*p == '/' || *p == '\\') last = p;
    }
    if (!last) return ".";
    *last = '\0';
    return b_dirname_buf[0] ? b_dirname_buf : ".";
}

#endif // BETTER_PATH_IMPL

#endif // BETTER_PATH_H
