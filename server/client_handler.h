#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include "simulation.h"

/* Štruktúra pre dáta vlákna klienta */
typedef struct {
    int client_socket;
    Simulation *simulation;
} ClientThreadData;


ClientThreadData* client_thread_data_create(int client_socket, Simulation *simulation);


void client_thread_data_destroy(ClientThreadData *data);


void* client_handler_thread(void *arg);

#endif 
