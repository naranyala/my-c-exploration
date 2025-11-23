#include "ergo_caps.h"
#include <stdio.h>

int main() {
    printf("fork available: %d\n", ergo_caps_has_fork());
    printf("symlink available: %d\n", ergo_caps_has_symlink());
    printf("supports color: %d\n", ergo_caps_has_color_terminal());
}

