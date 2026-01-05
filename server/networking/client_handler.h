#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include "../server_context.h"

typedef struct ClientHandlerArg {
    int sock;
    server_context_t *ctx;
} client_handler_arg_t;

void *client_handler_handle(void *arg);

#endif