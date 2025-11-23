#include "ergo_flags.h"
#include <stdio.h>

enum {
    F_READ  = 1 << 0,
    F_WRITE = 1 << 1,
    F_EXEC  = 1 << 2,
};

int main() {
    ergo_flag32 f = 0;
    ergo_flag_set(f, F_READ);
    ergo_flag_set(f, F_WRITE);

    printf("read? %d\n", ergo_flag_has(f, F_READ));
    printf("exec? %d\n", ergo_flag_has(f, F_EXEC));
}

