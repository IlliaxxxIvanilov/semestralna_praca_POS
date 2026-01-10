#include "menu.h"
#include "display.h"
#include "../common/config.h"
#include <stdio.h>
#include <string.h>

MenuOption menu_show(void) {
    display_header();
    printf("1. Nova simulacia\n");
    printf("2. Pripojit sa k simulacii\n");
    printf("3. Zobrazit stav parkoviska\n");
    printf("4. Zobrazit statistiky\n");
    printf("5. Ukoncit simulaciu\n");
    printf("0. Koniec\n");
    printf("Vyber: ");
    fflush(stdout);
    
    int choice;
    if (scanf("%d", &choice) != 1) {
        while (getchar() != '\n');
        return MENU_EXIT;
    }
    while (getchar() != '\n');
    
    return (MenuOption)choice;
}

uint32_t menu_get_uint(const char *prompt, uint32_t min, uint32_t max) {
    uint32_t value;
    
    while (1) {
        printf("%s", prompt);
        fflush(stdout);
        
        if (scanf("%u", &value) != 1) {
            while (getchar() != '\n');
            printf("Neplatný vstup. Skúste znova.\n");
            continue;
        }
        while (getchar() != '\n');
        
        if (value < min || value > max) {
            printf("Hodnota musí byť medzi %u a %u. Skúste znova.\n", min, max);
            continue;
        }
        
        return value;
    }
}

void menu_get_string(const char *prompt, char *buffer, size_t size) {
    printf("%s", prompt);
    fflush(stdout);
    
    if (fgets(buffer, size, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
    }
}

int menu_get_simulation_config(SimulationConfig *config) {
    if (!config) {
        return -1;
    }
    
    printf("\n========== KONFIGURÁCIA SIMULÁCIE ==========\n");
    
    /* Počet parkovacích miest */
    config->parking_spots = menu_get_uint(
        "Zadaj pocet parkovacich miest: ",
        1, MAX_PARKING_SPOTS
    );
    
    /* Režim parkovania */
    printf("Rezim parkovania (0 = bez cakania, 1 = s cakanim): ");
    fflush(stdout);
    uint32_t mode;
    if (scanf("%u", &mode) != 1) {
        while (getchar() != '\n');
        return -1;
    }
    while (getchar() != '\n');
    
    if (mode != 0 && mode != 1) {
        printf("Neplatný režim!\n");
        return -1;
    }
    config->mode = (ParkingMode)mode;
    
    /* Dĺžka simulácie */
    config->duration_sec = menu_get_uint(
        "Dlzka simulacie (s): ",
        MIN_SIMULATION_TIME, MAX_SIMULATION_TIME
    );
    
    /* Minimálny interval príchodu */
    config->min_arrival_interval = menu_get_uint(
        "Minimalny interval prichodu (s): ",
        1, 3600
    );
    
    /* Maximálny interval príchodu */
    config->max_arrival_interval = menu_get_uint(
        "Maximalny interval prichodu (s): ",
        config->min_arrival_interval, 3600
    );
    
    /* Minimálny čas parkovania */
    config->min_parking_time = menu_get_uint(
        "Minimalny cas parkovania (s): ",
        1, 3600
    );
    
    /* Maximálny čas parkovania */
    config->max_parking_time = menu_get_uint(
        "Maximalny cas parkovania (s): ",
        config->min_parking_time, 3600
    );
    
    return 0;
}
