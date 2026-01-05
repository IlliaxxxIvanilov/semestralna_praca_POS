#include "event_generator.h"
#include "../parking/vehicle.h"
#include "../parking/parking_lot.h"
#include "../utils/random_generator.h"
#include "../utils/logger.h"

void event_generator_generate(parking_state_t *state, statistics_t *stats, const sim_config_t *config) {
    if (rand() % random_generator_int(config->arrival_min, config->arrival_max) == 0) {
        vehicle_t v;
        vehicle_init(&v, config);
        stats->total_vehicles++;

        logger_log("Prichadza vozidlo %d", v.id);

        if (!parking_lot_try_park(state, &v, stats)) {
            if (state->mode == WITH_WAITING && state->queue_count < MAX_QUEUE_SIZE) {
                state->queue[state->queue_count++] = v;
            } else {
                stats->rejected_vehicles++;
            }
        }
    }
}