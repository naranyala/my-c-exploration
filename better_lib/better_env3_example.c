
#include "better_env3.h"
#include <stdio.h>

int main() {
    printf("HOME=%s\n", getenv_or("HOME", "/tmp"));
    printf("DEBUG=%d\n", getenv_bool("DEBUG", 0));
}
