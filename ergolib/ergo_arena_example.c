/* Usage */
#include "ergo_arena.h"
#include <stdio.h>

int main() {
    ergo_arena_t arena = {0};
    arena_init(&arena);

    char *hello = arena_strdup(&arena, "Hello arena!");
    int *numbers = arena_new(&arena, int[1000]);
    for (int i = 0; i < 1000; i++) numbers[i] = i*i;

    printf("%s\n", hello);  // works instantly, no free needed

    arena_reset(&arena);    // instantly reclaim everything
    // or arena_free(&arena); at the end of program
}

