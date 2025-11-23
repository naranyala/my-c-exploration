#include "ergo_path.h"
#include <stdio.h>

int main() {
    char dir[128];
    ergo_path_dirname("/usr/local/bin/gcc", dir, sizeof(dir));
    printf("Dir: %s\n", dir);
    printf("Base: %s\n", ergo_path_basename("/usr/local/bin/gcc"));
}

