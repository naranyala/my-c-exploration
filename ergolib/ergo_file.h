#ifndef ERGO_FILE_H
#define ERGO_FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

// Read entire file into a malloc'd buffer (NUL-terminated).
static inline char *ergo_file_read_all(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "ergo_file: cannot open %s: %s\n", path, strerror(errno));
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char *buf = (char *)malloc(size + 1);
    if (!buf) {
        fprintf(stderr, "ergo_file: malloc failed\n");
        fclose(f);
        return NULL;
    }
    fread(buf, 1, size, f);
    buf[size] = '\0';
    fclose(f);
    return buf;
}

// Write a string to a file (overwrite).
static inline int ergo_file_write_str(const char *path, const char *s) {
    FILE *f = fopen(path, "wb");
    if (!f) {
        fprintf(stderr, "ergo_file: cannot write %s: %s\n", path, strerror(errno));
        return -1;
    }
    size_t n = fwrite(s, 1, strlen(s), f);
    fclose(f);
    return (n == strlen(s)) ? 0 : -1;
}

#endif // ERGO_FILE_H

