#ifndef MESSAGE_H
#define MESSAGE_H

#include <stddef.h>
#include "../../shared/protocol.h"
#include "../../shared/dto.h"

/* Vytvorí binárnu správu do bufferu a vráti veľkosť */
size_t message_create_simulation(
    char *buffer,
    size_t buffer_size,
    const sim_config_t *config
);

size_t message_simple(
    char *buffer,
    size_t buffer_size,
    message_type_t type
);

size_t message_state_update(
    char *buffer,
    size_t buffer_size,
    const parking_state_t *state
);

size_t message_statistics_update(
    char *buffer,
    size_t buffer_size,
    const statistics_t *stats
);

#endif

