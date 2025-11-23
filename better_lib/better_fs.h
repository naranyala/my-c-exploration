
/* better_fs.h — directory listing + file utils */

#ifndef BETTER_FS_H
#define BETTER_FS_H

#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <unistd.h>
#endif

static inline int bfs_exists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0;
}

static inline long bfs_size(const char *path) {
    struct stat st;
    return stat(path, &st) == 0 ? st.st_size : -1;
}

static inline void bfs_listdir(const char *path,
    void (*callback)(const char *name, void *ud), void *ud)
{
#ifdef _WIN32
    char pattern[MAX_PATH];
    snprintf(pattern, sizeof(pattern), "%s\\*.*", path);

    WIN32_FIND_DATAA data;
    HANDLE h = FindFirstFileA(pattern, &data);
    if (h == INVALID_HANDLE_VALUE) return;

    do {
        if (strcmp(data.cFileName, ".") != 0 &&
            strcmp(data.cFileName, "..") != 0)
        {
            callback(data.cFileName, ud);
        }
    } while (FindNextFileA(h, &data));

    FindClose(h);
#else
    DIR *d = opendir(path);
    if (!d) return;

    struct dirent *e;
    while ((e = readdir(d))) {
        if (strcmp(e->d_name, ".") != 0 &&
            strcmp(e->d_name, "..") != 0)
        {
            callback(e->d_name, ud);
        }
    }
    closedir(d);
#endif
}

#endif
