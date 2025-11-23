/* Usage with ergo_defer.h */
#include "ergo_sock.h"
#include "ergo_defer2.h"

int main() {
    sock_t server = sock_tcp_listen("8080");
    DEFER_ARG(sock_close, &server);

    sock_t client = sock_accept(&server);
    DEFER_ARG(sock_close, &client);

    char buf[1024];
    ssize_t n = recv(client.fd, buf, sizeof(buf)-1, 0);
    if (n > 0) { buf[n] = 0; printf("Got: %s\n", buf); }
}

