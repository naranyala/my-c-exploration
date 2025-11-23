/* ergo_opt.h — minimal ergonomic argument parser
   Supports boolean flags (-v, --help), key flags (--port 8080), and positional args.
   Very small: no dependencies.
*/
#ifndef ERGO_OPT_H
#define ERGO_OPT_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char *name; /* long name e.g. "port" */
    char short_name;  /* single char short name e.g. 'p' or 0 */
    const char *value; /* filled if provided; for boolean flags value == "1" */
    int is_required;
} ergo_opt;

typedef struct {
    ergo_opt *opts;
    size_t nopts;
    const char **pos;
    size_t npos;
} ergo_opt_state;

/* Parse argc/argv. Returns 0 on success, -1 on parse error (missing value, unknown flag) */
int ergo_opt_parse(ergo_opt_state *st, int argc, char **argv);

#ifdef __cplusplus
}
#endif
#endif /* ERGO_OPT_H */

#ifdef ERGO_OPT_IMPLEMENTATION
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
int ergo_opt_parse(ergo_opt_state *st, int argc, char **argv){
    if (!st) return -1;
    st->npos = 0;
    for (int i=1;i<argc;i++){
        char *a = argv[i];
        if (a[0] == '-') {
            if (a[1] == '-') {
                /* long --name or --name=val */
                char *eq = strchr(a, '=');
                size_t lname = eq ? (size_t)(eq - (a+2)) : strlen(a+2);
                int matched = 0;
                for (size_t j=0;j<st->nopts;j++){
                    if (strncmp(st->opts[j].name, a+2, lname)==0 && st->opts[j].name[lname] == '\0'){
                        matched = 1;
                        if (eq) {
                            st->opts[j].value = strdup(eq+1);
                        } else {
                            /* if next argv exists and doesn't start with -, take as value; else boolean true */
                            if (i+1<argc && argv[i+1][0] != '-') {
                                st->opts[j].value = strdup(argv[++i]);
                            } else {
                                st->opts[j].value = "1";
                            }
                        }
                        break;
                    }
                }
                if (!matched) return -1;
            } else {
                /* short flags cluster -abc or -p 8080 */
                for (size_t k=1; a[k]; ++k){
                    char ch = a[k];
                    int matched = 0;
                    for (size_t j=0;j<st->nopts;j++){
                        if (st->opts[j].short_name == ch){
                            matched = 1;
                            /* if next char exists in cluster, we treat rest as its value */
                            if (a[k+1]) {
                                st->opts[j].value = strdup(a + k + 1);
                                k = strlen(a)-1; /* end cluster */
                            } else if (i+1<argc && argv[i+1][0] != '-') {
                                st->opts[j].value = strdup(argv[++i]);
                            } else {
                                st->opts[j].value = "1";
                            }
                            break;
                        }
                    }
                    if (!matched) return -1;
                }
            }
        } else {
            /* positional */
            st->pos[st->npos++] = a;
        }
    }
    /* check required */
    for (size_t j=0;j<st->nopts;j++){
        if (st->opts[j].is_required && !st->opts[j].value) return -1;
    }
    return 0;
}
#endif /* ERGO_OPT_IMPLEMENTATION */

