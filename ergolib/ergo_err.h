#ifndef ERGO_ERR_H
#define ERGO_ERR_H

#include <stdio.h>
#include <errno.h>
#include <string.h>

// Print error with context
#define ERGO_ERR(ctx) \
    fprintf(stderr, "[%s] error: %s\n", ctx, strerror(errno))

// Return if condition fails
#define ERGO_TRY(expr) \
    do { if (!(expr)) { ERGO_ERR(#expr); return -1; } } while (0)

#endif // ERGO_ERR_H

