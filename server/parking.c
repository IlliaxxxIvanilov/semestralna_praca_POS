#include "parking.h"
#include <stdlib.h>
#include <string.h>

/* Pomocné funkcie pre frontu */
static WaitingQueue* queue_create(void) {
    WaitingQueue *queue = (WaitingQueue*)malloc(sizeof(WaitingQueue));
    if (!queue) {
        return NULL;
    }
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
    return queue;
}

static void queue_destroy(WaitingQueue *queue) {
    if (!queue) {
        return;
    }
    
    QueueNode *current = queue->front;
    while (current) {
        QueueNode *next = current->next;
        free(current);
        current = next;
    }
    free(queue);
}

static int queue_enqueue(WaitingQueue *queue, Vehicle *vehicle) {
    if (!queue || !vehicle) {
        return -1;
    }
    
    QueueNode *node = (QueueNode*)malloc(sizeof(QueueNode));
    if (!node) {
        return -1;
    }
    
    node->vehicle = vehicle;
    node->next = NULL;
    
    if (queue->rear == NULL) {
        queue->front = node;
        queue->rear = node;
    } else {
        queue->rear->next = node;
        queue->rear = node;
    }
    
    queue->size++;
    return 0;
}

static Vehicle* queue_dequeue(WaitingQueue *queue) {
    if (!queue || queue->front == NULL) {
        return NULL;
    }
    
    QueueNode *node = queue->front;
    Vehicle *vehicle = node->vehicle;
    
    queue->front = node->next;
    if (queue->front == NULL) {
        queue->rear = NULL;
    }
    
    free(node);
    queue->size--;
    
    return vehicle;
}

/* Implementácia funkcií parkoviska */
Parking* parking_create(uint32_t total_spots, ParkingMode mode) {
    if (total_spots == 0 || total_spots > MAX_PARKING_SPOTS) {
        return NULL;
    }
    
    Parking *parking = (Parking*)malloc(sizeof(Parking));
    if (!parking) {
        return NULL;
    }
    
    parking->spots = (ParkingSpot*)malloc(sizeof(ParkingSpot) * total_spots);
    if (!parking->spots) {
        free(parking);
        return NULL;
    }
    
    parking->queue = queue_create();
    if (!parking->queue) {
        free(parking->spots);
        free(parking);
        return NULL;
    }
    
    parking->mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    if (!parking->mutex) {
        queue_destroy(parking->queue);
        free(parking->spots);
        free(parking);
        return NULL;
    }
    
    if (pthread_mutex_init(parking->mutex, NULL) != 0) {
        free(parking->mutex);
        queue_destroy(parking->queue);
        free(parking->spots);
        free(parking);
        return NULL;
    }
    
    /* Inicializácia parkovacích miest */
    for (uint32_t i = 0; i < total_spots; i++) {
        parking->spots[i].id = i;
        parking->spots[i].occupied = 0;
        parking->spots[i].vehicle = NULL;
    }
    
    parking->total_spots = total_spots;
    parking->occupied_count = 0;
    parking->mode = mode;
    
    return parking;
}

void parking_destroy(Parking *parking) {
    if (!parking) {
        return;
    }
    
    if (parking->mutex) {
        pthread_mutex_destroy(parking->mutex);
        free(parking->mutex);
    }
    
    if (parking->queue) {
        queue_destroy(parking->queue);
    }
    
    if (parking->spots) {
        free(parking->spots);
    }
    
    free(parking);
}

int parking_try_park(Parking *parking, Vehicle *vehicle, uint32_t current_time) {
    if (!parking || !vehicle || !parking->mutex) {
        return -1;
    }
    
    pthread_mutex_lock(parking->mutex);
    
    /* Hľadáme voľné miesto */
    int spot_id = -1;
    for (uint32_t i = 0; i < parking->total_spots; i++) {
        if (!parking->spots[i].occupied) {
            spot_id = i;
            break;
        }
    }
    
    /* Našli sme voľné miesto */
    if (spot_id >= 0) {
        parking->spots[spot_id].occupied = 1;
        parking->spots[spot_id].vehicle = vehicle;
        parking->occupied_count++;
        vehicle_park(vehicle, spot_id, current_time);
        pthread_mutex_unlock(parking->mutex);
        return 1;
    }
    
    /* Parkovisko plné */
    if (parking->mode == PARKING_MODE_WITH_WAIT) {
        /* Pridať do fronty */
        vehicle_wait(vehicle, current_time);
        queue_enqueue(parking->queue, vehicle);
        pthread_mutex_unlock(parking->mutex);
        return 0;
    } else {
        /* Bez čakania - vozidlo odchádza */
        vehicle_leave(vehicle);
        pthread_mutex_unlock(parking->mutex);
        return -1;
    }
}

int parking_depart(Parking *parking, Vehicle *vehicle) {
    if (!parking || !vehicle || !parking->mutex) {
        return -1;
    }
    
    pthread_mutex_lock(parking->mutex);
    
    int spot_id = vehicle->parking_spot_id;
    if (spot_id < 0 || spot_id >= (int)parking->total_spots) {
        pthread_mutex_unlock(parking->mutex);
        return -1;
    }
    
    /* Uvoľnenie miesta */
    parking->spots[spot_id].occupied = 0;
    parking->spots[spot_id].vehicle = NULL;
    parking->occupied_count--;
    
    vehicle_leave(vehicle);
    
    pthread_mutex_unlock(parking->mutex);
    return 0;
}

int parking_process_queue(Parking *parking, uint32_t current_time) {
    if (!parking || !parking->mutex) {
        return 0;
    }
    
    int parked_count = 0;
    
    pthread_mutex_lock(parking->mutex);
    
    /* Pokúšame sa zaparkovať vozidlá z fronty */
    while (parking->queue->size > 0 && parking->occupied_count < parking->total_spots) {
        /* Hľadáme voľné miesto */
        int spot_id = -1;
        for (uint32_t i = 0; i < parking->total_spots; i++) {
            if (!parking->spots[i].occupied) {
                spot_id = i;
                break;
            }
        }
        
        if (spot_id < 0) {
            break;
        }
        
        /* Vyberieme prvé vozidlo z fronty */
        Vehicle *vehicle = queue_dequeue(parking->queue);
        if (!vehicle) {
            break;
        }
        
        /* Zaparkujeme ho */
        parking->spots[spot_id].occupied = 1;
        parking->spots[spot_id].vehicle = vehicle;
        parking->occupied_count++;
        vehicle_park(vehicle, spot_id, current_time);
        parked_count++;
    }
    
    pthread_mutex_unlock(parking->mutex);
    
    return parked_count;
}

uint32_t parking_get_free_spots(Parking *parking) {
    if (!parking || !parking->mutex) {
        return 0;
    }
    
    pthread_mutex_lock(parking->mutex);
    uint32_t free = parking->total_spots - parking->occupied_count;
    pthread_mutex_unlock(parking->mutex);
    
    return free;
}

uint32_t parking_get_waiting_count(Parking *parking) {
    if (!parking || !parking->mutex || !parking->queue) {
        return 0;
    }
    
    pthread_mutex_lock(parking->mutex);
    uint32_t count = parking->queue->size;
    pthread_mutex_unlock(parking->mutex);
    
    return count;
}
