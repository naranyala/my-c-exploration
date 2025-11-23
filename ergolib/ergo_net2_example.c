#include "ergo_net2.h"

int main() {
    int sock = ergo_net_connect("93.184.216.34", 80); // example.com
    if (sock >= 0) {
        ergo_net_send_str(sock, "GET / HTTP/1.0\r\n\r\n");
        char buf[1024];
        ergo_net_recv_str(sock, buf, sizeof(buf));
        printf("Response:\n%s\n", buf);
        close(sock);
    }
}

