#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <time.h>

uint64_t get_time_ms(void);

uint64_t get_time_sec(void);


void sleep_ms(uint32_t ms);


uint32_t random_range(uint32_t min, uint32_t max);

void init_random(uint32_t seed);


void log_debug(const char *component, const char *format, ...);


void log_info(const char *component, const char *format, ...);


void log_error(const char *component, const char *format, ...);


int validate_simulation_config(const void *config);

#endif 
