#ifndef SIMULATION_H
#define SIMULATION_H

#include <stdint.h>
#include <pthread.h>
#include "../common/protocol.h"
#include "parking.h"
#include "vehicle.h"
#include "statistics.h"

//max pocet vozidiel simulacii
#define MAX_SIM_VEHICLES 1000

/* Štruktúra simulácie */
typedef struct {
    SimulationConfig config;       
    Parking *parking;              
    Statistics stats;              
    
    Vehicle **vehicles;            
    uint32_t vehicle_count;        
    uint32_t next_vehicle_id;      
    
    uint32_t elapsed_time;        
    uint32_t next_arrival_time;    
    
    int running;                   
    pthread_t thread;              
    pthread_mutex_t *mutex;        
} Simulation;


Simulation* simulation_create(const SimulationConfig *config);


void simulation_destroy(Simulation *sim);


int simulation_start(Simulation *sim);


void simulation_stop(Simulation *sim);


int simulation_get_state(Simulation *sim, ParkingState *state);


int simulation_get_statistics(Simulation *sim, SimulationStats *stats);


int simulation_is_running(Simulation *sim);

#endif 
