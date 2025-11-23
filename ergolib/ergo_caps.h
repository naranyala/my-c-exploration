#ifndef ERGO_CAPS_H
#define ERGO_CAPS_H

#include <stdbool.h>
#include <string.h>
#include <unistd.h>

static inline bool ergo_caps_has_fork(void) {
#ifdef _POSIX_VERSION
    return 1;
#else
    return 0;
#endif
}

static inline bool ergo_caps_has_symlink(void) {
#ifdef _POSIX_VERSION
    return 1;
#else
    return 0;
#endif
}

static inline bool ergo_caps_has_large_files(void) {
#ifdef _LARGEFILE_SOURCE
    return 1;
#else
    return 0;
#endif
}

static inline bool ergo_caps_has_color_terminal(void) {
    const char *t = getenv("TERM");
    if (!t) return false;
    return strstr(t, "xterm") || strstr(t, "color");
}

#endif

