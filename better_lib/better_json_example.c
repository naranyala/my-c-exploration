
#include "better_json.h"
#include <stdio.h>

int main() {
    const char *json = "{\"x\": 42, \"y\": true}";

    bj_token t;
    const char *p = json;

    while ((p = bj_next(p, &t)), t.type != BJ_NONE) {
        printf("token: %d", t.type);
        if (t.type == BJ_STRING || t.type == BJ_NUMBER)
            printf(" -> '%.*s'", (int)t.len, t.start);
        printf("\n");
    }
}
