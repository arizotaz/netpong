#ifndef GAME_NETWORKING_H
#define GAME_NETWORKING_H 1

#include <netinet/in.h>

#define MAX_SERVER_CLIENTS 1

typedef struct {
    int server_fd;
    struct sockaddr_in address;
    int clients[2];
    int clientID;
    int client_count;
} Server;

/**
 * Setup the actual socket server
 */
Server StartServer(int port);

/**
 * Removes client and closes the connection
 */
void RemoveClient(Server* s, int index);

void KillServer();

typedef struct {
    int client_fd;
    struct sockaddr_in address;
} Client;

/**
 * Creates the Client object
 */
Client StartClient(char* host, int port);

#endif