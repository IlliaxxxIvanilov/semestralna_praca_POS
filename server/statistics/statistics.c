#include "statistics.h"

void statistics_init(statistics_t *stats) {
    stats->total_vehicles = 0;
    stats->parked_vehicles = 0;
    stats->rejected_vehicles = 0;
    stats->avg_park_time = 0.0;
    stats->avg_wait_time = 0.0;
}

void statistics_update_park_time(statistics_t *stats, double park_time) {
    stats->parked_vehicles++;
    stats->avg_park_time = (stats->avg_park_time * (stats->parked_vehicles - 1) + park_time) / stats->parked_vehicles;
}

void statistics_update_wait_time(statistics_t *stats, double wait_time) {
    stats->avg_wait_time = (stats->avg_wait_time * (stats->parked_vehicles - 1) + wait_time) / stats->parked_vehicles;
}