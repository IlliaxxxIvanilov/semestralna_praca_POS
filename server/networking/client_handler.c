#include "client_handler.h"
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include "../../shared/protocol.h"
#include "../simulation/simulation.h"

void *client_handler_handle(void *arg) {
    client_handler_arg_t *handler_arg = arg;
    int sock = handler_arg->sock;
    server_context_t *ctx = handler_arg->ctx;
    free(handler_arg);

    bool is_creator = false;

    pthread_mutex_lock(&ctx->mutex);
    if (ctx->client_count == 0) is_creator = true;
    server_context_add_client(ctx, sock, is_creator);
    pthread_mutex_unlock(&ctx->mutex);

    char buffer[MAX_MESSAGE_SIZE];

    while (ctx->server_running) {
        ssize_t n = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) break;
        buffer[n] = '\0';

        message_type_t type = protocol_deserialize_type(buffer);

        pthread_mutex_lock(&ctx->mutex);
        switch (type) {
            case MSG_CREATE_SIMULATION:
                if (is_creator && !ctx->simulation_running) {
                    sim_config_t cfg;
                    protocol_deserialize_data(type, buffer, &cfg);
                    simulation_start(ctx, &cfg);
                }
                break;
            case MSG_END_SIMULATION:
                if (is_creator) {
                    simulation_stop(ctx);
                    ctx->server_running = false;
                }
                break;
            case MSG_GET_STATE:
                server_context_broadcast(ctx, MSG_STATE_UPDATE, &ctx->parking_state);
                break;
            case MSG_GET_STATISTICS:
                server_context_broadcast(ctx, MSG_STATISTICS_UPDATE, &ctx->statistics);
                break;
            default:
                break;
        }
        pthread_mutex_unlock(&ctx->mutex);
    }

    pthread_mutex_lock(&ctx->mutex);
    server_context_remove_client(ctx, sock);
    pthread_mutex_unlock(&ctx->mutex);

    close(sock);
    return NULL;
}
