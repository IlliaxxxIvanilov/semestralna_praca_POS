#include "client.h"
#include "../common/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include "../common/config.h"

int main(int argc, char *argv[]) {
    //inicializujeme generator nah cisel!
    init_random(0);
    
    ///dalej vytvarame klienta (klientov)
    Client *client = client_create();
    if (!client) {
        fprintf(stderr, "Nepodarilo sa vytvoriť klienta\n");
        return 1;
    }
    
    //parsovanei vsetkych argumentov 
    if (argc > 1) {
        

        client->server_port = DEFAULT_PORT;
        if (argc > 2) {
            client->server_port = atoi(argv[2]);
        }
    }
    
    //hlavna funkcia klienta
    client_run(client);
    
    //destroy vymaz ukonci
    client_destroy(client);
    
    printf("Aplikácia ukončená.\n");
    
    return 0;
}
