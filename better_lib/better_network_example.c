
#include "better_network.h"

int main() {
    int server = tcp_server(8080);
    if (server >= 0) printf("Server listening on 8080\n");
}
