#include "parking_lot.h"
#include "vehicle.h"
#include "../utils/random_generator.h"
#include <stdlib.h>

void parking_lot_init(parking_state_t *state, int num_spots) {
    state->num_spots = num_spots;
    state->occupied_count = 0;
    state->queue_count = 0;
    state->running = false;
    for (int i = 0; i < num_spots; i++) {
        state->spots[i].id = i;
        state->spots[i].occupied = false;
        state->spots[i].vehicle = NULL;
    }
}

bool parking_lot_try_park(parking_state_t *state, const vehicle_t *v, statistics_t *stats) {
    if (state->occupied_count >= state->num_spots) return false;

    for (int i = 0; i < state->num_spots; i++) {
        if (!state->spots[i].occupied) {
            state->spots[i].occupied = true;
            state->spots[i].vehicle = malloc(sizeof(vehicle_t));
            if (state->spots[i].vehicle) *state->spots[i].vehicle = *v;
            state->occupied_count++;
            stats->parked_vehicles++;
            return true;
        }
    }
    return false;
}

void parking_lot_process_departures(parking_state_t *state, statistics_t *stats, time_t now) {
    for (int i = 0; i < state->num_spots; i++) {
        if (state->spots[i].occupied && state->spots[i].vehicle && now >= state->spots[i].vehicle->departure_time) {
            double park_time = difftime(now, state->spots[i].vehicle->arrival_time);
            if (stats->parked_vehicles > 0) {
                stats->avg_park_time = (stats->avg_park_time * (stats->parked_vehicles - 1) + park_time) / stats->parked_vehicles;
            }
            free(state->spots[i].vehicle);
            state->spots[i].vehicle = NULL;
            state->spots[i].occupied = false;
            state->occupied_count--;
        }
    }
}

void parking_lot_process_queue(parking_state_t *state, statistics_t *stats, time_t now) {
    if (state->queue_count == 0 || state->occupied_count >= state->num_spots) return;

    vehicle_t v = state->queue[0];
    memmove(state->queue, state->queue + 1, (state->queue_count - 1) * sizeof(vehicle_t));
    state->queue_count--;

    if (parking_lot_try_park(state, &v, stats)) {
        double wait_time = difftime(now, v.arrival_time);
        if (stats->parked_vehicles > 0) {
            stats->avg_wait_time = (stats->avg_wait_time * (stats->parked_vehicles - 1) + wait_time) / stats->parked_vehicles;
        }
    } else {
        stats->rejected_vehicles++;
    }
}