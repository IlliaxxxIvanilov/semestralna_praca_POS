#ifndef MENU_H
#define MENU_H

#include<stdlib.h>
#include "../common/protocol.h"

///vsetky moznosti co su v menu
typedef enum {
    MENU_NEW_SIMULATION = 1,
    MENU_JOIN_SIMULATION = 2,
    MENU_SHOW_STATE = 3,
    MENU_SHOW_STATS = 4,
    MENU_STOP_SIMULATION = 5,
    MENU_EXIT = 0
} MenuOption;


MenuOption menu_show(void);


int menu_get_simulation_config(SimulationConfig *config);

uint32_t menu_get_uint(const char *prompt, uint32_t min, uint32_t max);


void menu_get_string(const char *prompt, char *buffer, size_t size);

#endif 
