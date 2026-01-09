#include "client_network.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

int client_network_connect(const char *host, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, host, &addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sock);
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sock);
        return -1;
    }

    return sock;
}

int client_network_send_str(int sock, const char *buffer, size_t length) {
  if (!buffer || length == 0) return -1;
  return send(sock, buffer, length, 0) == (ssize_t)length ? 0 : -1;
}

/* jednoduché správy bez payloadu */
int client_network_send(int sock, message_type_t type) {
    char buffer[1024];

    size_t size = protocol_serialize_message(
        type,
        NULL,
        0,
        buffer,
        sizeof(buffer)
    );

    return send(sock, buffer, size, 0);
}

/* správy s dátami (napr. CREATE_SIMULATION) */
int client_network_send_with_payload(
    int sock,
    message_type_t type,
    const void *payload,
    size_t payload_size
) {
    char buffer[1024];

    size_t size = protocol_serialize_message(
        type,
        payload,
        payload_size,
        buffer,
        sizeof(buffer)
    );

    return send(sock, buffer, size, 0);
}

void client_network_close(int sock) {
    if (sock >= 0) {
        close(sock);
    }
}

int client_network_receive(int sock, char *buffer, size_t buffer_size) {
  if (!buffer || buffer_size == 0) return  -1;
  ssize_t n = recv(sock, buffer, buffer_size, 0);
  if (n <= 0) return -1;

  buffer[n] = '\0';
  return (int)n;
}
