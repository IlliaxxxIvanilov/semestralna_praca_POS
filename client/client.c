#include "client.h"
#include "network.h"
#include "menu.h"
#include "display.h"
#include "../common/utils.h"
#include "../common/config.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

Client* client_create(void) {
    Client *client = (Client*)malloc(sizeof(Client));
    if (!client) {
        return NULL;
    }
    
    client->socket = -1;
    client->connected = 0;
    client->in_simulation = 0;
    strcpy(client->server_host, "127.0.0.1");
    client->server_port = DEFAULT_PORT;
    
    return client;
}

void client_destroy(Client *client) {
    if (!client) {
        return;
    }
    
    if (client->connected) {
        client_disconnect(client);
    }
    
    free(client);
}

int client_connect(Client *client, const char *host, int port) {
    if (!client) {
        return -1;
    }
    
    if (client->connected) {
        display_info("Už ste pripojený");
        return 0;
    }
    
    client->socket = network_connect(host, port);
    if (client->socket < 0) {
        return -1;
    }
    
    client->connected = 1;
    strncpy(client->server_host, host, sizeof(client->server_host) - 1);
    client->server_port = port;
    
    return 0;
}

void client_disconnect(Client *client) {
    if (!client || !client->connected) {
        return;
    }
    
    if (client->socket >= 0) {
        network_send_disconnect(client->socket);
        network_disconnect(client->socket);
        client->socket = -1;
    }
    
    client->connected = 0;
    client->in_simulation = 0;
}

void client_run(Client *client) {
    if (!client) {
        return;
    }
    
    int running = 1;
    
    while (running) {
        display_clear();
        MenuOption option = menu_show();
        
        switch (option) {
            case MENU_NEW_SIMULATION: {
                /* Vytvorenie novej simulácie */
                if (client->in_simulation) {
                    display_error("Už ste v simulácii!");
                    display_pause();
                    break;
                }
                
                SimulationConfig config;
                if (menu_get_simulation_config(&config) != 0) {
                    display_error("Nepodarilo sa načítať konfiguráciu");
                    display_pause();
                    break;
                }
                
                /* Pripojenie k serveru */
                if (!client->connected) {
                    if (client_connect(client, client->server_host, client->server_port) != 0) {
                        display_error("Nepodarilo sa pripojiť k serveru");
                        display_pause();
                        break;
                    }
                }
                
                log_debug("CLIENT", "Posielam MSG_SIM_CONFIG");
                
                /* Vytvorenie simulácie */
                if (network_create_simulation(client->socket, &config) == 0) {
                    display_info("Simulácia vytvorená");
                    client->in_simulation = 1;
                } else {
                    display_error("Nepodarilo sa vytvoriť simuláciu");
                    client_disconnect(client);
                }
                
                display_pause();
                break;
            }
            
            case MENU_JOIN_SIMULATION: {
                /* Pripojenie k existujúcej simulácii */
                if (client->in_simulation) {
                    display_info("Už si pripojený");
                    display_pause();
                    break;
                }
                
                /* Pripojenie k serveru */
                if (!client->connected) {
                    if (client_connect(client, client->server_host, client->server_port) != 0) {
                        display_error("Nepodarilo sa pripojiť k serveru");
                        display_pause();
                        break;
                    }
                }
                
                /* Pripojenie k simulácii */
                if (network_join_simulation(client->socket) == 0) {
                    display_info("Pripojený k simulácii");
                    client->in_simulation = 1;
                } else {
                    display_error("Nepodarilo sa pripojiť k simulácii");
                    client_disconnect(client);
                }
                
                display_pause();
                break;
            }
            
            case MENU_SHOW_STATE: {
                /* Zobrazenie stavu parkoviska */
                if (!client->in_simulation) {
                    display_error("Nie ste v žiadnej simulácii!");
                    display_pause();
                    break;
                }
                
                ParkingState state;
                if (network_get_state(client->socket, &state) == 0) {
                    display_parking_state(&state);
                } else {
                    display_error("Nepodarilo sa získať stav");
                }
                
                display_pause();
                break;
            }
            
            case MENU_SHOW_STATS: {
                /* Zobrazenie štatistík */
                if (!client->in_simulation) {
                    display_error("Nie ste v žiadnej simulácii!");
                    display_pause();
                    break;
                }
                
                SimulationStats stats;
                if (network_get_statistics(client->socket, &stats) == 0) {
                    display_statistics(&stats);
                } else {
                    display_error("Nepodarilo sa získať štatistiky");
                }
                
                display_pause();
                break;
            }
            
            case MENU_STOP_SIMULATION: {
                /* Ukončenie simulácie */
                if (!client->in_simulation) {
                    display_error("Nie ste v žiadnej simulácii!");
                    display_pause();
                    break;
                }
                
                client_disconnect(client);
                display_info("Simulácia ukončená");
                display_pause();
                break;
            }
            
            case MENU_EXIT: {
                /* Ukončenie aplikácie */
                if (client->connected) {
                    client_disconnect(client);
                }
                running = 0;
                break;
            }
            
            default:
                display_error("Neplatná voľba!");
                display_pause();
                break;
        }
    }
}
