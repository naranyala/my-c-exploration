
/* ergo_fs.h — single-header ergonomic filesystem helpers
   Usage: include normally. In one C file define ERGO_FS_IMPLEMENTATION and include to get implementation.
*/
#ifndef ERGO_FS_H
#define ERGO_FS_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Read whole file into malloc'd buffer (NUL-terminated). Returns ptr and sets out_size.
   On error returns NULL and out_size=0. Free with free(). */
char *ergo_read_file(const char *path, size_t *out_size);

/* Write whole buffer to file, atomically if possible. Returns 0 on success, -1 on error. */
int ergo_write_file(const char *path, const void *data, size_t size);

/* Join up to two path segments into out buffer (out_size bytes). Returns 0 on success, -1 on truncation. */
int ergo_path_join(char *out, size_t out_size, const char *a, const char *b);

/* Ensure directory exists (mkdir -p semantics). Returns 0 on success, -1 on error. */
int ergo_mkdir_p(const char *path);

#ifdef __cplusplus
}
#endif
#endif /* ERGO_FS_H */

#ifdef ERGO_FS_IMPLEMENTATION
/* Implementation */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#define PATH_SEP '\\'
#else
#include <unistd.h>
#define PATH_SEP '/'
#include <dirent.h>
#endif

char *ergo_read_file(const char *path, size_t *out_size){
    if (out_size) *out_size = 0;
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    if (fseek(f, 0, SEEK_END) != 0){ fclose(f); return NULL; }
    long len = ftell(f);
    if (len < 0){ fclose(f); return NULL; }
    rewind(f);
    char *buf = malloc((size_t)len + 1);
    if (!buf){ fclose(f); return NULL; }
    if (fread(buf, 1, (size_t)len, f) != (size_t)len){ free(buf); fclose(f); return NULL; }
    buf[len] = '\0';
    if (out_size) *out_size = (size_t)len;
    fclose(f);
    return buf;
}

int ergo_write_file(const char *path, const void *data, size_t size){
    /* Simple write (not fully atomic) */
    FILE *f = fopen(path, "wb");
    if (!f) return -1;
    size_t w = fwrite(data, 1, size, f);
    int ok = (w == size) ? 0 : -1;
    fclose(f);
    return ok;
}

int ergo_path_join(char *out, size_t out_size, const char *a, const char *b){
    if (!a || !*a) {
        if (!b) { if (out_size>0) out[0]='\0'; return 0; }
        if (strlen(b) + 1 > out_size) return -1;
        strcpy(out, b);
        return 0;
    }
    if (!b || !*b) {
        if (strlen(a) + 1 > out_size) return -1;
        strcpy(out, a);
        return 0;
    }
    size_t la = strlen(a);
    int need_sep = (a[la-1] != PATH_SEP);
    size_t total = la + (need_sep ? 1 : 0) + strlen(b) + 1;
    if (total > out_size) return -1;
    strcpy(out, a);
    if (need_sep) {
        out[la] = PATH_SEP; out[la+1] = '\0';
    }
    strcat(out, b);
    return 0;
}

static int ergo__mkdir_single(const char *p){
#ifdef _WIN32
    return _mkdir(p);
#else
    return mkdir(p, 0755);
#endif
}

int ergo_mkdir_p(const char *path){
    if (!path || !*path) return -1;
    char *dup = strdup(path);
    if (!dup) return -1;
    for (char *s = dup; *s; ++s){
        if (*s == '/' || *s == '\\') {
            char tmp = *s;
            *s = '\0';
            if (strlen(dup) > 0) {
                struct stat st;
                if (stat(dup, &st) != 0) {
                    if (ergo__mkdir_single(dup) != 0) {
                        free(dup); return -1;
                    }
                } else if (!S_ISDIR(st.st_mode)) {
                    free(dup); return -1;
                }
            }
            *s = tmp;
        }
    }
    /* final */
    struct stat st;
    if (stat(dup, &st) != 0){
        if (ergo__mkdir_single(dup) != 0) { free(dup); return -1; }
    } else if (!S_ISDIR(st.st_mode)){ free(dup); return -1; }
    free(dup);
    return 0;
}
#endif /* ERGO_FS_IMPLEMENTATION */
