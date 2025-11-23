
#include "better_env.h"
#include <stdio.h>

int main() {
    benv_set("FOO", "bar");
    printf("FOO = %s\n", benv_get("FOO", "<none>"));

    benv_unset("FOO");
    printf("FOO = %s\n", benv_get("FOO", "<none>"));
}
