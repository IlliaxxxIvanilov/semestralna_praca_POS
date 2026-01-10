#include "server.h"
#include "client_handler.h"
#include "../common/utils.h"
#include "../common/protocol.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

Server* server_create(int port) {
    Server *server = (Server*)malloc(sizeof(Server));
    if (!server) {
        return NULL;
    }
    
    server->server_socket = -1;
    server->running = 0;
    server->simulation = NULL;
    server->port = port;
    
    return server;
}

void server_destroy(Server *server) {
    if (!server) {
        return;
    }
    
    if (server->simulation) {
        simulation_destroy(server->simulation);
    }
    
    if (server->server_socket >= 0) {
        close(server->server_socket);
    }
    
    free(server);
}

int server_start(Server *server) {
    if (!server) {
        return -1;
    }
    
    /* Vytvorenie soketu */
    server->server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server->server_socket < 0) {
        log_error("SERVER", "Nepodarilo sa vytvoriť socket");
        return -1;
    }
    
    /* Nastavenie možnosti znovu použiť adresu */
    int opt = 1;
    if (setsockopt(server->server_socket, SOL_SOCKET, SO_REUSEADDR, 
                   &opt, sizeof(opt)) < 0) {
        log_error("SERVER", "Nepodarilo sa nastaviť SO_REUSEADDR");
        close(server->server_socket);
        return -1;
    }
    
    /* Nastavenie adresy */
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(server->port);
    
    /* Bind */
    if (bind(server->server_socket, (struct sockaddr*)&address, 
             sizeof(address)) < 0) {
        log_error("SERVER", "Nepodarilo sa pripojiť na port %d", server->port);
        close(server->server_socket);
        return -1;
    }
    
    /* Listen */
    if (listen(server->server_socket, MAX_CLIENTS) < 0) {
        log_error("SERVER", "Nepodarilo sa začať počúvať");
        close(server->server_socket);
        return -1;
    }
    
    server->running = 1;
    log_info("SERVER", "Server spustený na porte %d", server->port);
    
    return 0;
}

void server_stop(Server *server) {
    if (!server) {
        return;
    }
    
    server->running = 0;
    
    if (server->simulation) {
        simulation_stop(server->simulation);
    }
    
    if (server->server_socket >= 0) {
        close(server->server_socket);
        server->server_socket = -1;
    }
    
    log_info("SERVER", "Server zastavený");
}

int server_create_simulation(Server *server, const SimulationConfig *config) {
    if (!server || !config) {
        return -1;
    }
    
    /* Ak už simulácia existuje, zničíme ju */
    if (server->simulation) {
        simulation_destroy(server->simulation);
        server->simulation = NULL;
    }
    
    /* Vytvorenie novej simulácie */
    server->simulation = simulation_create(config);
    if (!server->simulation) {
        log_error("SERVER", "Nepodarilo sa vytvoriť simuláciu");
        return -1;
    }
    
    /* Spustenie simulácie */
    if (simulation_start(server->simulation) != 0) {
        log_error("SERVER", "Nepodarilo sa spustiť simuláciu");
        simulation_destroy(server->simulation);
        server->simulation = NULL;
        return -1;
    }
    
    log_info("SERVER", "Simulácia vytvorená a spustená");
    return 0;
}

void server_run(Server *server) {
    if (!server) {
        return;
    }
    
    log_info("SERVER", "Čakám na pripojenia...");
    
    while (server->running) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_socket = accept(server->server_socket, 
                                    (struct sockaddr*)&client_addr, 
                                    &client_len);
        
        if (client_socket < 0) {
            if (server->running) {
                log_error("SERVER", "Chyba pri prijímaní pripojenia");
            }
            continue;
        }
        
        log_info("SERVER", "Nové pripojenie z %s:%d", 
                 inet_ntoa(client_addr.sin_addr), 
                 ntohs(client_addr.sin_port));
        
        /* Prijatie prvej správy (MSG_CONNECT alebo MSG_CREATE_SIM) */
        MessageHeader header;
        if (recv_message_header(client_socket, &header) != 0) {
            log_error("SERVER", "Nepodarilo sa prijať hlavičku");
            close(client_socket);
            continue;
        }
        
        if (header.type == MSG_CREATE_SIM) {
            /* Vytvorenie novej simulácie */
            SimulationConfig config;
            if (recv_simulation_config(client_socket, &config) == 0) {
                if (server_create_simulation(server, &config) == 0) {
                    send_message_header(client_socket, MSG_SIM_CREATED, 0);
                    log_info("SERVER", "Simulácia vytvorená z konfigurácie klienta");
                } else {
                    send_error(client_socket, ERR_INTERNAL, 
                              "Nepodarilo sa vytvoriť simuláciu");
                    close(client_socket);
                    continue;
                }
            } else {
                send_error(client_socket, ERR_INVALID_CONFIG, 
                          "Neplatná konfigurácia");
                close(client_socket);
                continue;
            }
        } else if (header.type == MSG_CONNECT) {
            /* Pripojenie k existujúcej simulácii */
            if (!server->simulation) {
                send_error(client_socket, ERR_SIM_NOT_FOUND, 
                          "Simulácia neexistuje");
                close(client_socket);
                continue;
            }
            /* Potvrdenie pripojenia sa odošle v client_handler_thread */
        } else {
            log_error("SERVER", "Neočakávaný typ správy: %d", header.type);
            send_error(client_socket, ERR_INTERNAL, "Neočakávaný typ správy");
            close(client_socket);
            continue;
        }
        
        /* Vytvorenie vlákna pre klienta */
        ClientThreadData *thread_data = client_thread_data_create(
            client_socket, server->simulation);
        
        if (!thread_data) {
            log_error("SERVER", "Nepodarilo sa vytvoriť dáta vlákna");
            close(client_socket);
            continue;
        }
        
        pthread_t thread;
        if (pthread_create(&thread, NULL, client_handler_thread, 
                          thread_data) != 0) {
            log_error("SERVER", "Nepodarilo sa vytvoriť vlákno");
            client_thread_data_destroy(thread_data);
            close(client_socket);
            continue;
        }
        
        /* Detach vlákna - nemusíme čakať na jeho ukončenie */
        pthread_detach(thread);
    }
}
