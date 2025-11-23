
#include "better_errno.h"
#include <fcntl.h>

int main() {
    int fd = open("nonexistent.txt", O_RDONLY);
    if (fd == -1) {
        ERRNO_LOG("open failed");
    }
}
