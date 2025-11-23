/* better_socket.h */
#ifndef BETTER_SOCKET_H
#define BETTER_SOCKET_H

#include <stddef.h>
#include <stdbool.h>

// Connect to host:port (e.g., "google.com", "80"). Returns socket FD or -1.
int bs_connect(const char* host, const char* port);

// Send exactly len bytes. Handles partial sends. Returns false on error.
bool bs_send_all(int fd, const void* data, size_t len);

// Receive up to len bytes. Returns bytes read, or -1 on error, 0 on disconnect.
// Optional: timeout_sec (0 = no timeout).
int bs_recv_timeout(int fd, void* buf, size_t len, int timeout_sec);

// Close socket
void bs_close(int fd);

#endif

/* IMPLEMENTATION */
#ifdef BETTER_SOCKET_IMPLEMENTATION
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>

int bs_connect(const char* host, const char* port) {
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;     // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP

    if (getaddrinfo(host, port, &hints, &res) != 0) return -1;

    int fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (fd == -1) { freeaddrinfo(res); return -1; }

    if (connect(fd, res->ai_addr, res->ai_addrlen) == -1) {
        close(fd);
        freeaddrinfo(res);
        return -1;
    }

    freeaddrinfo(res);
    return fd;
}

bool bs_send_all(int fd, const void* data, size_t len) {
    const char* ptr = (const char*)data;
    size_t remaining = len;
    while (remaining > 0) {
        ssize_t sent = send(fd, ptr, remaining, 0);
        if (sent <= 0) return false;
        ptr += sent;
        remaining -= sent;
    }
    return true;
}

int bs_recv_timeout(int fd, void* buf, size_t len, int timeout_sec) {
    if (timeout_sec > 0) {
        struct timeval tv;
        tv.tv_sec = timeout_sec;
        tv.tv_usec = 0;
        setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    }
    return recv(fd, buf, len, 0);
}

void bs_close(int fd) {
    if (fd >= 0) close(fd);
}
#endif
