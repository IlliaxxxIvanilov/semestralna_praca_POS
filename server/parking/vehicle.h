#ifndef VEHICLE_H
#define VEHICLE_H

#include "../../shared/dto.h"
#include "../simulation/simulation_config.h"

void vehicle_init(vehicle_t *v, const sim_config_t *cfg);

#endif