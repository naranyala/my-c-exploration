#define ERGO_FS_IMPLEMENTATION
#include "ergo_fs3.h"
#include <stdio.h>

int main(void){
    size_t n;
    char *s = ergo_read_file("test.txt", &n);
    if (s){
        printf("file len=%zu\n%s\n", n, s);
        free(s);
    } else {
        puts("couldn't read file");
    }

    const char *content = "hello\n";
    ergo_write_file("out.txt", content, strlen(content));

    char path[256];
    ergo_path_join(path, sizeof(path), "dir/sub", "file.txt");
    puts(path); /* prints "dir/sub/file.txt" */

    ergo_mkdir_p("dir/sub");
    return 0;
}

