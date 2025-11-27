#ifndef MINI_OPT_H
#define MINI_OPT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

/* ---------------------------------------------
   Data structures
   --------------------------------------------- */

typedef enum {
    MINIOPT_NONE,
    MINIOPT_FLAG,
    MINIOPT_INT,
    MINIOPT_FLOAT,
    MINIOPT_STRING
} miniopt_type;

typedef struct {
    const char* long_name;
    char short_name;
    miniopt_type type;
    void* out;
    int found;
    const char* help;
} miniopt_spec;

typedef struct {
    int count;
    const char** values;
} miniopt_positional;

/* ---------------------------------------------
   Core parsing
   --------------------------------------------- */

static int miniopt_parse(
    int argc, const char** argv,
    miniopt_spec* specs, int spec_count,
    miniopt_positional* positional)
{
    int pos_index = 0;

    for (int i = 1; i < argc; i++) {
        const char* arg = argv[i];

        if (arg[0] == '-') {
            int matched = 0;

            for (int s = 0; s < spec_count; s++) {
                miniopt_spec* sp = &specs[s];

                if ((sp->short_name && arg[1] == sp->short_name) ||
                    (sp->long_name && strcmp(arg+2, sp->long_name) == 0 &&
                     arg[1] == '-'))
                {
                    matched = 1;
                    sp->found = 1;

                    switch (sp->type) {
                        case MINIOPT_FLAG:
                            *(int*)sp->out = 1;
                            break;
                        case MINIOPT_INT:
                        case MINIOPT_FLOAT:
                        case MINIOPT_STRING:
                            if (i + 1 >= argc)
                                return -1;
                            if (sp->type == MINIOPT_INT)
                                *(int*)sp->out = atoi(argv[++i]);
                            else if (sp->type == MINIOPT_FLOAT)
                                *(float*)sp->out = (float)atof(argv[++i]);
                            else
                                *(const char**)sp->out = argv[++i];
                            break;
                        default: break;
                    }
                }
            }

            if (!matched) return -2;
        } else {
            /* positional argument */
            if (positional && pos_index < positional->count) {
                positional->values[pos_index++] = arg;
            } else {
                return -3;
            }
        }
    }
    return pos_index;
}

/* ---------------------------------------------
   Help generator
   --------------------------------------------- */

static void miniopt_help(const char* prog,
    miniopt_spec* specs, int spec_count)
{
    printf("Usage: %s [options] ...\n\n", prog);
    for (int s = 0; s < spec_count; s++) {
        miniopt_spec* sp = &specs[s];
        printf("  -%c, --%-12s  %s\n",
            sp->short_name,
            sp->long_name ? sp->long_name : "",
            sp->help ? sp->help : "");
    }
}

#ifdef __cplusplus
}
#endif

#endif /* MINI_OPT_H */

