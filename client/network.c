#include "network.h"
#include "../common/utils.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

int network_connect(const char *host, int port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        log_error("CLIENT", "Nepodarilo sa vytvoriť socket");
        return -1;
    }
    
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, host, &server_addr.sin_addr) <= 0) {
        log_error("CLIENT", "Neplatná adresa servera");
        close(sockfd);
        return -1;
    }
    
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        log_error("CLIENT", "Nepodarilo sa pripojiť k serveru");
        close(sockfd);
        return -1;
    }
    
    log_debug("CLIENT", "Pripojený na server (socket=%d)", sockfd);
    
    return sockfd;
}

void network_disconnect(int sockfd) {
    if (sockfd >= 0) {
        close(sockfd);
        log_debug("CLIENT", "Odpojený od servera");
    }
}

int network_create_simulation(int sockfd, const SimulationConfig *config) {
    if (sockfd < 0 || !config) {
        return -1;
    }
    
    log_debug("CLIENT", "Posielam MSG_CREATE_SIM");
    
    /* Odoslanie konfigurácie */
    if (send_simulation_config(sockfd, config) != 0) {
        log_error("CLIENT", "Nepodarilo sa odoslať konfiguráciu");
        return -1;
    }
    
    /* Čakanie na odpoveď */
    MessageHeader header;
    if (recv_message_header(sockfd, &header) != 0) {
        log_error("CLIENT", "Nepodarilo sa prijať odpoveď");
        return -1;
    }
    
    if (header.type == MSG_SIM_CREATED) {
        log_info("CLIENT", "Simulácia vytvorená a beží na serveri");
        return 0;
    } else if (header.type == MSG_ERROR) {
        ErrorMessage error;
        if (recv_error(sockfd, &error) == 0) {
            log_error("CLIENT", "Chyba: %s", error.message);
        }
        return -1;
    }
    
    return -1;
}

int network_join_simulation(int sockfd) {
    if (sockfd < 0) {
        return -1;
    }
    
    log_debug("CLIENT", "Posielam MSG_CONNECT");
    
    /* Odoslanie správy MSG_CONNECT */
    if (send_message_header(sockfd, MSG_CONNECT, 0) != 0) {
        log_error("CLIENT", "Nepodarilo sa odoslať MSG_CONNECT");
        return -1;
    }
    
    /* OPRAVA: Nečakáme na MSG_CONNECT_ACK
     * Server priamo začne spracovávať príkazy */
    log_info("CLIENT", "Pripojený k simulácii");
    return 0;
}

int network_get_state(int sockfd, ParkingState *state) {
    if (sockfd < 0 || !state) {
        return -1;
    }
    
    /* Odoslanie žiadosti */
    if (send_message_header(sockfd, MSG_GET_STATE, 0) != 0) {
        return -1;
    }
    
    /* Prijatie hlavičky odpovede */
    MessageHeader header;
    if (recv_message_header(sockfd, &header) != 0) {
        return -1;
    }
    
    if (header.type == MSG_STATE_UPDATE) {
        return recv_parking_state(sockfd, state);
    } else if (header.type == MSG_ERROR) {
        ErrorMessage error;
        recv_error(sockfd, &error);
        return -1;
    }
    
    return -1;
}

int network_get_statistics(int sockfd, SimulationStats *stats) {
    if (sockfd < 0 || !stats) {
        return -1;
    }
    
    /* Odoslanie žiadosti */
    if (send_message_header(sockfd, MSG_GET_STATS, 0) != 0) {
        return -1;
    }
    
    /* Prijatie hlavičky odpovede */
    MessageHeader header;
    if (recv_message_header(sockfd, &header) != 0) {
        return -1;
    }
    
    if (header.type == MSG_STATS_UPDATE) {
        return recv_statistics(sockfd, stats);
    } else if (header.type == MSG_ERROR) {
        ErrorMessage error;
        recv_error(sockfd, &error);
        return -1;
    }
    
    return -1;
}

int network_send_disconnect(int sockfd) {
    if (sockfd < 0) {
        return -1;
    }
    
    return send_message_header(sockfd, MSG_DISCONNECT, 0);
}
