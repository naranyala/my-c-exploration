
#include "better_fs.h"
#include <stdio.h>

static void on_entry(const char *name, void *ud) {
    printf(" - %s\n", name);
}

int main() {
    printf("Exists? %d\n", bfs_exists("/tmp"));
    printf("Size of main.c: %ld\n", bfs_size("main.c"));

    printf("\nListing /tmp:\n");
    bfs_listdir("/tmp", on_entry, NULL);
}
