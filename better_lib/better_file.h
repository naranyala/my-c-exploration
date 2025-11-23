// better_file.h
#ifndef BETTER_FILE_H
#define BETTER_FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#ifdef __cplusplus
extern "C" {
#endif

// Read entire file into a malloc'd buffer.
// Returns NULL on error. Sets *size_out to file size (excluding null terminator).
// The returned buffer is null-terminated (extra byte allocated).
char* b_file_read_all(const char* path, size_t* size_out);

// Check if file exists and is readable.
int b_file_exists(const char* path);

#ifdef __cplusplus
}
#endif

#ifdef BETTER_FILE_IMPL

#include <errno.h>

char* b_file_read_all(const char* path, size_t* size_out) {
    if (!path) {
        if (size_out) *size_out = 0;
        return NULL;
    }

    struct stat st;
    if (stat(path, &st) != 0) return NULL;
    if (st.st_size < 0) return NULL;

    size_t size = (size_t)st.st_size;
    FILE* f = fopen(path, "rb");
    if (!f) return NULL;

    char* buffer = (char*)malloc(size + 1);
    if (!buffer) {
        fclose(f);
        return NULL;
    }

    size_t n = fread(buffer, 1, size, f);
    int err = ferror(f);
    fclose(f);

    if (err || n != size) {
        free(buffer);
        return NULL;
    }

    buffer[size] = '\0'; // null-terminate for text safety
    if (size_out) *size_out = size;
    return buffer;
}

int b_file_exists(const char* path) {
    if (!path) return 0;
    struct stat st;
    return (stat(path, &st) == 0);
}

#endif // BETTER_FILE_IMPL

#endif // BETTER_FILE_H
