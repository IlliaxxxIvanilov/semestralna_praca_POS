#ifndef CLIENT_H
#define CLIENT_H

//struct pre klienta
typedef struct {
    int socket;           
    int connected;        
    int in_simulation;    
    char server_host[256];
    int server_port;
} Client;


Client* client_create(void);


void client_destroy(Client *client);


int client_connect(Client *client, const char *host, int port);


void client_disconnect(Client *client);


void client_run(Client *client);

#endif 
