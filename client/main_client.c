#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "networking/client_network.h"
#include "networking/message.h"
#include "utils/input_parser.h"

#include "../shared/constants.h"
#include "../shared/dto.h"
#include "../shared/protocol.h"

int main(void) {
    int choice;
    int sock = -1;

    
    char recv_buf[MAX_MESSAGE_SIZE];

    while (1) {
        printf("\n===== PARKOVISKO - KLIENT =====\n");
        printf("1. Nova simulacia\n");
        printf("2. Pripojit sa k simulacii\n");
        printf("3. Zobrazit stav parkoviska\n");
        printf("4. Zobrazit statistiky\n");
        printf("5. Ukoncit simulaciu\n");
        printf("0. Koniec\n");
        printf("Vyber: ");

        if (scanf("%d", &choice) != 1) {
            printf("Neplatny vstup.\n");
            while (getchar() != '\n');
            continue;
        }

        if (choice == 0) {
            break;
        }

        /* 1. Nova simulacia */
        if (choice == 1) {
            if (sock >= 0) {
                printf("Uz si pripojeny k serveru.\n");
                continue;
            }

            printf("[CLIENT DEBUG] Pripajam sa na server...\n");
            fflush(stdout);

            sock = client_network_connect("127.0.0.1", DEFAULT_PORT);
            if (sock < 0) {
                printf("Nepodarilo sa pripojit na server.\n");
                continue;
            }

            printf("[CLIENT DEBUG] Pripojeny, socket=%d\n", sock);
            fflush(stdout);

            sim_config_t cfg;
            input_read_simulation_config(&cfg);

            printf("[CLIENT DEBUG] Posielam MSG_SIM_CONFIG\n");
            fflush(stdout);

            int sent = client_network_send_with_payload(sock, MSG_SIM_CONFIG, &cfg, sizeof(cfg));
            if (sent > 0) {
                printf("Simulacia vytvorena.\n");
            } else {
                printf("Chyba pri vytvarani simulacie.\n");
                client_network_close(sock);
                sock = -1;
            }
        }

        /* 2. Join simulacie */
        else if (choice == 2) {
            if (sock >= 0) {
                printf("Uz si pripojeny.\n");
                continue;
            }

            printf("[CLIENT DEBUG] Pripajam sa na server...\n");
            fflush(stdout);

            sock = client_network_connect("127.0.0.1", DEFAULT_PORT);
            if (sock < 0) {
                printf("Nepodarilo sa pripojit.\n");
                continue;
            }

            printf("[CLIENT DEBUG] Pripojeny, socket=%d\n", sock);
            fflush(stdout);

            printf("Pripojeny k simulacii.\n");
        }

        /* 3. Stav parkoviska */
        else if (choice == 3) {
            if (sock < 0) {
                printf("Najprv sa musis pripojit.\n");
                continue;
            }
            
            printf("[CLIENT DEBUG] Posielam MSG_GET_STATE na socket %d\n", sock);
            fflush(stdout);
            
            // Pošli žiadosť o stav
            int send_result = client_network_send(sock, MSG_GET_STATE);
            printf("[CLIENT DEBUG] send vratil: %d\n", send_result);
            fflush(stdout);

            if (send_result <= 0) {
                printf("Chyba pri posielani poziadavky.\n");
                continue;
            }

            printf("[CLIENT DEBUG] Cakam na odpoved...\n");
            fflush(stdout);

            // Prijmi odpoveď
            ssize_t n = client_network_receive(sock, recv_buf, sizeof(recv_buf) - 1);
            printf("[CLIENT DEBUG] Prijatych bajtov: %zd\n", n);
            fflush(stdout);

            if (n > 0) {
                recv_buf[n] = '\0';
                
                // Deserializuj správu
                message_type_t type = protocol_deserialize_type(recv_buf);
                printf("[CLIENT DEBUG] Typ prijatej spravy: %d\n", type);
                fflush(stdout);
                
                if (type == MSG_STATE_UPDATE) {
                    parking_state_t state;
                    if (protocol_deserialize_data(type, recv_buf, &state)) {
                        printf("\n=== STAV PARKOVISKA ===\n");
                        printf("Pocet miest: %d\n", state.num_spots);
                        printf("Obsadene miesta: %d\n", state.occupied_count);
                        printf("V rade caka: %d\n", state.queue_count);
                        printf("Simulacia bezi: %s\n", state.running ? "ANO" : "NIE");
                        printf("\nDetail miest (prvy 10):\n");
                        int max_show = state.num_spots > 10 ? 10 : state.num_spots;
                        for (int i = 0; i < max_show; i++) {
                            printf("Miesto %d: ", i);
                            if (state.spots[i].occupied && state.spots[i].vehicle) {
                                printf("OBSADENE (vozidlo %d)\n", state.spots[i].vehicle->id);
                            } else {
                                printf("VOLNE\n");
                            }
                        }
                        if (state.num_spots > 10) {
                            printf("... a dalsich %d miest\n", state.num_spots - 10);
                        }
                        printf("======================\n");
                    } else {
                        printf("Chyba pri deserializacii stavu.\n");
                    }
                } else {
                    printf("Neocakavany typ spravy: %d\n", type);
                }
            } else {
                printf("Nepodarilo sa prijat stav parkoviska (timeout alebo chyba).\n");
            }
        }

        /* 4. Statistiky */
        else if (choice == 4) {
            if (sock < 0) {
                printf("Najprv sa musis pripojit.\n");
                continue;
            }
            
            printf("[CLIENT DEBUG] Posielam MSG_GET_STATS na socket %d\n", sock);
            fflush(stdout);
            
            // Pošli žiadosť o štatistiky
            int send_result = client_network_send(sock, MSG_GET_STATS);
            printf("[CLIENT DEBUG] send vratil: %d\n", send_result);
            fflush(stdout);

            if (send_result <= 0) {
                printf("Chyba pri posielani poziadavky.\n");
                continue;
            }

            printf("[CLIENT DEBUG] Cakam na odpoved...\n");
            fflush(stdout);

            // Prijmi odpoveď
            ssize_t n = client_network_receive(sock, recv_buf, sizeof(recv_buf) - 1);
            printf("[CLIENT DEBUG] Prijatych bajtov: %zd\n", n);
            fflush(stdout);

            if (n > 0) {
                recv_buf[n] = '\0';
                
                // Deserializuj správu
                message_type_t type = protocol_deserialize_type(recv_buf);
                printf("[CLIENT DEBUG] Typ prijatej spravy: %d\n", type);
                fflush(stdout);
                
                if (type == MSG_STATISTICS_UPDATE) {
                    statistics_t stats;
                    if (protocol_deserialize_data(type, recv_buf, &stats)) {
                        printf("\n=== STATISTIKY SIMULACIE ===\n");
                        printf("Celkovo vozidiel:     %d\n", stats.total_vehicles);
                        printf("Zaparkovane vozidla:  %d\n", stats.parked_vehicles);
                        printf("Odmietnute vozidla:   %d\n", stats.rejected_vehicles);
                        printf("Priemerny cas parkovania: %.2f s\n", stats.avg_park_time);
                        printf("Priemerny cas cakania:     %.2f s\n", stats.avg_wait_time);
                        printf("===========================\n");
                    } else {
                        printf("Chyba pri deserializacii statistik.\n");
                    }
                } else {
                    printf("Neocakavany typ spravy: %d\n", type);
                }
            } else {
                printf("Nepodarilo sa prijat statistiky (timeout alebo chyba).\n");
            }
        }

        /* 5. Ukoncit simulaciu */
        else if (choice == 5) {
            if (sock < 0) {
                printf("Nie si pripojeny.\n");
                continue;
            }

            printf("[CLIENT DEBUG] Posielam MSG_STOP_SIMULATION\n");
            fflush(stdout);

            client_network_send(sock, MSG_STOP_SIMULATION);

            client_network_close(sock);
            sock = -1;

            printf("Simulacia ukoncena.\n");
        }

        else {
            printf("Neplatna volba.\n");
        }
    }

    if (sock >= 0) {
        client_network_close(sock);
    }

    printf("Klient ukonceny.\n");
    return 0;
}
