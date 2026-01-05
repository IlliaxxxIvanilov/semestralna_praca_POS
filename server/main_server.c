#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include "server_context.h"
#include "simulation/simulation.h"
#include "networking/server_network.h"
#include "utils/logger.h"

static server_context_t *sigint_ctx = NULL;

static void sigint_handler(int sig) {
    (void)sig;
    if (sigint_ctx && sigint_ctx->shutdown_pipe[1] != -1) {
        char byte = 'x';
        write(sigint_ctx->shutdown_pipe[1], &byte, 1);
    }
}

int main(void) {
    logger_init("server.log");
    logger_log("Server spusteny");

    server_context_t ctx;
    server_context_init(&ctx);

    sigint_ctx = &ctx;

    signal(SIGINT, sigint_handler);

    ctx.server_running = true;

    pthread_t sim_thread;
    pthread_t net_thread;

    pthread_create(&sim_thread, NULL, simulation_run_thread, &ctx);
    pthread_create(&net_thread, NULL, server_network_thread, &ctx);

    char buf[1];
    while (ctx.server_running) {
        if (read(ctx.shutdown_pipe[0], buf, 1) > 0) {
            ctx.server_running = false;
            logger_log("Ukoncenie cez Ctrl+C");
        }
        usleep(200000);
    }

    logger_log("Ukoncujem server...");
    pthread_join(sim_thread, NULL);
    pthread_join(net_thread, NULL);

    server_context_destroy(&ctx);
    logger_destroy();

    return 0;
}