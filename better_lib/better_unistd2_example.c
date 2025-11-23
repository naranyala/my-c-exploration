
#include "better_unistd.h"
#include <fcntl.h>

int main() {
    int fd = open("hello.txt", O_WRONLY|O_CREAT, 0644);
    safe_write(fd, "Hello\n", 6);
    close(fd);
}
