#include "parking_view.h"
#include <stdio.h>

/*
 * Jednoduchý výpis stavu parkoviska
 */
void parking_view_print(const parking_state_t *state) {
    if (!state || !state->running) {
        printf("Simulacia nebezi.\n");
        return;
    }

    printf("\n=== STAV PARKOVISKA ===\n");
    printf("Pocet miest: %d\n", state->num_spots);
    printf("Obsadene miesta: %d\n", state->occupied_count);
    printf("V rade caka: %d\n", state->queue_count);

    printf("\nDetail miest:\n");
    for (int i = 0; i < state->num_spots; i++) {
        printf("Miesto %d: ", i);
        if (state->spots[i].occupied) {
            printf("OBSADENE (vozidlo %d)\n",
                   state->spots[i].vehicle
                       ? state->spots[i].vehicle->id
                       : -1);
        } else {
            printf("VOLNE\n");
        }
    }

    printf("======================\n");
}

