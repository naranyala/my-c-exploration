/* Usage */
#include "ergo_vec_alt.h"
#include <stdio.h>

int main() {
    vec_t(int) numbers = vec_new(int);
    for (int i = 0; i < 100; ++i) vec_push(numbers, i*i);

    vec_t(const char*) words = vec_new(const char*);
    vec_push(words, "hello");
    vec_push(words, "world");

    printf("squares: %d %d %d\n", numbers.data[0], numbers.data[1], numbers.data[99]);
    printf("words: %s %s\n", words.data[0], words.data[1]);

    vec_free(numbers);
    vec_free(words);
}

