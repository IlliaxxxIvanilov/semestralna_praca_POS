#include "protocol.h"
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int send_message_header(int sockfd, MessageType type, uint32_t length) {
    MessageHeader header;
    header.type = htonl(type);
    header.length = htonl(length);
    
    ssize_t sent = send(sockfd, &header, sizeof(MessageHeader), 0);
    if (sent != sizeof(MessageHeader)) {
        return -1;
    }
    return 0;
}

int recv_message_header(int sockfd, MessageHeader *header) {
    if (!header) {
        return -1;
    }
    
    ssize_t received = recv(sockfd, header, sizeof(MessageHeader), MSG_WAITALL);
    if (received != sizeof(MessageHeader)) {
        return -1;
    }
    
    header->type = ntohl(header->type);
    header->length = ntohl(header->length);
    
    return 0;
}

int send_data(int sockfd, const void *data, uint32_t length) {
    if (!data || length == 0) {
        return -1;
    }
    
    ssize_t sent = send(sockfd, data, length, 0);
    if (sent != (ssize_t)length) {
        return -1;
    }
    return 0;
}

int recv_data(int sockfd, void *buffer, uint32_t length) {
    if (!buffer || length == 0) {
        return -1;
    }
    
    ssize_t received = recv(sockfd, buffer, length, MSG_WAITALL);
    if (received != (ssize_t)length) {
        return -1;
    }
    return 0;
}

int send_simulation_config(int sockfd, const SimulationConfig *config) {
    if (!config) {
        return -1;
    }
    
    //presmerovanie konverzie na network mode...

    SimulationConfig net_config;
    net_config.parking_spots = htonl(config->parking_spots);
    net_config.mode = htonl(config->mode);
    net_config.duration_sec = htonl(config->duration_sec);
    net_config.min_arrival_interval = htonl(config->min_arrival_interval);
    net_config.max_arrival_interval = htonl(config->max_arrival_interval);
    net_config.min_parking_time = htonl(config->min_parking_time);
    net_config.max_parking_time = htonl(config->max_parking_time);
    
    if (send_message_header(sockfd, MSG_CREATE_SIM, sizeof(SimulationConfig)) != 0) {
        return -1;
    }
    
    return send_data(sockfd, &net_config, sizeof(SimulationConfig));
}

int recv_simulation_config(int sockfd, SimulationConfig *config) {
    if (!config) {
        return -1;
    }
    
    SimulationConfig net_config;
    if (recv_data(sockfd, &net_config, sizeof(SimulationConfig)) != 0) {
        return -1;
    }
    
    /// tu je konverzia pre network mode z bytov
    config->parking_spots = ntohl(net_config.parking_spots);
    config->mode = ntohl(net_config.mode);
    config->duration_sec = ntohl(net_config.duration_sec);
    config->min_arrival_interval = ntohl(net_config.min_arrival_interval);
    config->max_arrival_interval = ntohl(net_config.max_arrival_interval);
    config->min_parking_time = ntohl(net_config.min_parking_time);
    config->max_parking_time = ntohl(net_config.max_parking_time);
    
    return 0;
}

int send_parking_state(int sockfd, const ParkingState *state) {
    if (!state) {
        return -1;
    }
    
    /// ta ista konverzia pre byte mode, lenze ina metoda ako stav parkoviska
    ParkingState net_state;
    net_state.total_spots = htonl(state->total_spots);
    net_state.occupied_spots = htonl(state->occupied_spots);
    net_state.waiting_vehicles = htonl(state->waiting_vehicles);
    net_state.elapsed_time = htonl(state->elapsed_time);
    net_state.remaining_time = htonl(state->remaining_time);
    net_state.mode = htonl(state->mode);
    
    if (send_message_header(sockfd, MSG_STATE_UPDATE, sizeof(ParkingState)) != 0) {
        return -1;
    }
    
    return send_data(sockfd, &net_state, sizeof(ParkingState));
}

int recv_parking_state(int sockfd, ParkingState *state) {
    if (!state) {
        return -1;
    }
    
    ParkingState net_state;
    if (recv_data(sockfd, &net_state, sizeof(ParkingState)) != 0) {
        return -1;
    }
    
    //konverzia na recieve po sprave sen(). konverzia bytov
    state->total_spots = ntohl(net_state.total_spots);
    state->occupied_spots = ntohl(net_state.occupied_spots);
    state->waiting_vehicles = ntohl(net_state.waiting_vehicles);
    state->elapsed_time = ntohl(net_state.elapsed_time);
    state->remaining_time = ntohl(net_state.remaining_time);
    state->mode = ntohl(net_state.mode);
    
    return 0;
}

int send_statistics(int sockfd, const SimulationStats *stats) {
    if (!stats) {
        return -1;
    }
    
    
    SimulationStats net_stats;
    net_stats.total_arrivals = htonl(stats->total_arrivals);
    net_stats.total_parked = htonl(stats->total_parked);
    net_stats.total_left_no_space = htonl(stats->total_left_no_space);
    net_stats.total_departed = htonl(stats->total_departed);
    net_stats.current_parked = htonl(stats->current_parked);
    net_stats.current_waiting = htonl(stats->current_waiting);
    
    /* Pre float použijeme memcpy (predpokladáme IEEE 754) */
    memcpy(&net_stats.avg_parking_time, &stats->avg_parking_time, sizeof(float));
    memcpy(&net_stats.avg_wait_time, &stats->avg_wait_time, sizeof(float));
    memcpy(&net_stats.occupancy_rate, &stats->occupancy_rate, sizeof(float));
    
    if (send_message_header(sockfd, MSG_STATS_UPDATE, sizeof(SimulationStats)) != 0) {
        return -1;
    }
    
    return send_data(sockfd, &net_stats, sizeof(SimulationStats));
}

int recv_statistics(int sockfd, SimulationStats *stats) {
    if (!stats) {
        return -1;
    }
    
    SimulationStats net_stats;
    if (recv_data(sockfd, &net_stats, sizeof(SimulationStats)) != 0) {
        return -1;
    }
    
    
    stats->total_arrivals = ntohl(net_stats.total_arrivals);
    stats->total_parked = ntohl(net_stats.total_parked);
    stats->total_left_no_space = ntohl(net_stats.total_left_no_space);
    stats->total_departed = ntohl(net_stats.total_departed);
    stats->current_parked = ntohl(net_stats.current_parked);
    stats->current_waiting = ntohl(net_stats.current_waiting);
    
    /* Pre float použijeme memcpy */
    memcpy(&stats->avg_parking_time, &net_stats.avg_parking_time, sizeof(float));
    memcpy(&stats->avg_wait_time, &net_stats.avg_wait_time, sizeof(float));
    memcpy(&stats->occupancy_rate, &net_stats.occupancy_rate, sizeof(float));
    
    return 0;
}

int send_error(int sockfd, ErrorCode code, const char *message) {
    ErrorMessage error;
    error.code = htonl(code);
    
    if (message) {
        strncpy(error.message, message, sizeof(error.message) - 1);
        error.message[sizeof(error.message) - 1] = '\0';
    } else {
        error.message[0] = '\0';
    }
    
    if (send_message_header(sockfd, MSG_ERROR, sizeof(ErrorMessage)) != 0) {
        return -1;
    }
    
    return send_data(sockfd, &error, sizeof(ErrorMessage));
}

int recv_error(int sockfd, ErrorMessage *error) {
    if (!error) {
        return -1;
    }
    
    if (recv_data(sockfd, error, sizeof(ErrorMessage)) != 0) {
        return -1;
    }
    
    error->code = ntohl(error->code);
    error->message[sizeof(error->message) - 1] = '\0';
    
    return 0;
}
