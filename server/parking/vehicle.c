#include "vehicle.h"
#include "../utils/random_generator.h"
#include <time.h>

static int vehicle_id_counter = 0;

void vehicle_init(vehicle_t *v, const sim_config_t *cfg) {
    v->id = vehicle_id_counter++;
    v->arrival_time = time(NULL);
    v->park_duration = random_generator_int(cfg->park_min, cfg->park_max);
    v->departure_time = v->arrival_time + v->park_duration;
}