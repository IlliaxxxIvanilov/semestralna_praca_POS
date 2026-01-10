#include "simulation.h"
#include "../common/utils.h"
#include <stdlib.h>
#include <string.h>

/* Funkcia vlákna simulácie */
static void* simulation_thread_func(void *arg);

Simulation* simulation_create(const SimulationConfig *config) {
    if (!config || !validate_simulation_config(config)) {
        return NULL;
    }
    
    Simulation *sim = (Simulation*)malloc(sizeof(Simulation));
    if (!sim) {
        return NULL;
    }
    
    memcpy(&sim->config, config, sizeof(SimulationConfig));
    
    /* Vytvorenie parkoviska */
    sim->parking = parking_create(config->parking_spots, config->mode);
    if (!sim->parking) {
        free(sim);
        return NULL;
    }
    
    /* Inicializácia štatistík */
    if (statistics_init(&sim->stats) != 0) {
        parking_destroy(sim->parking);
        free(sim);
        return NULL;
    }
    
    /* Alokácia poľa pre vozidlá */
    sim->vehicles = (Vehicle**)calloc(MAX_SIM_VEHICLES, sizeof(Vehicle*));
    if (!sim->vehicles) {
        statistics_destroy(&sim->stats);
        parking_destroy(sim->parking);
        free(sim);
        return NULL;
    }
    
    /* Vytvorenie mutexu */
    sim->mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    if (!sim->mutex) {
        free(sim->vehicles);
        statistics_destroy(&sim->stats);
        parking_destroy(sim->parking);
        free(sim);
        return NULL;
    }
    
    if (pthread_mutex_init(sim->mutex, NULL) != 0) {
        free(sim->mutex);
        free(sim->vehicles);
        statistics_destroy(&sim->stats);
        parking_destroy(sim->parking);
        free(sim);
        return NULL;
    }
    
    sim->vehicle_count = 0;
    sim->next_vehicle_id = 1;
    sim->elapsed_time = 0;
    sim->next_arrival_time = random_range(config->min_arrival_interval, 
                                          config->max_arrival_interval);
    sim->running = 0;
    sim->thread_created = 0;
    
    return sim;
}

void simulation_destroy(Simulation *sim) {
    if (!sim) {
        return;
    }
    
    /* Zastavenie simulácie ak beží */
    if (sim->running) {
        simulation_stop(sim);
    }
    
    /* Uvoľnenie vozidiel */
    if (sim->vehicles) {
        for (uint32_t i = 0; i < sim->vehicle_count; i++) {
            if (sim->vehicles[i]) {
                vehicle_destroy(sim->vehicles[i]);
            }
        }
        free(sim->vehicles);
    }
    
    /* Uvoľnenie ostatných zdrojov */
    if (sim->mutex) {
        pthread_mutex_destroy(sim->mutex);
        free(sim->mutex);
    }
    
    statistics_destroy(&sim->stats);
    parking_destroy(sim->parking);
    free(sim);
}

static void* simulation_thread_func(void *arg) {
    Simulation *sim = (Simulation*)arg;
    if (!sim) {
        return NULL;
    }
    
    log_info("SERVER", "Simulacia spustena");
    
    while (sim->running && sim->elapsed_time < sim->config.duration_sec) {
        pthread_mutex_lock(sim->mutex);
        
        /* Generovanie nového vozidla */
        if (sim->elapsed_time >= sim->next_arrival_time && 
            sim->vehicle_count < MAX_SIM_VEHICLES) {
            
            uint32_t parking_duration = random_range(
                sim->config.min_parking_time,
                sim->config.max_parking_time
            );
            
            Vehicle *vehicle = vehicle_create(
                sim->next_vehicle_id++,
                sim->elapsed_time,
                parking_duration
            );
            
            if (vehicle) {
                sim->vehicles[sim->vehicle_count++] = vehicle;
                statistics_add_arrival(&sim->stats);
                
                /* Pokus o zaparkovanie */
                int result = parking_try_park(sim->parking, vehicle, sim->elapsed_time);
                
                if (result == 1) {
                    /* Úspešne zaparkované */
                    statistics_add_parked(&sim->stats, 0);
                    log_debug("SERVER", "Vozidlo %u zaparkované", vehicle->id);
                } else if (result == 0) {
                    /* Čaká vo fronte */
                    log_debug("SERVER", "Vozidlo %u čaká", vehicle->id);
                } else {
                    /* Odišlo (plné parkovisko, bez čakania) */
                    statistics_add_left_no_space(&sim->stats);
                    log_debug("SERVER", "Vozidlo %u odišlo (plné)", vehicle->id);
                }
            }
            
            /* Nastavenie času ďalšieho príchodu */
            sim->next_arrival_time = sim->elapsed_time + random_range(
                sim->config.min_arrival_interval,
                sim->config.max_arrival_interval
            );
        }
        
        /* Spracovanie odchodov */
        for (uint32_t i = 0; i < sim->vehicle_count; i++) {
            Vehicle *vehicle = sim->vehicles[i];
            if (vehicle && vehicle_should_depart(vehicle, sim->elapsed_time)) {
                //uint32_t parking_time = sim->elapsed_time - 
                  //  (vehicle->arrival_time + 
                 //    (vehicle->departure_time - vehicle->arrival_time - vehicle->parking_duration));
                
                parking_depart(sim->parking, vehicle);
                statistics_add_departure(&sim->stats, vehicle->parking_duration);
                log_debug("SERVER", "Vozidlo %u odišlo", vehicle->id);
            }
        }
        
        /* Spracovanie čakajúcich vozidiel */
        int parked_from_queue = parking_process_queue(sim->parking, sim->elapsed_time);
        if (parked_from_queue > 0) {
            /* Aktualizácia štatistík pre vozidlá z fronty */
            for (int i = 0; i < parked_from_queue; i++) {
                statistics_add_parked(&sim->stats, 1);
            }
        }
        
        /* Aktualizácia štatistík */
        sim->stats.current_parked = sim->parking->occupied_count;
        sim->stats.current_waiting = parking_get_waiting_count(sim->parking);
        
        sim->elapsed_time++;
        
        pthread_mutex_unlock(sim->mutex);
        
        /* Spíme 1 sekundu (simulačný tick) */
        sleep_ms(1000);
    }
    
    pthread_mutex_lock(sim->mutex);
    sim->running = 0;
    pthread_mutex_unlock(sim->mutex);
    
    log_info("SERVER", "Simulacia ukončená");
    
    return NULL;
}

int simulation_start(Simulation *sim) {
    if (!sim || !sim->mutex) {
        return -1;
    }
    
    pthread_mutex_lock(sim->mutex);
    
    if (sim->running) {
        pthread_mutex_unlock(sim->mutex);
        return -1;
    }
    
    sim->running = 1;
    sim->elapsed_time = 0;
    
    /* Vytvorenie vlákna */
    if (pthread_create(&sim->thread, NULL, simulation_thread_func, sim) != 0) {
        sim->running = 0;
        pthread_mutex_unlock(sim->mutex);
        return -1;
    }
    
    sim->thread_created = 1;
    pthread_mutex_unlock(sim->mutex);
    return 0;
}

void simulation_stop(Simulation *sim) {
    if (!sim || !sim->mutex) {
        return;
    }
    
    int was_running = 0;
    int thread_was_created = 0;
    
    pthread_mutex_lock(sim->mutex);
    if (sim->running) {
        sim->running = 0;
        was_running = 1;
    }
    thread_was_created = sim->thread_created;
    pthread_mutex_unlock(sim->mutex);
    
    /* Čakanie na ukončenie vlákna (len ak bolo vytvorené a bežalo) */
    if (was_running && thread_was_created) {
        pthread_join(sim->thread, NULL);
        
        pthread_mutex_lock(sim->mutex);
        sim->thread_created = 0;
        pthread_mutex_unlock(sim->mutex);
    }
}

int simulation_get_state(Simulation *sim, ParkingState *state) {
    if (!sim || !state || !sim->mutex) {
        return -1;
    }
    
    pthread_mutex_lock(sim->mutex);
    
    state->total_spots = sim->parking->total_spots;
    state->occupied_spots = sim->parking->occupied_count;
    state->waiting_vehicles = parking_get_waiting_count(sim->parking);
    state->elapsed_time = sim->elapsed_time;
    state->remaining_time = sim->config.duration_sec - sim->elapsed_time;
    state->mode = sim->config.mode;
    
    pthread_mutex_unlock(sim->mutex);
    
    return 0;
}

int simulation_get_statistics(Simulation *sim, SimulationStats *stats) {
    if (!sim || !stats || !sim->mutex) {
        return -1;
    }
    
    pthread_mutex_lock(sim->mutex);
    
    /* Kopírovanie základných štatistík */
    stats->total_arrivals = sim->stats.total_arrivals;
    stats->total_parked = sim->stats.total_parked;
    stats->total_left_no_space = sim->stats.total_left_no_space;
    stats->total_departed = sim->stats.total_departed;
    stats->current_parked = sim->stats.current_parked;
    stats->current_waiting = sim->stats.current_waiting;
    
    /* Výpočet priemerov */
    if (sim->stats.total_parking_count > 0) {
        stats->avg_parking_time = (float)sim->stats.total_parking_time / 
                                  (float)sim->stats.total_parking_count;
    } else {
        stats->avg_parking_time = 0.0f;
    }
    
    if (sim->stats.total_waited > 0) {
        stats->avg_wait_time = (float)sim->stats.total_wait_time / 
                               (float)sim->stats.total_waited;
    } else {
        stats->avg_wait_time = 0.0f;
    }
    
    /* Výpočet obsadenosti */
    if (sim->parking->total_spots > 0) {
        stats->occupancy_rate = ((float)sim->parking->occupied_count / 
                                 (float)sim->parking->total_spots) * 100.0f;
    } else {
        stats->occupancy_rate = 0.0f;
    }
    
    pthread_mutex_unlock(sim->mutex);
    
    return 0;
}

int simulation_is_running(Simulation *sim) {
    if (!sim || !sim->mutex) {
        return 0;
    }
    
    pthread_mutex_lock(sim->mutex);
    int running = sim->running;
    pthread_mutex_unlock(sim->mutex);
    
    return running;
}
