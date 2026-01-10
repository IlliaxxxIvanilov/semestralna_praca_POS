#include "server.h"
#include "../common/config.h"
#include "../common/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

/* Globálna premenná pre server (pre signal handler) */
static Server *g_server = NULL;

/* Signal handler pre CTRL+C */
static void signal_handler(int sig) {
    (void)sig;
    if (g_server) {
        log_info("SERVER", "Prijatý signál na ukončenie");
        server_stop(g_server);
    }
}

int main(int argc, char *argv[]) {
    int port = DEFAULT_PORT;
    
    /* Parsovanie argumentov */
    if (argc > 1) {
        port = atoi(argv[1]);
        if (port <= 0 || port > 65535) {
            fprintf(stderr, "Neplatný port: %s\n", argv[1]);
            fprintf(stderr, "Použitie: %s [port]\n", argv[0]);
            return 1;
        }
    }
    
    /* Inicializácia generátora náhodných čísel */
    init_random(0);
    
    /* Vytvorenie servera */
    g_server = server_create(port);
    if (!g_server) {
        log_error("SERVER", "Nepodarilo sa vytvoriť server");
        return 1;
    }
    
    /* Nastavenie signal handlera */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    /* Spustenie servera */
    if (server_start(g_server) != 0) {
        server_destroy(g_server);
        return 1;
    }
    
    /* Hlavná slučka */
    server_run(g_server);
    
    /* Úklid */
    server_destroy(g_server);
    
    log_info("SERVER", "Server ukončený");
    
    return 0;
}
