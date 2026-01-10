#ifndef SERVER_H
#define SERVER_H

#include "simulation.h"

/* Štruktúra pre tracking vlákien */
typedef struct ThreadNode {
    pthread_t thread;
    struct ThreadNode *next;
} ThreadNode;

/* Štruktúra servera */
typedef struct {
    int server_socket;
    int running;
    Simulation *simulation;
    int port;
    ThreadNode *threads;
    pthread_mutex_t *threads_mutex;
} Server;

/**
 * Vytvorenie servera
 * @param port Port, na ktorom server počúva
 * @return Ukazovateľ na server alebo NULL pri chybe
 */
Server* server_create(int port);

/**
 * Zničenie servera
 * @param server Ukazovateľ na server
 */
void server_destroy(Server *server);

/**
 * Spustenie servera
 * @param server Ukazovateľ na server
 * @return 0 pri úspechu, -1 pri chybe
 */
int server_start(Server *server);

/**
 * Zastavenie servera
 * @param server Ukazovateľ na server
 */
void server_stop(Server *server);

/**
 * Vytvorenie novej simulácie
 * @param server Ukazovateľ na server
 * @param config Konfigurácia simulácie
 * @return 0 pri úspechu, -1 pri chybe
 */
int server_create_simulation(Server *server, const SimulationConfig *config);

/**
 * Hlavná slučka servera
 * @param server Ukazovateľ na server
 */
void server_run(Server *server);

#endif /* SERVER_H */
