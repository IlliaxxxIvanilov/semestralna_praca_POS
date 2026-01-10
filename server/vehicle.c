#include "vehicle.h"
#include <stdlib.h>
#include <string.h>

Vehicle* vehicle_create(uint32_t id, uint32_t arrival_time, uint32_t parking_duration) {
    Vehicle *vehicle = (Vehicle*)malloc(sizeof(Vehicle));
    if (!vehicle) {
        return NULL;
    }
    
    vehicle->id = id;
    vehicle->arrival_time = arrival_time;
    vehicle->parking_duration = parking_duration;
    vehicle->departure_time = 0;
    vehicle->wait_start_time = 0;
    vehicle->state = VEHICLE_STATE_WAITING;
    vehicle->parking_spot_id = -1;
    
    return vehicle;
}

void vehicle_destroy(Vehicle *vehicle) {
    if (vehicle) {
        free(vehicle);
    }
}

void vehicle_park(Vehicle *vehicle, int spot_id, uint32_t current_time) {
    if (!vehicle) {
        return;
    }
    
    vehicle->state = VEHICLE_STATE_PARKED;
    vehicle->parking_spot_id = spot_id;
    vehicle->departure_time = current_time + vehicle->parking_duration;
}

void vehicle_wait(Vehicle *vehicle, uint32_t current_time) {
    if (!vehicle) {
        return;
    }
    
    vehicle->state = VEHICLE_STATE_WAITING;
    vehicle->wait_start_time = current_time;
}

void vehicle_leave(Vehicle *vehicle) {
    if (!vehicle) {
        return;
    }
    
    vehicle->state = VEHICLE_STATE_LEFT;
    vehicle->parking_spot_id = -1;
}

int vehicle_should_depart(Vehicle *vehicle, uint32_t current_time) {
    if (!vehicle) {
        return 0;
    }
    
    if (vehicle->state == VEHICLE_STATE_PARKED && 
        current_time >= vehicle->departure_time) {
        return 1;
    }
    
    return 0;
}
