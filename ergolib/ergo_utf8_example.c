/* Usage */
#include "ergo_utf8.h"
#include <stdio.h>
int main() {
    const char *s = "Hello 世界 🌍";
    printf("Code points: %d\n", utf8_len(s));
}

