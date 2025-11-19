
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons(8080);
  inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

  if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == 0) {
    char *msg = "Hello Server";
    send(sock, msg, strlen(msg), 0);
  }

  close(sock);
  return 0;
}
