#ifndef EVENT_GENERATOR_H
#define EVENT_GENERATOR_H

#include "../../shared/dto.h"

void event_generator_generate(parking_state_t *state, statistics_t *stats, const sim_config_t *config);

#endif