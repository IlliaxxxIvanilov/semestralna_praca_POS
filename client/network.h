#ifndef NETWORK_H
#define NETWORK_H

#include "../common/protocol.h"


int network_connect(const char *host, int port);


void network_disconnect(int sockfd);


int network_create_simulation(int sockfd, const SimulationConfig *config);


int network_join_simulation(int sockfd);


int network_get_state(int sockfd, ParkingState *state);


int network_get_statistics(int sockfd, SimulationStats *stats);


int network_send_disconnect(int sockfd);

#endif
