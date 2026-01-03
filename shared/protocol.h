#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <stdio.h>

typedef enum MessageType {
  MSG_CREATE_SIMULATION,
  MSG_JOIN_SIMULATION,
  MSG_GET_STATE,
  MSG_GET_STATISTICS,
  MSG_END_SIMULATION,
  MSG_STATE_UPDATE,
  MSG_STATISTICS_UPDATE,
  MSG_SIM_ENDED,
  MSG_OK,
  MSG_ERROR
} message_type_t;

void protocol_serialize_message(message_type_t, const void *data, char *buffer, size_t buffer_size);
message_type_t protocol_deserialize_type(const char *buffer);
void protocol_deserialize_data(message_type_t type, const char *buffer, void *data);

#endif
