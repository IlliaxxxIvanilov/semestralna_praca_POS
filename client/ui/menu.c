#include "menu.h"
#include <stdio.h>

/*
 * Hlavné menu aplikácie (pred simuláciou)
 */
int menu_show_main() {
    int choice;

    printf("\n=== PARKOVISKO – KLIENT ===\n");
    printf("1 - Nova simulacia\n");
    printf("2 - Pripojit sa k simulacii\n");
    printf("0 - Koniec\n");
    printf("> ");

    if (scanf("%d", &choice) != 1) {
        while (getchar() != '\n'); // vycisti vstup
        return -1;
    }

    return choice;
}

/*
 * Menu počas bežiacej simulácie
 */
int menu_show_simulation() {
    int choice;

    printf("\n--- SIMULACIA ---\n");
    printf("1 - Zobraz stav parkoviska\n");
    printf("2 - Zobraz statistiky\n");
    printf("3 - Ukoncit simulaciu\n");
    printf("0 - Odpojit klienta\n");
    printf("> ");

    if (scanf("%d", &choice) != 1) {
        while (getchar() != '\n');
        return -1;
    }

    return choice;
}

