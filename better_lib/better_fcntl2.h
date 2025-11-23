
#ifndef BETTER_FCNTL_H
#define BETTER_FCNTL_H

#include <fcntl.h>
#include <unistd.h>

// Lock file exclusively (blocking)
static inline int lock_file(int fd) {
    struct flock fl = {0};
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    return fcntl(fd, F_SETLKW, &fl);
}

// Unlock file
static inline int unlock_file(int fd) {
    struct flock fl = {0};
    fl.l_type = F_UNLCK;
    fl.l_whence = SEEK_SET;
    return fcntl(fd, F_SETLK, &fl);
}

#endif // BETTER_FCNTL_H
