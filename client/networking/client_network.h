#ifndef CLIENT_NETWORK_H
#define CLIENT_NETWORK_H

#include <stdbool.h>
#include "../../shared/protocol.h"

/*
 * Pripojenie klienta na server
 * vracia true ak sa podarí, inak false
 */
bool client_network_connect(const char *ip, int port);

/*
 * Odpojenie klienta
 */
void client_network_disconnect();

/*
 * Odoslanie správy serveru
 */
bool client_network_send(message_type_t type, const void *data);

/*
 * Prijatie správy zo servera
 */
bool client_network_receive(message_type_t *type, void *data);

#endif

