
#include "better_fcntl2.h"
#include <stdio.h>
#include <fcntl.h>

int main() {
    int fd = open("data.txt", O_RDWR|O_CREAT, 0644);
    lock_file(fd);
    write(fd, "Locked write\n", 13);
    unlock_file(fd);
    close(fd);
}
