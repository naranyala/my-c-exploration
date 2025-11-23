/* ergo_net.h - Simplified TCP Sockets (POSIX) */
#ifndef ERGO_NET_H
#define ERGO_NET_H

// Creates a client socket connected to host:port. Returns file descriptor or -1.
int ergo_dial(const char* host, int port);

// Creates a server socket bound to port. Returns file descriptor or -1.
int ergo_serve(int port);

#endif // ERGO_NET_H

#ifdef ERGO_IMPLEMENTATION
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h> // inet_addr
#include <netdb.h>     // gethostbyname
#include <unistd.h>

int ergo_dial(const char* host, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) return -1;

    struct hostent* he = gethostbyname(host);
    if (!he) { close(sock); return -1; }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    memcpy(&server.sin_addr, he->h_addr_list[0], he->h_length);

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        close(sock);
        return -1;
    }
    return sock;
}

int ergo_serve(int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) return -1;

    // Allow immediate reuse of the port (SO_REUSEADDR)
    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    if (bind(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        close(sock); return -1;
    }

    listen(sock, 5); // Backlog of 5
    return sock;
}
#endif
