/* ergo_net.h — simple TCP server/client helpers */
#ifndef ERGO_NET_H
#define ERGO_NET_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Create TCP listener on port. Returns socket fd or -1. */
int ergo_tcp_listen(uint16_t port);

/* Accept one connection, return fd or -1. */
int ergo_tcp_accept(int listen_fd);

/* Connect to host:port, return socket fd or -1. */
int ergo_tcp_connect(const char *host, uint16_t port);

#ifdef __cplusplus
}
#endif
#endif /* ERGO_NET_H */

#ifdef ERGO_NET_IMPLEMENTATION

#ifdef _WIN32
#error "Windows impl omitted for brevity"
#else

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

int ergo_tcp_listen(uint16_t port){
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;
    int yes = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    struct sockaddr_in a = {0};
    a.sin_family = AF_INET;
    a.sin_addr.s_addr = INADDR_ANY;
    a.sin_port = htons(port);

    if (bind(fd, (struct sockaddr*)&a, sizeof(a)) < 0) { close(fd); return -1; }
    if (listen(fd, 16) < 0) { close(fd); return -1; }
    return fd;
}

int ergo_tcp_accept(int listen_fd){
    return accept(listen_fd, NULL, NULL);
}

int ergo_tcp_connect(const char *host, uint16_t port){
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;

    struct sockaddr_in a = {0};
    a.sin_family = AF_INET;
    a.sin_port = htons(port);
    if (inet_pton(AF_INET, host, &a.sin_addr) <= 0) { close(fd); return -1; }

    if (connect(fd, (struct sockaddr*)&a, sizeof(a)) < 0) { close(fd); return -1; }
    return fd;
}

#endif
#endif /* ERGO_NET_IMPLEMENTATION */

