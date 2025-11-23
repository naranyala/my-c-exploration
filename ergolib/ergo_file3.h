/* ergo_file.h */
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

static inline char* file_read_all(const char *path, size_t *out_size) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = malloc(size + 1);
    fread(buf, 1, size, f);
    fclose(f);
    buf[size] = '\0';
    if (out_size) *out_size = size;
    return buf;
}

static inline int file_write_all(const char *path, const void *data, size_t size) {
    FILE *f = fopen(path, "wb");
    if (!f) return -1;
    int ok = fwrite(data, 1, size, f) == size;
    fclose(f);
    return ok ? 0 : -1;
}

static inline void* file_map(const char *path, size_t *size) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) return NULL;
    struct stat st;
    if (fstat(fd, &st) < 0) { close(fd); return NULL; }
    void *p = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
    if (p == MAP_FAILED) return NULL;
    if (size) *size = st.st_size;
    return p;
}

#define file_unmap(p, sz) munmap((p), (sz))

