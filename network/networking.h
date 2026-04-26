#ifndef GAME_NETWORKING_H
#define GAME_NETWORKING_H 1

#include <netinet/in.h>

typedef struct {
        const int max_clients;
    int server_fd;
    struct sockaddr_in address;
    int clients[2];
    int clientID;
    int client_count;
} Server;

Server StartServer(int port);
void RemoveClient(Server* s, int index);

typedef struct {
    int client_fd;
    struct sockaddr_in address;
} Client;


Client StartClient(char* host, int port);

#endif