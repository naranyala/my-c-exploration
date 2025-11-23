#include "better_stdio.h"
#include <string.h>

int main() {
    char* content = bread_file("main.c", NULL);
    printf("File size: %zu bytes\n", strlen(content));
    free(content);

    char* msg = basprintf("The answer is %d", 42);
    bwrite_file("out.txt", msg, strlen(msg));
    free(msg);
}
