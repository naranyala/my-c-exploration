#ifndef ERGO_NET_H
#define ERGO_NET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

// Create a TCP client socket and connect
static inline int ergo_net_connect(const char *host, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket"); return -1; }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, host, &addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sock);
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sock);
        return -1;
    }
    return sock;
}

// Send a string over a socket
static inline int ergo_net_send_str(int sock, const char *s) {
    size_t len = strlen(s);
    return (send(sock, s, len, 0) == (ssize_t)len) ? 0 : -1;
}

// Receive into buffer (NUL-terminated)
static inline int ergo_net_recv_str(int sock, char *buf, size_t maxlen) {
    ssize_t n = recv(sock, buf, maxlen - 1, 0);
    if (n < 0) return -1;
    buf[n] = '\0';
    return (int)n;
}

#endif // ERGO_NET_H

