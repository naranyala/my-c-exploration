#define ERGO_TERM_IMPLEMENTATION
#include "ergo_term.h"
#include <stdio.h>

int main() {
    printf(ERGO_T_GREEN "Green text!" ERGO_T_RESET "\n");
    ergo_term_raw(1);
    getchar();
    ergo_term_raw(0);
}

