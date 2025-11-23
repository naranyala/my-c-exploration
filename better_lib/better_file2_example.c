#define BETTER_FILE_IMPLEMENTATION
#include "better_file2.h"

int main() {
    // 1. Write config
    bf_write_file("config.txt", "host=localhost\nport=8080", 25);

    // 2. Slurp config
    size_t len;
    char* content = bf_read_file("config.txt", &len);
    
    if (content) {
        printf("Read %zu bytes: \n%s\n", len, content);
        free(content);
    }
    return 0;
}
