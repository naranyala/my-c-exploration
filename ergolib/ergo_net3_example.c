#define ERGO_NET_IMPLEMENTATION
#include "ergo_net3.h"
#include <stdio.h>

int main() {
    int srv = ergo_tcp_listen(9000);
    if (srv < 0) return 1;
    printf("listening...\n");
    int fd = ergo_tcp_accept(srv);
    printf("client accepted fd=%d\n", fd);
    return 0;
}

