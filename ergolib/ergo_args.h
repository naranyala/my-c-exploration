/* ergo_args.h */
#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    int argc;
    char **argv;
    int i;
} args_t;

#define args_init(ac, av) ((args_t){ac, av, 1})
#define args_end(a) ((a).i >= (a).argc)

static inline int args_flag(args_t *a, const char *name) {
    if (a->i < a->argc && !strcmp(a->argv[a->i], name)) { a->i++; return 1; }
    return 0;
}

static inline const char* args_str(args_t *a, const char *name, const char *def) {
    if (a->i+1 < a->argc && !strcmp(a->argv[a->i], name)) {
        const char *v = a->argv[a->i+1];
        a->i += 2;
        return v;
    }
    return def;
}

static inline int args_int(args_t *a, const char *name, int def) {
    if (a->i+1 < a->argc && !strcmp(a->argv[a->i], name)) {
        int v = atoi(a->argv[a->i+1]);
        a->i += 2;
        return v;
    }
    return def;
}

static inline const char* args_positional(args_t *a) {
    return a->i < a->argc ? a->argv[a->i++] : NULL;
}

static inline void args_help(const char *prog, const char *help) {
    printf("Usage: %s [options] <args>\n%s\n", prog, help);
    exit(0);
}

