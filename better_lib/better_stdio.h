/* better_stdio.h v1.1 - Safe & convenient stdio wrappers */
#ifndef BETTER_STDIO_H
#define BETTER_STDIO_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

static inline FILE* bfopen(const char* path, const char* mode) {
    FILE* f = fopen(path, mode);
    if (!f) { perror(path); abort(); }
    return f;
}

static inline char* bread_file(const char* path, size_t* out_size) {
    FILE* f = bfopen(path, "rb");
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    if (size < 0) { fclose(f); abort(); }
    fseek(f, 0, SEEK_SET);
    char* buf = (char*)malloc((size_t)size + 1);
    if (!buf) { fclose(f); abort(); }
    if (fread(buf, 1, (size_t)size, f) != (size_t)size) {
        free(buf); fclose(f); perror("fread"); abort();
    }
    buf[size] = '\0';
    if (out_size) *out_size = (size_t)size;
    fclose(f);
    return buf;
}

static inline void bwrite_file(const char* path, const char* data, size_t size) {
    FILE* f = bfopen(path, "wb");
    if (fwrite(data, 1, size, f) != size) {
        fclose(f); perror("fwrite"); abort();
    }
    fclose(f);
}

/* asprintf_checked - never fails */
static inline char* basprintf(const char* fmt, ...) {
    va_list ap, ap2;
    va_start(ap, fmt);
    va_copy(ap2, ap);
    int n = vsnprintf(NULL, 0, fmt, ap);
    va_end(ap);
    if (n < 0) abort();
    char* s = (char*)malloc((size_t)n + 1);
    if (!s) abort();
    vsnprintf(s, (size_t)n + 1, fmt, ap2);
    va_end(ap2);
    return s;
}

#endif /* BETTER_STDIO_H */
