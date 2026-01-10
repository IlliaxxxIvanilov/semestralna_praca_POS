#ifndef VEHICLE_H
#define VEHICLE_H

#include <stdint.h>
#include "../common/config.h"

//struct pre to co obshauje vozidlo
typedef struct {
    uint32_t id;                    
    uint32_t arrival_time;          
    uint32_t parking_duration;      
    uint32_t departure_time;        
    uint32_t wait_start_time;       
    VehicleState state;             
    int parking_spot_id;            
} Vehicle;


Vehicle* vehicle_create(uint32_t id, uint32_t arrival_time, uint32_t parking_duration);


void vehicle_destroy(Vehicle *vehicle);


void vehicle_park(Vehicle *vehicle, int spot_id, uint32_t current_time);


void vehicle_wait(Vehicle *vehicle, uint32_t current_time);


void vehicle_leave(Vehicle *vehicle);


int vehicle_should_depart(Vehicle *vehicle, uint32_t current_time);

#endif 
