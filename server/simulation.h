#ifndef SIMULATION_H
#define SIMULATION_H

#include <stdint.h>
#include <pthread.h>
#include "../common/protocol.h"
#include "parking.h"
#include "vehicle.h"
#include "statistics.h"

/* Maximálny počet vozidiel v simulácii */
#define MAX_SIM_VEHICLES 1000

/* Štruktúra simulácie */
typedef struct {
    SimulationConfig config;       /* Konfigurácia */
    Parking *parking;              /* Parkovisko */
    Statistics stats;              /* Štatistiky */
    
    Vehicle **vehicles;            /* Pole všetkých vozidiel */
    uint32_t vehicle_count;        /* Počet vytvorených vozidiel */
    uint32_t next_vehicle_id;      /* ID pre ďalšie vozidlo */
    
    uint32_t elapsed_time;         /* Uplynulý čas (s) */
    uint32_t next_arrival_time;    /* Čas ďalšieho príchodu */
    
    int running;                   /* 1 = beží, 0 = zastavená */
    int thread_created;            /* 1 = vlákno bolo vytvorené */
    pthread_t thread;              /* Vlákno simulácie */
    pthread_mutex_t *mutex;        /* Mutex pre synchronizáciu */
} Simulation;

/**
 * Vytvorenie novej simulácie
 * @param config Konfigurácia simulácie
 * @return Ukazovateľ na simuláciu alebo NULL pri chybe
 */
Simulation* simulation_create(const SimulationConfig *config);

/**
 * Zničenie simulácie
 * @param sim Ukazovateľ na simuláciu
 */
void simulation_destroy(Simulation *sim);

/**
 * Spustenie simulácie (vytvorenie vlákna)
 * @param sim Ukazovateľ na simuláciu
 * @return 0 pri úspechu, -1 pri chybe
 */
int simulation_start(Simulation *sim);

/**
 * Zastavenie simulácie
 * @param sim Ukazovateľ na simuláciu
 */
void simulation_stop(Simulation *sim);

/**
 * Získanie aktuálneho stavu parkoviska
 * @param sim Ukazovateľ na simuláciu
 * @param state Ukazovateľ na štruktúru pre stav
 * @return 0 pri úspechu, -1 pri chybe
 */
int simulation_get_state(Simulation *sim, ParkingState *state);

/**
 * Získanie štatistík
 * @param sim Ukazovateľ na simuláciu
 * @param stats Ukazovateľ na štruktúru pre štatistiky
 * @return 0 pri úspechu, -1 pri chybe
 */
int simulation_get_statistics(Simulation *sim, SimulationStats *stats);

/**
 * Kontrola, či simulácia beží
 * @param sim Ukazovateľ na simuláciu
 * @return 1 ak beží, 0 inak
 */
int simulation_is_running(Simulation *sim);

#endif /* SIMULATION_H */
