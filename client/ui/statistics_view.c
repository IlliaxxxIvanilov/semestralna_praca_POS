#include "statistics_view.h"
#include <stdio.h>

void statistics_view_print(const statistics_t *stats) {
    if (!stats) {
        printf("Statistiky nie su dostupne.\n");
        return;
    }

    printf("\n=== STATISTIKY SIMULACIE ===\n");
    printf("Celkovo vozidiel:     %d\n", stats->total_vehicles);
    printf("Zaparkovane vozidla:  %d\n", stats->parked_vehicles);
    printf("Odmietnute vozidla:   %d\n", stats->rejected_vehicles);
    printf("Priemerny cas parkovania: %.2f s\n", stats->avg_park_time);
    printf("Priemerny cas cakania:     %.2f s\n", stats->avg_wait_time);
    printf("===========================\n");
}

