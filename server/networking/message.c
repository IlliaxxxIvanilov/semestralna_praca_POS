#include "message.h"
#include "../../shared/constants.h"

size_t message_create_simulation(
    char *buffer,
    size_t buffer_size,
    const sim_config_t *config
) {
    return protocol_serialize_message(
        MSG_SIM_CONFIG,
        config,
        sizeof(sim_config_t),
        buffer,
        buffer_size
    );
}

size_t message_simple(
    char *buffer,
    size_t buffer_size,
    message_type_t type
) {
    return protocol_serialize_message(
        type,
        NULL,
        0,
        buffer,
        buffer_size
    );
}

size_t message_state_update(
    char *buffer,
    size_t buffer_size,
    const parking_state_t *state
) {
    return protocol_serialize_message(
        MSG_STATE_UPDATE,
        state,
        sizeof(parking_state_t),
        buffer,
        buffer_size
    );
}

size_t message_statistics_update(
    char *buffer,
    size_t buffer_size,
    const statistics_t *stats
) {
    return protocol_serialize_message(
        MSG_STATISTICS_UPDATE,
        stats,
        sizeof(statistics_t),
        buffer,
        buffer_size
    );
}

