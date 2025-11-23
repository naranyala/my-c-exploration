#ifndef ERGO_DBG_H
#define ERGO_DBG_H

#include <stdio.h>

#define ergo_dbg(fmt, ...) \
    fprintf(stderr, "[%s:%d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#endif

