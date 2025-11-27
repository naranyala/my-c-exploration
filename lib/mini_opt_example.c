#define MINI_OPT_IMPLEMENTATION
#include "mini_opt.h"
#include <stdio.h>

int main(int argc, const char** argv) {
    int verbose = 0;
    int count = 0;
    const char* name = NULL;

    const char* pos_vals[2];
    miniopt_positional pos = {2, pos_vals};

    miniopt_spec opts[] = {
        {"verbose", 'v', MINIOPT_FLAG,   &verbose, 0, "Enable verbose output"},
        {"count",   'c', MINIOPT_INT,    &count,   0, "Repeat count"},
        {"name",    'n', MINIOPT_STRING, &name,    0, "Custom name"},
    };

    int got = miniopt_parse(argc, argv, opts, 3, &pos);

    if (got < 0) {
        miniopt_help(argv[0], opts, 3);
        return 1;
    }

    printf("verbose = %d\n", verbose);
    printf("count   = %d\n", count);
    printf("name    = %s\n", name);
    printf("pos1    = %s\n", pos_vals[0]);
    printf("pos2    = %s\n", pos_vals[1]);

    return 0;
}

