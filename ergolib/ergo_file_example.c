#include "ergo_file.h"

int main() {
    char *data = ergo_file_read_all("input.txt");
    if (data) {
        printf("File contents:\n%s\n", data);
        free(data);
    }
    ergo_file_write_str("output.txt", "Hello Ergo!\n");
}

