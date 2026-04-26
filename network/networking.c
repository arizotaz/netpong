#include "networking.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

Server StartServer(int port) {
    Server s = {2};
    s.clientID = 0;


    // Create Socket
    if ((s.server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set Address
    s.address.sin_family = AF_INET;
    s.address.sin_addr.s_addr = INADDR_ANY; // listen on all interfaces
    s.address.sin_port = htons(port);

    // Bind Port
    if (bind(s.server_fd, (struct sockaddr *)&s.address, sizeof(s.address)) < 0) {
        perror("bind failed");
        close(s.server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(s.server_fd, 3) < 0) {
        perror("listen");
        close(s.server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", port);

    return s;
}

void RemoveClient(Server* s, int index) {
    close(s->clients[index]);
    s->clients[index] = s->clients[s->client_count - 1];
    s->client_count--;
}

Client StartClient(char* host, int port) {    

    Client c;

    // Create socket
    if ((c.client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set address info
    c.address.sin_family = AF_INET;
    c.address.sin_port = htons(port);

    // Convert hostname/IP to binary form
    if (inet_pton(AF_INET, host, &c.address.sin_addr) <= 0) {
        // If not a direct IP, try resolving hostname
        struct hostent* he = gethostbyname(host);
        if (he == NULL) {
            perror("gethostbyname failed");
            close(c.client_fd);
            exit(EXIT_FAILURE);
        }
        memcpy(&c.address.sin_addr, he->h_addr_list[0], he->h_length);
    }

    // Connect to server
    if (connect(c.client_fd, (struct sockaddr*)&c.address, sizeof(c.address)) < 0) {
        perror("connect failed");
        close(c.client_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to %s:%d\n", host, port);

    return c;
}


