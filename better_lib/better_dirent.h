/* better_dirent.h v1.2 */
#ifndef BETTER_DIRENT_H
#define BETTER_DIRENT_H
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

static inline int bmkdir_p(const char* path, mode_t mode) {
    char tmp[1024];
    snprintf(tmp, sizeof(tmp), "%s", path);
    for (char* p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            mkdir(tmp, mode);
            *p = '/';
        }
    }
    return mkdir(tmp, mode);
}

static inline int brm_rf(const char* path) {
    DIR* d = opendir(path);
    if (!d) return -1;
    struct dirent* de;
    int r = 0;
    while ((de = readdir(d))) {
        if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, "..")) continue;
        char sub[1024];
        snprintf(sub, sizeof(sub), "%s/%s", path, de->d_name);
        if (de->d_type == DT_DIR) {
            if (brm_rf(sub)) r = -1;
        } else {
            if (unlink(sub)) r = -1;
        }
    }
    closedir(d);
    return r ? r : rmdir(path);
}

#endif
