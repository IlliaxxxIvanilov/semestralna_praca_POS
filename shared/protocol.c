#include "protocol.h"
#include <string.h>

/*
 * Serializuje správu do bufferu
 * @return počet zapísaných bajtov alebo 0 pri chybe
 */
size_t protocol_serialize_message(
    message_type_t type,
    const void *payload,
    size_t payload_size,
    void *buffer,
    size_t buffer_size
) {
    if (!buffer)
        return 0;

    if (buffer_size < sizeof(message_header_t) + payload_size)
        return 0;

    message_header_t header;
    header.type = type;
    header.payload_size = payload_size;

    /* zapíš hlavičku */
    memcpy(buffer, &header, sizeof(header));

    /* zapíš payload (ak existuje) */
    if (payload && payload_size > 0) {
        memcpy(
            (char *)buffer + sizeof(header),
            payload,
            payload_size
        );
    }

    return sizeof(header) + payload_size;
}

/*
 * Z bufferu zistí typ správy
 */
message_type_t protocol_deserialize_type(const void *buffer) {
    if (!buffer)
        return 0;

    const message_header_t *header =
        (const message_header_t *)buffer;

    return header->type;
}

/*
 * Z bufferu načíta payload podľa typu správy
 * out_data musí ukazovať na správnu štruktúru (DTO)
 */
int protocol_deserialize_data(
    message_type_t type,
    const void *buffer,
    void *out_data
) {
    (void)type; /* momentálne typ nekontrolujeme */

    if (!buffer || !out_data)
        return 0;

    const message_header_t *header =
        (const message_header_t *)buffer;

    if (header->payload_size == 0)
        return 0;

    const void *payload =
        (const char *)buffer + sizeof(message_header_t);

    memcpy(out_data, payload, header->payload_size);
    return 1;
}

