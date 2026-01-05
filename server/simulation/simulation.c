#include "simulation.h"
#include "simulation_config.h"
#include "time_manager.h"
#include "event_generator.h"
#include "../parking/parking_lot.h"
#include "../statistics/statistics.h"
#include "../utils/random_generator.h"
#include "../utils/logger.h"
#include "../../shared/constants.h"

#include <time.h>
#include <unistd.h>

void *simulation_run_thread(void *arg) {
    server_context_t *ctx = arg;

    random_generator_seed();

    while (ctx->server_running) {
        pthread_mutex_lock(&ctx->mutex);

        if (!ctx->simulation_running) {
            pthread_mutex_unlock(&ctx->mutex);
            break;
        }

        time_t now = time(NULL);

        event_generator_generate(&ctx->parking_state, &ctx->statistics, &ctx->current_config);

        parking_lot_process_departures(&ctx->parking_state, &ctx->statistics, now);

        if (ctx->parking_state.mode == WITH_WAITING) {
            parking_lot_process_queue(&ctx->parking_state, &ctx->statistics, now);
        }

        if (now - ctx->parking_state.start_time >= ctx->current_config.duration) {
            simulation_stop(ctx);
        }

        server_context_broadcast(ctx, MSG_STATE_UPDATE, &ctx->parking_state);
        server_context_broadcast(ctx, MSG_STATISTICS_UPDATE, &ctx->statistics);

        pthread_mutex_unlock(&ctx->mutex);
        sleep(SERVER_TICK_SECONDS);
    }

    return NULL;
}

void simulation_start(server_context_t *ctx, const sim_config_t *config) {
    pthread_mutex_lock(&ctx->mutex);
    ctx->current_config = *config;
    parking_lot_init(&ctx->parking_state, config->num_spots);
    ctx->parking_state.mode = config->mode;
    ctx->parking_state.running = true;
    ctx->parking_state.start_time = time(NULL);
    ctx->simulation_running = true;
    statistics_init(&ctx->statistics);
    pthread_mutex_unlock(&ctx->mutex);
}

void simulation_stop(server_context_t *ctx) {
    pthread_mutex_lock(&ctx->mutex);
    ctx->simulation_running = false;
    ctx->parking_state.running = false;
    server_context_broadcast(ctx, MSG_SIM_ENDED, NULL);
    pthread_mutex_unlock(&ctx->mutex);
}