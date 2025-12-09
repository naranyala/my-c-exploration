// proper_opt.h
#ifndef PROPER_OPT_H
#define PROPER_OPT_H

#include <stddef.h>

// Usage: call proper_opt_init(), then loop with proper_opt_next()
typedef struct {
  int argc;
  char **argv;
  int idx;
  const char *current_opt;
  const char *current_arg;
} proper_opt_t;

static inline void proper_opt_init(proper_opt_t *ctx, int argc, char **argv) {
  ctx->argc = argc;
  ctx->argv = argv;
  ctx->idx = 1; // skip program name
  ctx->current_opt = NULL;
  ctx->current_arg = NULL;
}

// Returns 1 if option found, 0 if done, -1 on error (missing arg)
static inline int proper_opt_next(proper_opt_t *ctx, const char *short_opts) {
  if (ctx->idx >= ctx->argc)
    return 0;

  const char *arg = ctx->argv[ctx->idx++];
  if (arg[0] != '-' || arg[1] == '\0') {
    // Non-option argument → treat as positional (stop parsing)
    ctx->current_opt = NULL;
    ctx->current_arg = arg;
    return 0;
  }

  if (arg[1] == '-') {
    // Long option or -- terminator
    if (arg[2] == '\0')
      return 0; // --
    ctx->current_opt = arg + 2;
    ctx->current_arg = NULL;
    return 1;
  }

  // Short option (e.g., -v, -f FILE)
  ctx->current_opt = arg + 1;
  ctx->current_arg = NULL;

  // Check if this option requires an argument (listed in short_opts)
  const char *p = short_opts;
  while (*p) {
    if (*p == ':') {
      p++;
      continue;
    }
    if (*p == ctx->current_opt[0]) {
      if (p[1] == ':') {
        // Takes argument
        if (ctx->idx < ctx->argc) {
          ctx->current_arg = ctx->argv[ctx->idx++];
        } else {
          return -1; // missing argument
        }
      }
      break;
    }
    p++;
  }

  return 1;
}

#endif // PROPER_OPT_H

// example
//
// proper_opt_t opt;
// proper_opt_init(&opt, argc, argv);
// while (1) {
//     int r = proper_opt_next(&opt, "vf:");
//     if (r == 0) break;
//     if (r == -1) { /* error */ }
//     if (opt.current_opt[0] == 'v') { verbose = 1; }
//     if (opt.current_opt[0] == 'f') { filename = opt.current_arg; }
// }
