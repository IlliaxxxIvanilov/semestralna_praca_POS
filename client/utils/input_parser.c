#include "input_parser.h"
#include <stdio.h>

void input_read_simulation_config(sim_config_t *cfg) {
    if (!cfg) return;

    printf("Zadaj pocet parkovacich miest: ");
    scanf("%d", &cfg->num_spots);

    printf("Rezim parkovania (0 = bez cakania, 1 = s cakanim): ");
    scanf("%d", (int *)&cfg->mode);

    printf("Dlzka simulacie (s): ");
    scanf("%d", &cfg->duration);

    printf("Minimalny interval prichodu (s): ");
    scanf("%d", &cfg->arrival_min);

    printf("Maximalny interval prichodu (s): ");
    scanf("%d", &cfg->arrival_max);

    printf("Minimalny cas parkovania (s): ");
    scanf("%d", &cfg->park_min);

    printf("Maximalny cas parkovania (s): ");
    scanf("%d", &cfg->park_max);
}

