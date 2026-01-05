#ifndef PARKING_LOT_H
#define PARKING_LOT_H

#include "../../shared/dto.h"
#include "../../shared/constants.h"

void parking_lot_init(parking_state_t *state, int num_spots);
bool parking_lot_try_park(parking_state_t *state, const vehicle_t *v, statistics_t *stats);
void parking_lot_process_departures(parking_state_t *state, statistics_t *stats, time_t now);
void parking_lot_process_queue(parking_state_t *state, statistics_t *stats, time_t now);

#endif