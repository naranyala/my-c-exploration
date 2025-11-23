#include "ergo_err.h"
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("missing.txt", O_RDONLY);
    ERGO_TRY(fd >= 0);
    close(fd);
}

