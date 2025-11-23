
#include "better_path.h"
#include <stdio.h>

int main() {
    const char *p = "/home/me/file.txt";
    printf("basename = %s\n", bpath_basename(p));
    printf("ext      = %s\n", bpath_ext(p));

    char buf[256];
    bpath_join(buf, sizeof(buf), "/tmp", "log.txt");
    printf("join     = %s\n", buf);
}
