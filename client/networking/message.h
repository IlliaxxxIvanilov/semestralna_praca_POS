#ifndef CLIENT_MESSAGE_H
#define CLIENT_MESSAGE_H

#include <stddef.h>
#include "../../shared/protocol.h"
#include "../../shared/dto.h"

/*
 * Funkcie na vytvorenie správ, ktoré klient posiela serveru.
 * Všetky správy sú obyčajný text (string).
 */

/* požiadavky od klienta */
void message_create_simulation(char *buffer, size_t size, const sim_config_t *config);
void message_join_simulation(char *buffer, size_t size);
void message_get_state(char *buffer, size_t size);
void message_get_statistics(char *buffer, size_t size);
void message_end_simulation(char *buffer, size_t size);

/*
 * Parsovanie správy prijatej zo servera
 * - vráti typ správy
 * - ak je potrebné, vyplní data_out
 */
message_type_t message_parse(const char *buffer, void *data_out);

#endif

