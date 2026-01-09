#include <stdio.h>
#include <unistd.h>

#include "networking/client_network.h"
#include "utils/input_parser.h"
#include "../shared/constants.h"

/*
 * Hlavný klientsky program
 */
int main(void) {
    int choice;
    int sock = -1;

    while (1) {
        printf("\n===== PARKOVISKO – KLIENT =====\n");
        printf("1. Nova simulacia\n");
        printf("2. Pripojit sa k simulacii\n");
        printf("3. Zobrazit stav parkoviska\n");
        printf("4. Zobrazit statistiky\n");
        printf("5. Ukoncit simulaciu\n");
        printf("0. Koniec\n");
        printf("Vyber: ");

        scanf("%d", &choice);

        if (choice == 0) {
            break;
        }

        /* 1. Nova simulacia */
        if (choice == 1) {
            if (sock >= 0) {
                printf("Uz si pripojeny k serveru.\n");
                continue;
            }

            sock = client_network_connect("127.0.0.1", DEFAULT_PORT);
            if (sock < 0) {
                printf("Nepodarilo sa pripojit na server.\n");
                continue;
            }

            sim_config_t cfg;
            input_read_simulation_config(&cfg);

            client_network_send(sock, MSG_CREATE_SIMULATION);
            printf("Simulacia vytvorena.\n");
        }

        /* 2. Join simulacie */
        else if (choice == 2) {
            if (sock >= 0) {
                printf("Uz si pripojeny.\n");
                continue;
            }

            sock = client_network_connect("127.0.0.1", DEFAULT_PORT);
            if (sock < 0) {
                printf("Nepodarilo sa pripojit.\n");
                continue;
            }

            client_network_send(sock, MSG_JOIN_SIMULATION);
            printf("Pripojeny k simulacii.\n");
        }

        /* 3. Stav parkoviska */
        else if (choice == 3) {
            if (sock < 0) {
                printf("Najprv sa musis pripojit.\n");
                continue;
            }

            client_network_send(sock, MSG_GET_STATE);
        }

        /* 4. Statistiky */
        else if (choice == 4) {
            if (sock < 0) {
                printf("Najprv sa musis pripojit.\n");
                continue;
            }

            client_network_send(sock, MSG_GET_STATISTICS);
        }

        /* 5. Ukoncit simulaciu */
        else if (choice == 5) {
            if (sock < 0) {
                printf("Nie si pripojeny.\n");
                continue;
            }

            client_network_send(sock, MSG_END_SIMULATION);
            client_network_close(sock);
            sock = -1;

            printf("Simulacia ukoncena.\n");
        }
    }

    if (sock >= 0) {
        client_network_close(sock);
    }

    printf("Klient ukonceny.\n");
    return 0;
}

