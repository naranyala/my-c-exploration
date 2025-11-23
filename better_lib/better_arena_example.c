
#include "better_arena.h"
#include <stdio.h>

int main() {
    barena a;
    barena_init(&a, 1024);

    int *x = (int*)barena_alloc(&a, sizeof(int));
    *x = 13;

    char *s = (char*)barena_alloc(&a, 6);
    memcpy(s, "hello", 6);

    printf("%d %s\n", *x, s);

    barena_free(&a);
}
