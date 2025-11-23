/* ergo_sock.h */
#pragma once
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct { int fd; } sock_t;
#define SOCK_INVALID ((sock_t){-1})

static inline void sock_close(sock_t *s) {
    if (s->fd >= 0) { close(s->fd); s->fd = -1; }
}

#define SOCK_DEFER_CLOSE(s) DEFER_ARG((void(*)(void*))sock_close, &(s))

static inline sock_t sock_tcp_listen(const char *port) {
    struct addrinfo hints = { .ai_flags = AI_PASSIVE, .ai_family = AF_UNSPEC, .ai_socktype = SOCK_STREAM };
    struct addrinfo *res, *r;
    if (getaddrinfo(NULL, port, &hints, &res)) return SOCK_INVALID;

    sock_t s = SOCK_INVALID;
    for (r = res; r; r = r->ai_next) {
        s.fd = socket(r->ai_family, r->ai_socktype, r->ai_protocol);
        if (s.fd < 0) continue;
        int yes = 1; setsockopt(s.fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
        if (bind(s.fd, r->ai_addr, r->ai_addrlen) == 0 && listen(s.fd, 64) == 0) break;
        close(s.fd); s.fd = -1;
    }
    freeaddrinfo(res);
    return s;
}

static inline sock_t sock_accept(sock_t *srv) {
    struct sockaddr_storage peer; socklen_t len = sizeof(peer);
    int fd = accept(srv->fd, (struct sockaddr*)&peer, &len);
    return (sock_t){fd};
}

static inline sock_t sock_connect(const char *host, const char *port) {
    struct addrinfo hints = { .ai_family = AF_UNSPEC, .ai_socktype = SOCK_STREAM };
    struct addrinfo *res;
    if (getaddrinfo(host, port, &hints, &res)) return SOCK_INVALID;

    sock_t s = SOCK_INVALID;
    for (struct addrinfo *r = res; r; r = r->ai_next) {
        s.fd = socket(r->ai_family, r->ai_socktype, r->ai_protocol);
        if (s.fd < 0) continue;
        if (connect(s.fd, r->ai_addr, r->ai_addrlen) == 0) break;
        close(s.fd); s.fd = -1;
    }
    freeaddrinfo(res);
    return s;
}

