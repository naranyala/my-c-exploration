/**
 * Basic TCP client template
 */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int create_client_socket(const char *server_ip, int port) {
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("Socket creation failed");
    return -1;
  }

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);

  if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
    perror("Invalid address");
    close(sockfd);
    return -1;
  }

  if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    perror("Connection failed");
    close(sockfd);
    return -1;
  }

  return sockfd;
}

void client_loop(int sockfd) {
  char buffer[BUFFER_SIZE];

  while (1) {
    printf("Enter message (or 'quit' to exit): ");
    fgets(buffer, BUFFER_SIZE, stdin);

    // Remove newline
    buffer[strcspn(buffer, "\n")] = 0;

    if (strcmp(buffer, "quit") == 0) {
      break;
    }

    // Send message
    send(sockfd, buffer, strlen(buffer), 0);

    // Receive response
    int bytes_received = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received > 0) {
      buffer[bytes_received] = '\0';
      printf("Server response: %s\n", buffer);
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Usage: %s <server_ip> <port>\n", argv[0]);
    return 1;
  }

  const char *server_ip = argv[1];
  int port = atoi(argv[2]);

  int sockfd = create_client_socket(server_ip, port);
  if (sockfd < 0) {
    return 1;
  }

  printf("Connected to server %s:%d\n", server_ip, port);
  client_loop(sockfd);

  close(sockfd);
  return 0;
}
