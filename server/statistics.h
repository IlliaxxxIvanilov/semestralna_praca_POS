#ifndef STATISTICS_H
#define STATISTICS_H

#include "../common/protocol.h"
#include <pthread.h>

typedef struct {
    uint32_t total_arrivals;
    uint32_t total_parked;
    uint32_t total_left_no_space;
    uint32_t total_departed;
    uint32_t current_parked;
    uint32_t current_waiting;
    
    // Pre výpočet priemerov 
    uint64_t total_parking_time;   
    uint32_t total_parking_count;  
    uint64_t total_wait_time;      
    uint32_t total_waited;         
    
    pthread_mutex_t *mutex;        // Mutex pre synchronizáciu 
} Statistics;


int statistics_init(Statistics *stats);

void statistics_destroy(Statistics *stats);


void statistics_add_arrival(Statistics *stats);


void statistics_add_parked(Statistics *stats, uint32_t wait_time);


void statistics_add_departure(Statistics *stats, uint32_t parking_time);


void statistics_add_left_no_space(Statistics *stats);

void statistics_update_averages(Statistics *stats);


void statistics_calculate_occupancy(Statistics *stats, uint32_t total_spots);

#endif 
