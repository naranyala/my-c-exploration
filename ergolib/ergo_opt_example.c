#define ERGO_OPT_IMPLEMENTATION
#include "ergo_opt.h"
#include <stdio.h>

int main(int argc, char **argv){
    ergo_opt opts[] = {
        { "help", 'h', NULL, 0 },
        { "port", 'p', NULL, 1 }, /* required */
        { "verbose", 'v', NULL, 0 },
    };
    const char *posbuf[16];
    ergo_opt_state st = { opts, 3, posbuf, 0 };
    if (ergo_opt_parse(&st, argc, argv) != 0){
        fprintf(stderr, "parse error or missing required\n");
        return 2;
    }
    if (opts[0].value) { puts("help requested"); return 0; }
    printf("port=%s verbose=%s\n", opts[1].value, opts[2].value ? "yes" : "no");
    for (size_t i=0;i<st.npos;i++) printf("pos[%zu] = %s\n", i, st.pos[i]);
    return 0;
}

