#include "client_handler.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <sys/time.h>
#include "../../shared/protocol.h"
#include "../simulation/simulation.h"

void *client_handler_handle(void *arg) {
    client_handler_arg_t *handler_arg = arg;
    int sock = handler_arg->sock;
    server_context_t *ctx = handler_arg->ctx;
    free(handler_arg);

    bool is_creator = false;

    pthread_mutex_lock(&ctx->mutex);
    if (ctx->client_count == 0)
        is_creator = true;
    server_context_add_client(ctx, sock, is_creator);
    pthread_mutex_unlock(&ctx->mutex);

    printf("[SERVER] Klient pripojeny, socket=%d, is_creator=%d\n", sock, is_creator);
    fflush(stdout);

    // Nastav timeout na recv() - 1 sekunda
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    char buffer[MAX_MESSAGE_SIZE];

    while (ctx->server_running) {
        ssize_t n = recv(sock, buffer, sizeof(buffer), 0);
        
        if (n < 0) {
            // Timeout alebo chyba
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // Timeout - pokračuj v cykle
                continue;
            }
            // Iná chyba - klient sa odpojil
            printf("[SERVER] recv error na sockete %d\n", sock);
            fflush(stdout);
            break;
        }
        
        if (n == 0) {
            printf("[SERVER] Klient sa odpojil, socket=%d\n", sock);
            fflush(stdout);
            break;
        }

        printf("[SERVER] Prijatych %zd bajtov od socketu %d\n", n, sock);
        printf("[SERVER] Obsah: ");
        for (ssize_t i = 0; i < n && i < 20; i++) {
            printf("%02x ", (unsigned char)buffer[i]);
        }
        printf("\n");
        fflush(stdout);

        message_type_t type = protocol_deserialize_type(buffer);
        
        printf("[SERVER] Prijata sprava typu: %d od socketu: %d\n", type, sock);
        fflush(stdout);

        pthread_mutex_lock(&ctx->mutex);

        switch (type) {
        case MSG_SIM_CONFIG:
            printf("[SERVER] Spracovavam MSG_SIM_CONFIG\n");
            fflush(stdout);
            if (is_creator && !ctx->simulation_running) {
                sim_config_t cfg;
                protocol_deserialize_data(type, buffer, &cfg);
                simulation_start(ctx, &cfg);
                printf("[SERVER] Simulacia spustena\n");
                fflush(stdout);
            }
            break;

        case MSG_STOP_SIMULATION:
            printf("[SERVER] Spracovavam MSG_STOP_SIMULATION\n");
            fflush(stdout);
            if (is_creator) {
                simulation_stop(ctx);
                ctx->server_running = false;
            }
            break;

        case MSG_GET_STATE: {
            printf("[SERVER] Spracovavam MSG_GET_STATE\n");
            fflush(stdout);
            
            // Pošli stav parkoviska len tomuto klientovi
            char response[MAX_MESSAGE_SIZE];
            size_t response_size = protocol_serialize_message(
                MSG_STATE_UPDATE,
                &ctx->parking_state,
                sizeof(parking_state_t),
                response,
                sizeof(response)
            );
            
            printf("[SERVER] Posielam stav parkoviska, velkost: %zu\n", response_size);
            fflush(stdout);
            
            ssize_t sent = send(sock, response, response_size, 0);
            printf("[SERVER] Odoslanych bajtov: %zd\n", sent);
            fflush(stdout);
            break;
        }

        case MSG_GET_STATS: {
            printf("[SERVER] Spracovavam MSG_GET_STATS\n");
            fflush(stdout);
            
            // Pošli štatistiky len tomuto klientovi
            char response[MAX_MESSAGE_SIZE];
            size_t response_size = protocol_serialize_message(
                MSG_STATISTICS_UPDATE,
                &ctx->statistics,
                sizeof(statistics_t),
                response,
                sizeof(response)
            );
            
            printf("[SERVER] Posielam statistiky, velkost: %zu\n", response_size);
            fflush(stdout);
            
            ssize_t sent = send(sock, response, response_size, 0);
            printf("[SERVER] Odoslanych bajtov: %zd\n", sent);
            fflush(stdout);
            break;
        }

        default:
            printf("[SERVER] Neznamy typ spravy: %d\n", type);
            fflush(stdout);
            break;
        }

        pthread_mutex_unlock(&ctx->mutex);
    }

    pthread_mutex_lock(&ctx->mutex);
    server_context_remove_client(ctx, sock);
    pthread_mutex_unlock(&ctx->mutex);

    close(sock);
    printf("[SERVER] Handler ukonceny pre socket %d\n", sock);
    fflush(stdout);
    return NULL;
}
