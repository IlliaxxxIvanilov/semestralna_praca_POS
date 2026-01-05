#ifndef STATISTICS_H
#define STATISTICS_H

#include "../../shared/dto.h"

void statistics_init(statistics_t *stats);
void statistics_update_park_time(statistics_t *stats, double park_time);
void statistics_update_wait_time(statistics_t *stats, double wait_time);

#endif