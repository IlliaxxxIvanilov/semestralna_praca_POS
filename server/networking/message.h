#ifndef MESSAGE_H
#define MESSAGE_H

#include "../../shared/protocol.h"  
#include "../../shared/constants.h"
#include "../../shared/dto.h"
#include <stddef.h>

#define MESSAGE_SEPARATOR '|'
#define MESSAGE_END       '\n'

void message_create_simulation(char *buffer, size_t size, const sim_config_t *config);
void message_join_simulation(char *buffer, size_t size);
void message_get_state(char *buffer, size_t size);
void message_get_statistics(char *buffer, size_t size);
void message_end_simulation(char *buffer, size_t size);

void message_ok(char *buffer, size_t size);
void message_error(char *buffer, size_t size, const char *reason);
void message_state_update(char *buffer, size_t size, const parking_state_t *state);
void message_statistics_update(char *buffer, size_t size, const statistics_t *stats);
void message_sim_ended(char *buffer, size_t size);

message_type_t message_parse(const char *buffer, void *data_out);

#endif
