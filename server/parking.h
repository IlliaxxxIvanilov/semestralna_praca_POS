#ifndef PARKING_H
#define PARKING_H

#include <stdint.h>
#include <pthread.h>
#include "../common/config.h"
#include "vehicle.h"

// Štruktúra parkovacieho miesta
typedef struct {
    int id;                
    int occupied;          
    Vehicle *vehicle;      
} ParkingSpot;

// Prvok fronty čakajúcich vozidiel
typedef struct QueueNode {
    Vehicle *vehicle;
    struct QueueNode *next;
} QueueNode;


typedef struct {
    QueueNode *front;
    QueueNode *rear;
    uint32_t size;
} WaitingQueue;


typedef struct {
    ParkingSpot *spots;           
    uint32_t total_spots;         
    uint32_t occupied_count;      
    WaitingQueue *queue;          
    ParkingMode mode;             
    pthread_mutex_t *mutex;       
} Parking;

Parking* parking_create(uint32_t total_spots, ParkingMode mode);

void parking_destroy(Parking *parking);

int parking_try_park(Parking *parking, Vehicle *vehicle, uint32_t current_time);

int parking_depart(Parking *parking, Vehicle *vehicle);

int parking_process_queue(Parking *parking, uint32_t current_time);

uint32_t parking_get_free_spots(Parking *parking);

uint32_t parking_get_waiting_count(Parking *parking);

#endif 
