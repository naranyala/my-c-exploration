
/* better_tempfile.h */

#ifndef BETTER_TEMPFILE_H
#define BETTER_TEMPFILE_H

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
static inline FILE *btmp_open(char *out, size_t outsz) {
    char tmp[MAX_PATH];
    GetTempPathA(MAX_PATH, tmp);
    GetTempFileNameA(tmp, "bt", 0, out);
    return fopen(out, "wb+");
}
#else
#include <unistd.h>

static inline FILE *btmp_open(char *out, size_t outsz) {
    snprintf(out, outsz, "/tmp/btXXXXXX");
    int fd = mkstemp(out);
    if (fd < 0) return NULL;
    return fdopen(fd, "w+");
}
#endif

#endif
