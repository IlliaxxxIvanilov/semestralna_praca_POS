#include "simulation_config.h"

void simulation_config_default(sim_config_t *config) {
    config->num_spots = 20;
    config->mode = WITH_WAITING;
    config->duration = DEFAULT_SIM_DURATION;
    config->arrival_min = MIN_ARRIVAL_INTERVAL;
    config->arrival_max = MAX_ARRIVAL_INTERVAL;
    config->park_min = MIN_PARK_DURATION;
    config->park_max = MAX_PARK_DURATION;
}