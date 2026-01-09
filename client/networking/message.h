#ifndef MESSAGE_H
#define MESSAGE_H

#include "../../shared/protocol.h"
#include <stddef.h>

size_t message_build_simple(
  message_type_t type,
  char *buffer,
  size_t buffer_size
);

size_t client_message_build(
    message_type_t type,
    void *buffer,
    size_t buffer_size
);

size_t client_message_build_with_payload(
    message_type_t type,
    const void *payload,
    size_t payload_size,
    void *buffer,
    size_t buffer_size
);

#endif

