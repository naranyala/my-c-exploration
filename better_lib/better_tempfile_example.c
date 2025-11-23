
#include "better_tempfile.h"
#include <stdio.h>

int main() {
    char path[256];
    FILE *f = btmp_open(path, sizeof(path));

    if (!f) return 1;

    fprintf(f, "hello temp!\n");
    rewind(f);

    char buf[64];
    fgets(buf, sizeof(buf), f);
    printf("Temp content: %s\n", buf);

    fclose(f);
    printf("Temporary file: %s\n", path);
}
