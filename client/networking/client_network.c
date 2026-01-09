#include "client_network.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "../../shared/constants.h"
#include "../../shared/protocol.h"

/*
 * Pripojenie na server
 */
int client_network_connect(const char *ip, int port) {
    int sock;
    struct sockaddr_in server_addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sock);
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sock);
        return -1;
    }

    printf("Pripojeny na server %s:%d\n", ip, port);
    return sock;
}

/*
 * Pošle správu serveru (bez dát)
 */
int client_network_send(int sock, message_type_t type) {
    char buffer[MAX_MESSAGE_SIZE];

    protocol_serialize_message(type, NULL, buffer, sizeof(buffer));

    ssize_t sent = send(sock, buffer, strlen(buffer), 0);
    if (sent <= 0) {
        perror("send");
        return -1;
    }

    return 0;
}

/*
 * Zavrie spojenie
 */
void client_network_close(int sock) {
    if (sock >= 0) {
        close(sock);
    }
}

