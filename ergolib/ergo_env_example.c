#include "ergo_env.h"
#include <stdio.h>

int main() {
    printf("HOME=%s\n", ergo_env_get("HOME", "/tmp"));
    ergo_env_set("FOO", "bar", 1);
    printf("FOO=%s\n", ergo_env_get("FOO", "none"));
}

