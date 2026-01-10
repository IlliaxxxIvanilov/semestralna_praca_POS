#ifndef DISPLAY_H
#define DISPLAY_H

#include "../common/protocol.h"


void display_clear(void);


void display_header(void);

void display_parking_state(const ParkingState *state);


void display_statistics(const SimulationStats *stats);


void display_error(const char *message);

void display_info(const char *message);


void display_pause(void);

#endif
