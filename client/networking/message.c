#include "message.h"
#include <stdio.h>
#include <string.h>

/*
 * Vytvorenie správy na spustenie simulácie
 */
void message_create_simulation(char *buffer, size_t size, const sim_config_t *config) {
    snprintf(buffer, size,
             "%d|%d|%d|%d|%d|%d|%d|%d\n",
             MSG_CREATE_SIMULATION,
             config->num_spots,
             config->mode,
             config->duration,
             config->arrival_min,
             config->arrival_max,
             config->park_min,
             config->park_max);
}

/*
 * Klient sa chce pripojiť k simulácii
 */
void message_join_simulation(char *buffer, size_t size) {
    snprintf(buffer, size, "%d\n", MSG_JOIN_SIMULATION);
}

/*
 * Klient žiada aktuálny stav parkoviska
 */
void message_get_state(char *buffer, size_t size) {
    snprintf(buffer, size, "%d\n", MSG_GET_STATE);
}

/*
 * Klient žiada štatistiky
 */
void message_get_statistics(char *buffer, size_t size) {
    snprintf(buffer, size, "%d\n", MSG_GET_STATISTICS);
}

/*
 * Klient ukončí simuláciu (len creator)
 */
void message_end_simulation(char *buffer, size_t size) {
    snprintf(buffer, size, "%d\n", MSG_END_SIMULATION);
}

/*
 * Spracovanie správy prijatej zo servera
 */
message_type_t message_parse(const char *buffer, void *data_out) {
    message_type_t type = protocol_deserialize_type(buffer);

    if (data_out != NULL) {
        switch (type) {
            case MSG_STATE_UPDATE:
            case MSG_STATISTICS_UPDATE:
                protocol_deserialize_data(type, buffer, data_out);
                break;
            default:
                break;
        }
    }

    return type;
}

