#include "client_handler.h"
#include "../common/protocol.h"
#include "../common/utils.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

ClientThreadData* client_thread_data_create(int client_socket, Simulation *simulation) {
    ClientThreadData *data = (ClientThreadData*)malloc(sizeof(ClientThreadData));
    if (!data) {
        return NULL;
    }
    
    data->client_socket = client_socket;
    data->simulation = simulation;
    
    return data;
}

void client_thread_data_destroy(ClientThreadData *data) {
    if (data) {
        free(data);
    }
}

void* client_handler_thread(void *arg) {
    ClientThreadData *data = (ClientThreadData*)arg;
    if (!data) {
        return NULL;
    }
    
    int client_socket = data->client_socket;
    Simulation *sim = data->simulation;
    
    log_debug("SERVER", "Klient pripojený, socket=%d", client_socket);
    
    /* OPRAVA: Neposielame MSG_CONNECT_ACK automaticky
     * Odpoveď už bola poslaná v server_run() */
    
    int connected = 1;
    
    while (connected) {
        MessageHeader header;
        
        /* Prijatie hlavičky správy */
        if (recv_message_header(client_socket, &header) != 0) {
            log_debug("SERVER", "Klient sa odpojil");
            break;
        }
        
        switch (header.type) {
            case MSG_GET_STATE: {
                /* Žiadosť o stav parkoviska */
                ParkingState state;
                if (simulation_get_state(sim, &state) == 0) {
                    send_parking_state(client_socket, &state);
                } else {
                    send_error(client_socket, ERR_INTERNAL, "Nepodarilo sa získať stav");
                }
                break;
            }
            
            case MSG_GET_STATS: {
                /* Žiadosť o štatistiky */
                SimulationStats stats;
                if (simulation_get_statistics(sim, &stats) == 0) {
                    send_statistics(client_socket, &stats);
                } else {
                    send_error(client_socket, ERR_INTERNAL, "Nepodarilo sa získať štatistiky");
                }
                break;
            }
            
            case MSG_DISCONNECT: {
                /* Klient sa chce odpojiť */
                log_debug("SERVER", "Klient žiada odpojenie");
                connected = 0;
                break;
            }
            
            default:
                log_error("SERVER", "Neznámy typ správy: %d", header.type);
                send_error(client_socket, ERR_INTERNAL, "Neznámy typ správy");
                break;
        }
    }
    
    close(client_socket);
    client_thread_data_destroy(data);
    
    log_info("SERVER", "Klient odpojený, simulácia pokračuje");
    
    return NULL;
}
