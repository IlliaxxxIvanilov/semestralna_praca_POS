#include "vehicle.h"
#include "../utils/random_generator.h"
#include <time.h>
#include <pthread.h>

/*
 * Globálny čítač ID vozidiel.
 * Chránený mutexom, aby bol thread-safe.
 */
static int vehicle_id_counter = 0;
static pthread_mutex_t vehicle_id_mutex = PTHREAD_MUTEX_INITIALIZER;

void vehicle_init(vehicle_t *v, const sim_config_t *cfg) {
    /* --- THREAD-SAFE generovanie ID --- */
    pthread_mutex_lock(&vehicle_id_mutex);
    v->id = vehicle_id_counter++;
    pthread_mutex_unlock(&vehicle_id_mutex);

    /* --- Časy vozidla --- */
    v->arrival_time = time(NULL);
    v->park_duration = random_generator_int(cfg->park_min, cfg->park_max);
    v->departure_time = v->arrival_time + v->park_duration;
}

