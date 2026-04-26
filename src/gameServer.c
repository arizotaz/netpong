#include "../include/game.h"
#include <curses.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void CreateGameServer(Server* serv, GameData* gd)
{

    Server s = *serv;

    bool game_running = true;

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(s.server_fd, &fds);

    struct timeval tv = { 0, 0 }; // non-blocking check

    SetupTermWin();

    DrawWalls(*gd);

    while (game_running) {

        fd_set read_fds;
        FD_ZERO(&read_fds);

        // Add server socket
        FD_SET(s.server_fd, &read_fds);
        int max_fd = s.server_fd;

        // Add client sockets
        for (int i = 0; i < s.client_count; i++) {
            int fd = s.clients[i];
            FD_SET(fd, &read_fds);
            if (fd > max_fd)
                max_fd = fd;
        }

        // Small timeout (prevents 100% CPU usage)
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 1000; // 1ms

        int ret = select(max_fd + 1, &read_fds, NULL, NULL, &tv);

        if (ret < 0) {
            perror("select");
            break;
        }

        // New connection
        if (FD_ISSET(s.server_fd, &read_fds)) {
            int client_fd = accept(s.server_fd, NULL, NULL);
            if (client_fd >= 0 && s.client_count < MAX_SERVER_CLIENTS) {
                s.clients[s.client_count++] = client_fd;

                char msg[64];
                snprintf(msg, sizeof(msg), "New client connected: %d\n", client_fd);
                GameAlert(msg);
            }
        }

        // Handle client data
        for (int i = 0; i < s.client_count; i++) {
            int fd = s.clients[i];

            if (FD_ISSET(fd, &read_fds)) {
                char buffer[1024];
                int bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);

                if (bytes > 0) {
                    buffer[bytes] = '\0';

                    char msg[1024];
                    snprintf(msg, sizeof(msg), "Client %d says: %s\r", fd, buffer);
                    GameAlert(msg);

                    // Example: echo back
                    send(fd, buffer, bytes, 0);
                } else if (bytes == 0) {
                    char msg[64];
                    snprintf(msg, sizeof(msg), "Client %d disconnected\n", fd);
                    GameAlert(msg);

                    RemoveClient(&s, i);
                    i--; // re-check swapped client
                } else {
                    perror("recv");
                    RemoveClient(&s, i);
                    i--;
                }
            }
        }

        if (s.client_count < 1) {
            char msg[64];
            snprintf(msg, sizeof(msg), "Waiting for other player\n");
            GameAlert(msg);
        } else {
            // Process Game Data
            
        }
    }

    CloseTermWin();
}

void RunGameClient(Client* client)
{
    Client c = *client;

    // char* msg = "Hello from client, this is a test, I really want to see what happens if I create a really long packet and try to send it.  I would imagine it will be some kind of buffer overflow";
    // send(c.client_fd, msg, strlen(msg), 0);

    // {
    //     char buffer[1024] = { 0 };
    //     read(c.client_fd, buffer, sizeof(buffer));
    //     printf("Server: %s\n", buffer);
    // }

    sleep(10);
    // char* msg2 = "Ok I am leaving now";
    // send(c.client_fd, msg2, strlen(msg2), 0);
    // {
    //     char buffer[1024] = { 0 };
    //     read(c.client_fd, buffer, sizeof(buffer));
    //     printf("Server: %s\n", buffer);
    // }
}