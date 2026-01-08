#include "message.h"
#include "../../shared/protocol.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

void message_join_simulation(char *buffer, size_t size) {
    snprintf(buffer, size, "%d\n", MSG_JOIN_SIMULATION);
}

void message_get_state(char *buffer, size_t size) {
    snprintf(buffer, size, "%d\n", MSG_GET_STATE);
}

void message_get_statistics(char *buffer, size_t size) {
    snprintf(buffer, size, "%d\n", MSG_GET_STATISTICS);
}

void message_end_simulation(char *buffer, size_t size) {
    snprintf(buffer, size, "%d\n", MSG_END_SIMULATION);
}


void message_ok(char *buffer, size_t size) {
    snprintf(buffer, size, "%d\n", MSG_OK);
}

void message_error(char *buffer, size_t size, const char *reason) {
    snprintf(buffer, size, "%d|%s\n", MSG_ERROR, reason ? reason : "Neznama chyba");
}

void message_state_update(char *buffer, size_t size, const parking_state_t *state) {
    snprintf(buffer, size,
             "%d|%d|%d|%d|%d\n",
             MSG_STATE_UPDATE,
             state->occupied_count,
             state->queue_count,
             state->num_spots,
             state->mode);
}

void message_statistics_update(char *buffer, size_t size, const statistics_t *stats) {
    snprintf(buffer, size,
             "%d|%d|%d|%d|%.2f|%.2f\n",
             MSG_STATISTICS_UPDATE,
             stats->total_vehicles,
             stats->parked_vehicles,
             stats->rejected_vehicles,
             stats->avg_park_time,
             stats->avg_wait_time);
}

void message_sim_ended(char *buffer, size_t size) {
    snprintf(buffer, size, "%d\n", MSG_SIM_ENDED);
}


message_type_t message_parse(const char *buffer, void *data_out) {
    message_type_t type = protocol_deserialize_type(buffer);

    if (data_out && (type == MSG_CREATE_SIMULATION || type == MSG_STATE_UPDATE || type == MSG_STATISTICS_UPDATE)) {
        protocol_deserialize_data(type, buffer, data_out);
    }

    return type;
}
