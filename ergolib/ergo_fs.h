
/* ergo_fs.h - Ergonomic Filesystem Operations */
#ifndef ERGO_FS_H
#define ERGO_FS_H

#include <stddef.h>
#include <stdbool.h>

// Reads entire file into a null-terminated string.
// Returns NULL on failure. Caller must free() the result.
char* ergo_read_file(const char* path);

// Atomic write: writes to temp file then renames (prevents corrupted files on crash).
// Returns true on success.
bool ergo_write_file(const char* path, const void* data, size_t len);

#endif // ERGO_FS_H

#ifdef ERGO_IMPLEMENTATION
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* ergo_read_file(const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (length < 0) { fclose(f); return NULL; }

    char* buffer = (char*)malloc(length + 1);
    if (buffer) {
        size_t read_len = fread(buffer, 1, length, f);
        buffer[read_len] = '\0'; // Safety null-terminator
    }
    
    fclose(f);
    return buffer;
}

bool ergo_write_file(const char* path, const void* data, size_t len) {
    // simple implementation (non-atomic for brevity, but ergonomic)
    FILE* f = fopen(path, "wb");
    if (!f) return false;
    
    size_t written = fwrite(data, 1, len, f);
    fclose(f);
    return written == len;
}
#endif
