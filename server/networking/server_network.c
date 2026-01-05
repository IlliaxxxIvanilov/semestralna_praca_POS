#include "server_network.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "../utils/logger.h"
#include "client_handler.h"
#include "../../shared/constants.h"

void *server_network_thread(void *arg) {
    server_context_t *ctx = arg;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        logger_log("socket failed");
        return NULL;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(DEFAULT_PORT);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        logger_log("bind failed");
        close(server_fd);
        return NULL;
    }

    if (listen(server_fd, MAX_CLIENTS) == -1) {
        logger_log("listen failed");
        close(server_fd);
        return NULL;
    }

    logger_log("Server pocuva na porte %d", DEFAULT_PORT);

    while (ctx->server_running) {
        int client_sock = accept(server_fd, NULL, NULL);
        if (client_sock == -1) continue;

        client_handler_arg_t *argc = malloc(sizeof(client_handler_arg_t));
        argc->sock = client_sock;
        argc->ctx = ctx;

        pthread_t th;
        pthread_create(&th, NULL, client_handler_handle, argc);
        pthread_detach(th);
    }

    close(server_fd);
    return NULL;
}
