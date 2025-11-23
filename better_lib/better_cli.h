
#ifndef BETTER_CLI_H
#define BETTER_CLI_H

#include <string.h>
#include <stdio.h>

// Get option value by flag (e.g. --name)
static inline const char* cli_getopt(int argc, char **argv, const char *flag) {
    for (int i = 1; i < argc - 1; i++) {
        if (strcmp(argv[i], flag) == 0) {
            return argv[i+1];
        }
    }
    return NULL;
}

// Check if flag exists (boolean option)
static inline int cli_hasflag(int argc, char **argv, const char *flag) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], flag) == 0) return 1;
    }
    return 0;
}

#endif // BETTER_CLI_H
