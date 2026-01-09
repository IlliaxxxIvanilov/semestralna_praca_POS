#ifndef CLIENT_NETWORK_H
#define CLIENT_NETWORK_H

#include <stddef.h>
#include "../../shared/protocol.h"

/* pripojenie na server */
int client_network_connect(const char *host, int port);

/* odoslanie správy BEZ dát */
int client_network_send(int sock, message_type_t type);

/* odoslanie správy S dátami */
int client_network_send_with_payload(
    int sock,
    message_type_t type,
    const void *payload,
    size_t payload_size
);

/* zatvorenie spojenia */
void client_network_close(int sock);

#endif

