#define BETTER_SOCKET_IMPLEMENTATION
#include "better_socket.h"
#include <stdio.h>

int main() {
    int fd = bs_connect("example.com", "80");
    if (fd < 0) return 1;

    char req[] = "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n";
    bs_send_all(fd, req, sizeof(req)-1);

    char response[1024];
    int bytes = bs_recv_timeout(fd, response, sizeof(response)-1, 2); // 2s timeout
    if (bytes > 0) {
        response[bytes] = '\0';
        printf("Received:\n%s\n", response);
    }
    bs_close(fd);
    return 0;
}
