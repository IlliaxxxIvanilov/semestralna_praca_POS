#include "utils.h"
#include "config.h"
#include "protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/time.h>
#include <unistd.h>

uint64_t get_time_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)(tv.tv_sec) * 1000 + (uint64_t)(tv.tv_usec) / 1000;
}

uint64_t get_time_sec(void) {
    return time(NULL);
}

void sleep_ms(uint32_t ms) {
    usleep(ms * 1000);
}

uint32_t random_range(uint32_t min, uint32_t max) {
    if (min >= max) {
        return min;
    }
    return min + (rand() % (max - min + 1));
}

void init_random(uint32_t seed) {
    if (seed == 0) {
        srand((unsigned int)time(NULL));
    } else {
        srand(seed);
    }
}

void log_debug(const char *component, const char *format, ...) {
#if DEBUG_MODE
    va_list args;
    printf("[%s DEBUG] ", component);
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
    fflush(stdout);
#else
    (void)component;
    (void)format;
#endif
}

void log_info(const char *component, const char *format, ...) {
    va_list args;
    printf("[%s] ", component);
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
    fflush(stdout);
}

void log_error(const char *component, const char *format, ...) {
    va_list args;
    fprintf(stderr, "[%s ERROR] ", component);
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
    fflush(stderr);
}

int validate_simulation_config(const void *cfg) {
    const SimulationConfig *config = (const SimulationConfig *)cfg;
    
    if (!config) {
        return 0;
    }
    
    //Validácia počtu parkovacích miest
    if (config->parking_spots == 0 || config->parking_spots > MAX_PARKING_SPOTS) {
        return 0;
    }
    
    // Validácia režimu parkovania 
    if (config->mode != PARKING_MODE_NO_WAIT && config->mode != PARKING_MODE_WITH_WAIT) {
        return 0;
    }
    
    // Validácia trvania simulácie 
    if (config->duration_sec < MIN_SIMULATION_TIME || 
        config->duration_sec > MAX_SIMULATION_TIME) {
        return 0;
    }
    
    // Validácia intervalov príchodu 
    if (config->min_arrival_interval == 0 || 
        config->min_arrival_interval > config->max_arrival_interval) {
        return 0;
    }
    
    // Validácia času parkovania 
    if (config->min_parking_time == 0 || 
        config->min_parking_time > config->max_parking_time) {
        return 0;
    }
    
    return 1;
}
