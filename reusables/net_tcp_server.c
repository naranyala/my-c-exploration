/**
 * Basic TCP server template
 */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

int create_server_socket(int port) {
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("Socket creation failed");
    return -1;
  }

  // Allow port reuse
  int opt = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port);

  if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    perror("Bind failed");
    close(sockfd);
    return -1;
  }

  if (listen(sockfd, MAX_CLIENTS) < 0) {
    perror("Listen failed");
    close(sockfd);
    return -1;
  }

  return sockfd;
}

void handle_client(int client_sock) {
  char buffer[BUFFER_SIZE];

  while (1) {
    int bytes_received = recv(client_sock, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received <= 0) {
      break;
    }

    buffer[bytes_received] = '\0';
    printf("Received: %s\n", buffer);

    // Echo back
    send(client_sock, buffer, bytes_received, 0);

    if (strcmp(buffer, "quit") == 0) {
      break;
    }
  }

  close(client_sock);
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s <port>\n", argv[0]);
    return 1;
  }

  int port = atoi(argv[1]);
  int server_sock = create_server_socket(port);

  if (server_sock < 0) {
    return 1;
  }

  printf("Server listening on port %d\n", port);

  while (1) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    int client_sock =
        accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
    if (client_sock < 0) {
      perror("Accept failed");
      continue;
    }

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    printf("Client connected: %s:%d\n", client_ip, ntohs(client_addr.sin_port));

    handle_client(client_sock);
    printf("Client disconnected: %s:%d\n", client_ip,
           ntohs(client_addr.sin_port));
  }

  close(server_sock);
  return 0;
}
