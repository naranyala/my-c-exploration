/* Usage */
#include "ergo_file3.h"

int main() {
    size_t sz;
    char *content = file_read_all("main.c", &sz);
    if (content) {
        printf("Read %zu bytes\n", sz);
        free(content);
    }

    file_write_all("out.txt", "Hello ergo!", 11);

    void *map = file_map("out.txt", &sz);
    if (map) {
        printf("Mapped: %.*s\n", (int)sz, (char*)map);
        file_unmap(map, sz);
    }
}

