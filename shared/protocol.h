#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stddef.h>
#include "constants.h"
#include "dto.h"

/* ===== TYPY SPRÁV ===== */
typedef enum {
    MSG_PING = 1,

   /* klient → server */
    MSG_START_SIMULATION,
    MSG_SIM_CONFIG,
    MSG_GET_STATS,
    MSG_STOP_SIMULATION,
    MSG_JOIN_SIMULATION,
    MSG_CREATE_SIMULATION,
    MSG_GET_STATE,
    MSG_END_SIM,

    /* server → klient */
    MSG_STATE_UPDATE,
    MSG_STATISTICS_UPDATE,
    MSG_SIM_ENDED,

    /* server → klient */
    MSG_OK,
    MSG_ERROR
} message_type_t;

/* ===== HLAVIČKA SPRÁVY ===== */
typedef struct {
    message_type_t type;
    size_t payload_size;
} message_header_t;

/* ===== SERIALIZÁCIA ===== */
size_t protocol_serialize_message(
    message_type_t type,
    const void *payload,
    size_t payload_size,
    void *buffer,
    size_t buffer_size
);

/* ===== DESERIALIZÁCIA ===== */
message_type_t protocol_deserialize_type(const void *buffer);

int protocol_deserialize_data(
    message_type_t type,
    const void *buffer,
    void *out_data
);

#endif

