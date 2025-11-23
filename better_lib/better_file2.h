/* better_file.h */
#ifndef BETTER_FILE_H
#define BETTER_FILE_H

#include <stddef.h>
#include <stdbool.h>

// Reads entire file. Returns NULL on failure.
// Adds a null-terminator at the end (safe for text), but writes actual size to out_len.
char* bf_read_file(const char* path, size_t* out_len);

// Writes buffer to file. Returns true on success.
bool bf_write_file(const char* path, const void* data, size_t len);

#endif // BETTER_FILE_H

/* IMPLEMENTATION */
#ifdef BETTER_FILE_IMPLEMENTATION
#include <stdio.h>
#include <stdlib.h>

char* bf_read_file(const char* path, size_t* out_len) {
    FILE* f = fopen(path, "rb"); // Binary mode for exact size
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (length < 0) { fclose(f); return NULL; }

    // Allocate +1 for null terminator (safety for text files)
    char* buffer = (char*)malloc(length + 1);
    if (!buffer) { fclose(f); return NULL; }

    size_t read_size = fread(buffer, 1, length, f);
    buffer[read_size] = '\0'; // Null terminate nicely

    if (out_len) *out_len = read_size;
    fclose(f);
    return buffer;
}

bool bf_write_file(const char* path, const void* data, size_t len) {
    FILE* f = fopen(path, "wb");
    if (!f) return false;
    size_t written = fwrite(data, 1, len, f);
    fclose(f);
    return (written == len);
}
#endif
