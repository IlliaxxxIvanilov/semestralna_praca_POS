#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include <time.h>

typedef struct TimeManager {
    time_t current_time;
} time_manager_t;

void time_manager_init(time_manager_t *tm);
time_t time_manager_get_time(const time_manager_t *tm);

#endif  