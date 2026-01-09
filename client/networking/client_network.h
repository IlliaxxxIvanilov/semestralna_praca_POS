#ifndef CLIENT_NETWORK_H
#define CLIENT_NETWORK_H

#include "../../shared/protocol.h"

/* Pripojenie na server – vracia socket alebo -1 */
int client_network_connect(const char *ip, int port);

/* Pošle jednoduchú správu (bez dát) */
int client_network_send(int sock, message_type_t type);

/* Zavrie socket */
void client_network_close(int sock);

#endif

