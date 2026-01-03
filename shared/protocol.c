#include "protocol.h"
#include "dto.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>

void protocol_serialize_message(message_type_t type, const void *data, char *buffer, size_t buffer_size) {
  memset(buffer, 0, buffer_size);
  snprintf(buffer, buffer_size, "%d|", type);
  size_t len = strlen(buffer);
  switch (type) {
    case MSG_CREATE_SIMULATION: {
      const sim_config_t *config = (const sim_config_t *)data;
      snprintf(buffer + len, buffer_size - len, "%d|%d|%d|#d|%d|%d|%d", config->num_spots, config->mode, config->duration, config->arrival_min, config->arrival_max, config->park_min, config->park_max);
      break;
    }
    case MSG_STATE_UPDATE: {
      const parking_state_t *state = (const parking_state_t *)data;
      snprintf(buffer + len, buffer_size - len, "%d|%d|%d|%d", state->occupied_count, state->queue_count, state->num_spots, state->mode);
      break;
    }
    case MSG_STATISTICS_UPDATE: {
      const statistics_t *stats = (const statistics_t *)data;
      snprintf(buffer + len, buffer_size - len, "%d|%d|%d|%lf|%lf", stats->total_vehicles, stats->parked_vehicles, stats->rejected_vehicles, stats->avg_park_time, stats->avg_wait_time);
      break;
    }
    default:
      break;
  }
}

message_type_t protocol_deserialize_type(const char *buffer) {
  int type;
  sscanf(buffer, "%d|", &type);
  return (message_type_t)type;
}

void protocol_deserialize_data(message_type_t type, const char *buffer, void *data) {
  const char *ptr = strchr(buffer, '|') + 1;
  switch (type) {
    case MSG_CREATE_SIMULATION: {
      const sim_config_t *config = (const sim_config_t *)data;
      sscanf(ptr, "%d|%d|%d|#d|%d|%d|%d", config->num_spots, config->mode, config->duration, config->arrival_min, config->arrival_max, config->park_min, config->park_max);
      break;
    }
    case MSG_STATE_UPDATE: {
      const parking_state_t *state = (const parking_state_t *)data;
      sscanf(ptr, "%d|%d|%d|%d", state->occupied_count, state->queue_count, state->num_spots, state->mode);
      break;
    }
    case MSG_STATISTICS_UPDATE: {
      const statistics_t *stats = (const statistics_t *)data;
      sscanf(ptr, "%d|%d|%d|%lf|%lf", stats->total_vehicles, stats->parked_vehicles, stats->rejected_vehicles, stats->avg_park_time, stats->avg_wait_time);
      break;
    }
    default:
      break;
  }
}
