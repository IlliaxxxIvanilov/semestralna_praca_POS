#include "server_context.h"
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "../shared/protocol.h"
#include "unistd.h"

void server_context_init(server_context_t *ctx) {
    memset(ctx, 0, sizeof(*ctx));
    pthread_mutex_init(&ctx->mutex, NULL);
    pthread_cond_init(&ctx->state_changed_cond, NULL);

    ctx->server_running = true;
    ctx->simulation_running = false;
    ctx->client_count = 0;

    pipe(ctx->shutdown_pipe);
    fcntl(ctx->shutdown_pipe[0], F_SETFL, O_NONBLOCK);
}

void server_context_destroy(server_context_t *ctx) {
    pthread_mutex_destroy(&ctx->mutex);
    pthread_cond_destroy(&ctx->state_changed_cond);

    for (int i = 0; i < ctx->parking_state.num_spots; i++) {
        if (ctx->parking_state.spots[i].occupied &&
            ctx->parking_state.spots[i].vehicle) {
            free(ctx->parking_state.spots[i].vehicle);
        }
    }

    for (int i = 0; i < ctx->client_count; i++) {
        close(ctx->clients[i].socket);
    }

    close(ctx->shutdown_pipe[0]);
    close(ctx->shutdown_pipe[1]);
}

int server_context_add_client(server_context_t *ctx, int sock, bool is_creator) {
    if (ctx->client_count >= MAX_CLIENTS)
        return -1;

    ctx->clients[ctx->client_count].socket = sock;
    ctx->clients[ctx->client_count].is_creator = is_creator;
    ctx->client_count++;
    return ctx->client_count - 1;
}

void server_context_remove_client(server_context_t *ctx, int sock) {
    for (int i = 0; i < ctx->client_count; i++) {
        if (ctx->clients[i].socket == sock) {
            close(sock);
            memmove(&ctx->clients[i],
                    &ctx->clients[i + 1],
                    (ctx->client_count - i - 1) *
                        sizeof(client_connection_t));
            ctx->client_count--;
            return;
        }
    }
}

void server_context_broadcast(
    server_context_t *ctx,
    message_type_t type,
    const void *data,
    size_t data_size
) {
    char buffer[MAX_MESSAGE_SIZE];

    size_t msg_size = protocol_serialize_message(
        type,
        data,
        data ? data_size : 0,
        buffer,
        sizeof(buffer)
    );

    if (msg_size == 0)
        return;

    pthread_mutex_lock(&ctx->mutex);
    for (int i = 0; i < ctx->client_count; i++) {
        send(ctx->clients[i].socket, buffer, msg_size, 0);
    }
    pthread_mutex_unlock(&ctx->mutex);
}

