#include "include/game.h"

#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/**
 * Main Fucntion
 */
int main(int argc, char* argv[])
{
    // Create GameData Object
    GameData data = { false, 0, NULL, 0 };

    // Detect Server or Client
    switch (argc) {
    case 2:
        // Start Server
        data.host = true;
        data.playerNumber = 0;
        data.port = atoi(argv[1]);
        break;
    case 3:
        // Start Client
        data.playerNumber = 1;
        data.domain = argv[1];
        data.port = atoi(argv[2]);
        break;
    default:
        data.playerNumber = 0;
        printf("Please read the following usage\n");
        printf("Server: %s <portnumber>\n", argv[0]);
        printf("Client: %s <domain> <port>\n", argv[0]);
        return 1;
    }




    if (data.host) {
        // Create Server
        Server s = StartServer(data.port);
        printf("Server Started\n");

        CreateGameServer(&s,&data);

        close(s.server_fd);

    } else {

        printf("Connecting to %s:%i\n", data.domain, data.port);
        Client c = StartClient(data.domain, data.port);

        RunGameClient(&c,&data);

        close(c.client_fd);
        return 0;
    }
}