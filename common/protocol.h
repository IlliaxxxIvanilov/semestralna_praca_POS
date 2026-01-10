#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include "config.h"

//enum pre napisane vsetky mozne spravy medzi klientom a serverom, hodniity ako cisla
typedef enum {
    MSG_CONNECT = 1,          
    MSG_CONNECT_ACK = 2,       
  MSG_CREATE_SIM = 3,       
  MSG_SIM_CREATED = 4,      
    MSG_JOIN_SIM = 5,          
  MSG_SIM_JOINED = 6,        
  MSG_GET_STATE = 7,         
    MSG_STATE_UPDATE = 8,      
    MSG_GET_STATS = 9,         
    MSG_STATS_UPDATE = 10,     
    MSG_STOP_SIM = 11,         
    MSG_SIM_STOPPED = 12,      
    MSG_DISCONNECT = 13,       
    MSG_ERROR = 14             
} MessageType;

//konfiguracia samotnej simulacie
typedef struct {
    uint32_t parking_spots;           
    ParkingMode mode;                 
    uint32_t duration_sec;            
    uint32_t min_arrival_interval;    
    uint32_t max_arrival_interval;    
    uint32_t min_parking_time;        
    uint32_t max_parking_time;        
} SimulationConfig;

//stavy co nadobude parkovisko
typedef struct {
    uint32_t total_spots;            
  uint32_t occupied_spots;     
    uint32_t waiting_vehicles;   
    uint32_t elapsed_time;       
    uint32_t remaining_time;     
    ParkingMode mode;          //aky mame rezim parkovania  
} ParkingState;

//statistika parkovani
typedef struct {
    uint32_t total_arrivals;        
    uint32_t total_parked;          
    uint32_t total_left_no_space;   
    uint32_t total_departed;        
    uint32_t current_parked;        
    uint32_t current_waiting;       
    float avg_parking_time;         
    float avg_wait_time;            
    float occupancy_rate;         
} SimulationStats;

// hlavicka spravy?
typedef struct {
    MessageType type;     
    uint32_t length;      
} MessageHeader;

//hadzanie erorrov!
typedef enum {
    ERR_NONE = 0,
    ERR_SIM_EXISTS = 1,        
    ERR_SIM_NOT_FOUND = 2,     
    ERR_INVALID_CONFIG = 3,    
    ERR_NOT_CONNECTED = 4,     
    ERR_ALREADY_CONNECTED = 5, 
    ERR_INTERNAL = 6           
} ErrorCode;

///vypis pre chybovu sravu
typedef struct {
    ErrorCode code;
    char message[256];
} ErrorMessage;


int send_message_header(int sockfd, MessageType type, uint32_t length);


int recv_message_header(int sockfd, MessageHeader *header);


int send_data(int sockfd, const void *data, uint32_t length);


int recv_data(int sockfd, void *buffer, uint32_t length);


int send_simulation_config(int sockfd, const SimulationConfig *config);


int recv_simulation_config(int sockfd, SimulationConfig *config);


int send_parking_state(int sockfd, const ParkingState *state);


int recv_parking_state(int sockfd, ParkingState *state);

int send_statistics(int sockfd, const SimulationStats *stats);


int recv_statistics(int sockfd, SimulationStats *stats);


int send_error(int sockfd, ErrorCode code, const char *message);


int recv_error(int sockfd, ErrorMessage *error);

#endif
