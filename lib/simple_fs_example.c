#include "simple_fs.h"
#include <stdio.h>
#include <stdlib.h>

void on_entry(const char* name, int is_dir) {
    printf("%s %s\n", is_dir ? "[DIR]" : "[FILE]", name);
}

int main() {
    printf("Exists? %d\n", fs_exists("test.txt"));

    const char* msg = "Hello from fs!";
    fs_write_all("out.txt", msg, strlen(msg));

    size_t sz;
    fs_read_all("out.txt", NULL, &sz);
    char* buf = malloc(sz + 1);
    fs_read_all("out.txt", buf, &sz);
    buf[sz] = 0;
    printf("Contents: %s\n", buf);

    printf("\nDirectory listing:\n");
    fs_list_dir(".", on_entry);

    free(buf);
}

