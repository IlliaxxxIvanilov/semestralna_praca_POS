#ifndef DTO_H
#define DTO_H

#include <time.h>
#include <stdbool.h>
#include "constants.h"

typedef struct Vehicle {
  int id;
  time_t arrival_time;
  int park_duration;
  time_t departure_time;
} vehicle_t;

typedef struct ParkingSpot {
  int id;
  bool occupied;
  vehicle_t *vehicle;
} parking_spot_t;

typedef enum ParkingMode {
  NO_WAITING,
  WITH_WAITING
} parking_mode_t;

typedef struct SimConfig {
  int num_spots;
  parking_mode_t mode;
  int duration;
  int arrival_min;
  int arrival_max;
  int park_min;
  int park_max;
} sim_config_t;

typedef struct ParkingState {
  int occupied_count;
  int queue_count;
  int num_spots;
  parking_mode_t mode;
  parking_spot_t spots[MAX_PARKING_SPOTS];
  vehicle_t queue[MAX_QUEUE_SIZE];
  bool running;
  time_t start_time;
} parking_state_t;

typedef struct Statistics {
  int total_vehicles;
  int parked_vehicles;
  int rejected_vehicles;
  double avg_park_time;
  double avg_wait_time;
} statistics_t;

#endif

