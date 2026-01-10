#ifndef SERVER_H
#define SERVER_H

#include "simulation.h"

//struct pre serverik 
typedef struct {
    int server_socket;
    int running;
    Simulation *simulation;
    int port;
} Server;


Server* server_create(int port);

void server_destroy(Server *server);


int server_start(Server *server);


void server_stop(Server *server);


int server_create_simulation(Server *server, const SimulationConfig *config);

void server_run(Server *server);

#endif 
