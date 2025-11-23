
#ifndef BETTER_ERRNO_H
#define BETTER_ERRNO_H

#include <errno.h>
#include <string.h>
#include <stdio.h>

// Convert errno to string
static inline const char* errno_str(void) {
    return strerror(errno);
}

// Print error with context
#define ERRNO_LOG(ctx) \
    fprintf(stderr, "[ERR] %s: %s\n", (ctx), strerror(errno))

#endif // BETTER_ERRNO_H
