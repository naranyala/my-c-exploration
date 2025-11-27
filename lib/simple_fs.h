#ifndef SIMPLE_FS_H
#define SIMPLE_FS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* --------------------------------------------------
   Read whole file
   -------------------------------------------------- */
static int fs_read_all(const char* path, void* buffer, size_t* size_out) {
    FILE* f = fopen(path, "rb");
    if (!f) return -1;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (size_out) *size_out = (size_t)size;

    if (!buffer) {
        fclose(f);
        return 0;
    }

    size_t read = fread(buffer, 1, size, f);
    fclose(f);

    return (read == (size_t)size) ? 0 : -2;
}

/* --------------------------------------------------
   Write whole file
   -------------------------------------------------- */
static int fs_write_all(const char* path, const void* data, size_t size) {
    FILE* f = fopen(path, "wb");
    if (!f) return -1;

    size_t written = fwrite(data, 1, size, f);
    fclose(f);
    return (written == size) ? 0 : -2;
}

/* --------------------------------------------------
   Exists?
   -------------------------------------------------- */
static int fs_exists(const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) return 0;
    fclose(f);
    return 1;
}

/* --------------------------------------------------
   File size
   -------------------------------------------------- */
static long fs_size(const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) return -1;
    fseek(f, 0, SEEK_END);
    long s = ftell(f);
    fclose(f);
    return s;
}

/* --------------------------------------------------
   Directory listing (cross-platform)
   -------------------------------------------------- */

#if defined(_WIN32)

#include <windows.h>

static int fs_list_dir(const char* path,
                       void (*callback)(const char* name, int is_dir))
{
    char search[260];
    snprintf(search, sizeof(search), "%s\\*.*", path);

    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA(search, &fd);
    if (h == INVALID_HANDLE_VALUE) return -1;

    do {
        if (strcmp(fd.cFileName, ".") == 0 ||
            strcmp(fd.cFileName, "..") == 0)
            continue;

        int is_dir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        callback(fd.cFileName, is_dir);

    } while (FindNextFileA(h, &fd));

    FindClose(h);
    return 0;
}

#else /* POSIX */

#include <dirent.h>
#include <sys/stat.h>

static int fs_list_dir(const char* path,
                       void (*callback)(const char* name, int is_dir))
{
    DIR* d = opendir(path);
    if (!d) return -1;

    struct dirent* ent;

    while ((ent = readdir(d)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 ||
            strcmp(ent->d_name, "..") == 0)
            continue;

        char tmp[512];
        snprintf(tmp, sizeof(tmp), "%s/%s", path, ent->d_name);

        struct stat st;
        stat(tmp, &st);

        int is_dir = S_ISDIR(st.st_mode);
        callback(ent->d_name, is_dir);
    }

    closedir(d);
    return 0;
}

#endif

#ifdef __cplusplus
}
#endif
#endif /* SIMPLE_FS_H */

