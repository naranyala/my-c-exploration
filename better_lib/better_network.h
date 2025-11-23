
#ifndef BETTER_NETWORK_H
#define BETTER_NETWORK_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>

// Create TCP server socket
static inline int tcp_server(int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket"); return -1; }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind"); close(sock); return -1;
    }
    if (listen(sock, 5) < 0) {
        perror("listen"); close(sock); return -1;
    }
    return sock;
}

#endif // BETTER_NETWORK_H
