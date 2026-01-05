#ifndef SERVER_CONTEXT_H
#define SERVER_CONTEXT_H

#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include "../shared/dto.h"
#include "../shared/constants.h"
#include "../shared/protocol.h"

typedef struct ClientConnection {
    int socket;
    bool is_creator;
} client_connection_t;

typedef struct ServerContext {
    pthread_mutex_t mutex;
    pthread_cond_t  state_changed_cond;

    bool            server_running;
    bool            simulation_running;

    parking_state_t parking_state;
    statistics_t    statistics;
    sim_config_t    current_config;

    client_connection_t clients[MAX_CLIENTS];
    int             client_count;

    int             shutdown_pipe[2]; 
} server_context_t;


void server_context_destroy(server_context_t *ctx);
void server_context_init(server_context_t *ctx);

int  server_context_add_client(server_context_t *ctx, int sock, bool is_creator);
void server_context_remove_client(server_context_t *ctx, int sock);
void server_context_broadcast(server_context_t *ctx, message_type_t type, const void *data);

#endif
