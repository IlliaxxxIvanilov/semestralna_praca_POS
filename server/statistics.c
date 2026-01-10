#include "statistics.h"
#include <stdlib.h>
#include <string.h>

int statistics_init(Statistics *stats) {
    if (!stats) {
        return -1;
    }
    
    memset(stats, 0, sizeof(Statistics));
    
    stats->mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    if (!stats->mutex) {
        return -1;
    }
    
    if (pthread_mutex_init(stats->mutex, NULL) != 0) {
        free(stats->mutex);
        stats->mutex = NULL;
        return -1;
    }
    
    return 0;
}

void statistics_destroy(Statistics *stats) {
    if (!stats) {
        return;
    }
    
    if (stats->mutex) {
        pthread_mutex_destroy(stats->mutex);
        free(stats->mutex);
        stats->mutex = NULL;
    }
}

void statistics_add_arrival(Statistics *stats) {
    if (!stats || !stats->mutex) {
        return;
    }
    
    pthread_mutex_lock(stats->mutex);
    stats->total_arrivals++;
    stats->current_waiting++;
    pthread_mutex_unlock(stats->mutex);
}

void statistics_add_parked(Statistics *stats, uint32_t wait_time) {
    if (!stats || !stats->mutex) {
        return;
    }
    
    pthread_mutex_lock(stats->mutex);
    stats->total_parked++;
    stats->current_parked++;
    if (stats->current_waiting > 0) {
        stats->current_waiting--;
    }
    
    if (wait_time > 0) {
        stats->total_wait_time += wait_time;
        stats->total_waited++;
    }
    pthread_mutex_unlock(stats->mutex);
}

void statistics_add_departure(Statistics *stats, uint32_t parking_time) {
    if (!stats || !stats->mutex) {
        return;
    }
    
    pthread_mutex_lock(stats->mutex);
    stats->total_departed++;
    if (stats->current_parked > 0) {
        stats->current_parked--;
    }
    
    stats->total_parking_time += parking_time;
    stats->total_parking_count++;
    pthread_mutex_unlock(stats->mutex);
}

void statistics_add_left_no_space(Statistics *stats) {
    if (!stats || !stats->mutex) {
        return;
    }
    
    pthread_mutex_lock(stats->mutex);
    stats->total_left_no_space++;
    if (stats->current_waiting > 0) {
        stats->current_waiting--;
    }
    pthread_mutex_unlock(stats->mutex);
}

void statistics_update_averages(Statistics *stats) {
    if (!stats) {
        return;
    }
    
    /* Priemery sa počítajú pri získavaní štatistík */
}

void statistics_calculate_occupancy(Statistics *stats, uint32_t total_spots) {
    if (!stats || total_spots == 0) {
        return;
    }
    
    /* Obsadenosť sa počíta pri získavaní štatistík */
    (void)total_spots;
}
