#include "display.h"
#include <stdio.h>
//#include <stdlib.h>

void display_clear(void) {
    printf("\033[2J\033[H");
    fflush(stdout);
}

void display_header(void) {
    printf("====== PARKOVISKO - KLIENT ======\n");
}

void display_parking_state(const ParkingState *state) {
    if (!state) {
        printf("Chyba: Žiadny stav\n");
        return;
    }
    
    printf("\n========== STAV PARKOVISKA ==========\n");
    printf("Celkový počet miest:     %u\n", state->total_spots);
    printf("Obsadené miesta:         %u\n", state->occupied_spots);
    printf("Voľné miesta:            %u\n", state->total_spots - state->occupied_spots);
    printf("Čakajúce vozidlá:        %u\n", state->waiting_vehicles);
    printf("Režim:                   %s\n", 
           state->mode == PARKING_MODE_WITH_WAIT ? "s čakaním" : "bez čakania");
    printf("Uplynulý čas:            %u s\n", state->elapsed_time);
    printf("Zostávajúci čas:         %u s\n", state->remaining_time);
    
    /* Vizualizácia obsadenosti */
    printf("\nObsadenosť: [");
    int bar_length = 50;
    int filled = (int)((float)state->occupied_spots / (float)state->total_spots * bar_length);
    for (int i = 0; i < bar_length; i++) {
        if (i < filled) {
            printf("#");
        } else {
            printf("-");
        }
    }
    printf("] %.1f%%\n", 
           ((float)state->occupied_spots / (float)state->total_spots) * 100.0f);
    
    printf("=====================================\n");
}

void display_statistics(const SimulationStats *stats) {
    if (!stats) {
        printf("Chyba: Žiadne štatistiky\n");
        return;
    }
    
    printf("\n========== ŠTATISTIKY ==========\n");
    printf("Celkový počet príchodov:        %u\n", stats->total_arrivals);
    printf("Celkový počet zaparkovaných:    %u\n", stats->total_parked);
    printf("Odišli (nedostatok miesta):     %u\n", stats->total_left_no_space);
    printf("Celkový počet odchodov:         %u\n", stats->total_departed);
    printf("\n");
    printf("Aktuálne zaparkované:           %u\n", stats->current_parked);
    printf("Aktuálne čakajúce:              %u\n", stats->current_waiting);
    printf("\n");
    printf("Priemerný čas parkovania:       %.2f s\n", stats->avg_parking_time);
    printf("Priemerný čas čakania:          %.2f s\n", stats->avg_wait_time);
    printf("Miera obsadenosti:              %.2f%%\n", stats->occupancy_rate);
    printf("================================\n");
}

void display_error(const char *message) {
    if (message) {
        printf("[CHYBA] %s\n", message);
    }
}

void display_info(const char *message) {
    if (message) {
        printf("[INFO] %s\n", message);
    }
}

void display_pause(void) {
    printf("\nStlačte ENTER pre pokračovanie...");
    fflush(stdout);
    getchar();
}
