/* Usage */
#include "ergo_str2.h"

int main() {
    estr_t s = estr_new("Hello");
    estr_append(&s, ", world!");
    estr_appendf(&s, " Count: %d", 42);
    estr_cat(&s, " | ", "easy", " peasy", NULL);
    printf("%s\n", s.data);  // Hello, world! Count: 42 | easy peasy
    estr_free(s);
}

