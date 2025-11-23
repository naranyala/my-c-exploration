
#include "better_ini.h"
#include <stdio.h>

static void on_kv(const char *sec, const char *k, const char *v, void *ud) {
    printf("[%s] %s = %s\n", sec, k, v);
}

int main() {
    const char *text =
        "[server]\n"
        "port = 8080\n"
        "name = test\n";

    bini_parse(text, on_kv, NULL);
}
