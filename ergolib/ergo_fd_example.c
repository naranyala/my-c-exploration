#define ERGO_FD_IMPLEMENTATION
#include "ergo_fd.h"
#include <stdio.h>

int main() {
    ergo_fd_set_nonblock(0); /* stdin non-blocking */
    return 0;
}

