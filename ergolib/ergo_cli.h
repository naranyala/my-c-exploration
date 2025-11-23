#ifndef ERGO_CLI_H
#define ERGO_CLI_H

#include <stdio.h>
#include <string.h>

// Simple flag checker: returns 1 if flag present
static inline int ergo_cli_has(int argc, char **argv, const char *flag) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], flag) == 0) return 1;
    }
    return 0;
}

// Get option value: returns pointer or NULL
static inline const char *ergo_cli_get(int argc, char **argv, const char *opt) {
    for (int i = 1; i < argc - 1; i++) {
        if (strcmp(argv[i], opt) == 0) return argv[i+1];
    }
    return NULL;
}

#endif // ERGO_CLI_H

