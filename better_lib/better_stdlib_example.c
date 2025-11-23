#include "better_stdlib.h"
int main() {
    char* s = bstrdup("never worry about NULL again");
    char* buf = bmalloc(1024);
    // ...
    free(s); free(buf);
}
