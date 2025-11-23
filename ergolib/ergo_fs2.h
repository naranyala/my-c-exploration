#ifndef ERGO_FS_H
#define ERGO_FS_H

#include <sys/stat.h>
#include <unistd.h>

// Check if path exists
static inline int ergo_fs_exists(const char *path) {
    return access(path, F_OK) == 0;
}

// Check if path is directory
static inline int ergo_fs_isdir(const char *path) {
    struct stat st;
    return (stat(path, &st) == 0 && S_ISDIR(st.st_mode));
}

// Get file size
static inline long ergo_fs_size(const char *path) {
    struct stat st;
    return (stat(path, &st) == 0) ? st.st_size : -1;
}

#endif // ERGO_FS_H

