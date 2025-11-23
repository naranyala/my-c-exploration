#define ERGO_FILE_IMPLEMENTATION
#include "ergo_file2.h"
#include <stdio.h>
#include <string.h>

int main(void) {
    const char *msg = "Hello from ergo_file!\n";
    if (ergo_write_file("test.txt", msg, strlen(msg)) != 0) {
        perror("write failed");
        return 1;
    }

    size_t len;
    char *content = ergo_read_file("test.txt", &len);
    if (!content) {
        perror("read failed");
        return 1;
    }

    printf("Read %zu bytes: '%s'\n", len, content);
    free(content);

    printf("File exists? %d\n", ergo_file_exists("test.txt")); // 1

    return 0;
}
