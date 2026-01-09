#include "message.h"
#include "../../shared/protocol.h"
#include <string.h>

/*
 * Klientsky helper na vytvorenie správy bez payloadu
 */
size_t client_message_build(
    message_type_t type,
    void *buffer,
    size_t buffer_size
) {
    return protocol_serialize_message(
        type,
        NULL,
        0,
        buffer,
        buffer_size
    );
}

/*
 * Klientsky helper na vytvorenie správy s payloadom
 */
size_t client_message_build_with_payload(
    message_type_t type,
    const void *payload,
    size_t payload_size,
    void *buffer,
    size_t buffer_size
) {
    return protocol_serialize_message(
        type,
        payload,
        payload_size,
        buffer,
        buffer_size
    );
}

