#include "time_manager.h"

void time_manager_init(time_manager_t *tm) {
    tm->current_time = time(NULL);
}

time_t time_manager_get_time(const time_manager_t *tm) {
    return tm->current_time;
}