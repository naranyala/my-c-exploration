/* ergo_file.h
 *
 * Simple, safe file utilities.
 *
 * Define ERGO_FILE_IMPLEMENTATION in ONE C file.
 */

#ifndef ERGO_FILE_H
#define ERGO_FILE_H

#include <stddef.h>  /* size_t */

#ifdef __cplusplus
extern "C" {
#endif

/* Reads entire file into a malloc'd buffer. Sets *out_len to file size.
 * Returns NULL on error (check errno). Caller must free().
 */
char *ergo_read_file(const char *path, size_t *out_len);

/* Writes `len` bytes from `data` to file at `path`.
 * Returns 0 on success, -1 on error (errno set).
 */
int ergo_write_file(const char *path, const void *data, size_t len);

/* Returns 1 if file exists and is readable, 0 otherwise. */
int ergo_file_exists(const char *path);

#ifdef __cplusplus
}
#endif

#endif /* ERGO_FILE_H */

#ifdef ERGO_FILE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>

char *ergo_read_file(const char *path, size_t *out_len) {
    if (!path) {
        errno = EINVAL;
        return NULL;
    }

    FILE *f = fopen(path, "rb");
    if (!f) return NULL;

    if (fseek(f, 0, SEEK_END) != 0) goto fail;
    long size = ftell(f);
    if (size < 0) goto fail;
    if (fseek(f, 0, SEEK_SET) != 0) goto fail;

    char *buf = (char *)malloc((size_t)size + 1);
    if (!buf) goto fail;

    size_t nread = fread(buf, 1, (size_t)size, f);
    if (nread != (size_t)size) {
        free(buf);
        fclose(f);
        errno = EIO;
        return NULL;
    }
    buf[nread] = '\0';  // convenient for text

    fclose(f);
    if (out_len) *out_len = (size_t)size;
    return buf;

fail:
    fclose(f);
    return NULL;
}

int ergo_write_file(const char *path, const void *data, size_t len) {
    if (!path) {
        errno = EINVAL;
        return -1;
    }
    FILE *f = fopen(path, "wb");
    if (!f) return -1;

    size_t nwritten = fwrite(data, 1, len, f);
    int err = ferror(f);
    fclose(f);

    if (err || nwritten != len) {
        errno = err ? err : EIO;
        return -1;
    }
    return 0;
}

int ergo_file_exists(const char *path) {
    if (!path) return 0;
    struct stat st;
    return (stat(path, &st) == 0);
}

#endif /* ERGO_FILE_IMPLEMENTATION */
