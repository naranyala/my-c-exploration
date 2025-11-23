
#include "ergo_fs2.h"
#include <stdio.h>

int main() {
    printf("Exists? %d\n", ergo_fs_exists("test.txt"));
    printf("Is dir? %d\n", ergo_fs_isdir("."));
    printf("Size: %ld\n", ergo_fs_size("test.txt"));
}
