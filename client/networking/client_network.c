#include "client_network.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "../../shared/constants.h"
#include "../../shared/protocol.h"

/* socket klienta (len v tomto súbore) */
static int client_socket = -1;

bool client_network_connect(const char *ip, int port) {
    struct sockaddr_in server_addr;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("socket");
        return false;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(client_socket);
        client_socket = -1;
        return false;
    }

    if (connect(client_socket,
                (struct sockaddr *)&server_addr,
                sizeof(server_addr)) < 0) {
        perror("connect");
        close(client_socket);
        client_socket = -1;
        return false;
    }

    return true;
}

void client_network_disconnect() {
    if (client_socket != -1) {
        close(client_socket);
        client_socket = -1;
    }
}

bool client_network_send(message_type_t type, const void *data) {
    char buffer[MAX_MESSAGE_SIZE];

    protocol_serialize_message(type, data, buffer, sizeof(buffer));

    ssize_t sent = send(client_socket, buffer, strlen(buffer), 0);
    return sent > 0;
}

bool client_network_receive(message_type_t *type, void *data) {
    char buffer[MAX_MESSAGE_SIZE];

    ssize_t received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (received <= 0) {
        return false;
    }

    buffer[received] = '\0';

    *type = protocol_deserialize_type(buffer);
    protocol_deserialize_data(*type, buffer, data);

    return true;
}

