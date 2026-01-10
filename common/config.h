#ifndef CONFIG_H
#define CONFIG_H

//konstanty pr  server a spravanie sieti
#define DEFAULT_PORT 8888
#define MAX_CLIENTS 10
#define BUFFER_SIZE 4096

//konsty pre simulaciu
#define MAX_PARKING_SPOTS 200
#define MAX_VEHICLES 1000
#define MIN_SIMULATION_TIME 10
#define MAX_SIMULATION_TIME 3600

//konst na beh simulacie, tick bude ako na 100milisekund
#define SIMULATION_TICK_MS 100


#define DEBUG_MODE 1

//enum na vsetky rezimi parkovania, s cakanim a bez
typedef enum {
    PARKING_MODE_NO_WAIT = 0, 
    PARKING_MODE_WITH_WAIT = 1   
} ParkingMode;




//pomocka pre stavy v ktorych je vozidlo (vsetky mozne)
typedef enum {
    VEHICLE_STATE_WAITING = 0,  
    VEHICLE_STATE_PARKED = 1,    
    VEHICLE_STATE_LEFT = 2      
} VehicleState;

#endif
