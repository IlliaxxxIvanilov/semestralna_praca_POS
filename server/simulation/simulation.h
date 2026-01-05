#ifndef SIMULATION_H
#define SIMULATION_H

#include "../server_context.h"

void *simulation_run_thread(void *arg);
void simulation_start(server_context_t *ctx, const sim_config_t *config);
void simulation_stop(server_context_t *ctx);

#endif