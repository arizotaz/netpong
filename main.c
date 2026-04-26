#include "game.h"
#include <curses.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "network/networking.h"

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

    // srand(getpid());

    // // Setup Terminal
    // if (initscr() == NULL) {
    //     perror("initscr failed");
    //     exit(1);
    // }
    // noecho();
    // cbreak();
    // nodelay(stdscr, TRUE);

    int game_running = true;

    if (data.host) {
        // Create Server
        Server s = StartServer(data.port);

        printf("Server Started\n");

        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(s.server_fd, &fds);

        struct timeval tv = { 0, 0 }; // non-blocking check

        int updateChar = 0;

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
                if (client_fd >= 0 && s.client_count < s.max_clients) {
                    s.clients[s.client_count++] = client_fd;
                    printf("New client connected: %d\n", client_fd);
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
                        printf("Client %d says: %s\n", fd, buffer);

                        // Example: echo back
                        send(fd, buffer, bytes, 0);
                    } else if (bytes == 0) {
                        printf("Client %d disconnected\n", fd);
                        RemoveClient(&s, i);
                        i--; // re-check swapped client
                    } else {
                        perror("recv");
                        RemoveClient(&s, i);
                        i--;
                    }
                }
            }

            
        }

        close(s.server_fd);

    } else {

        printf("Connecting to %s:%i\n", data.domain, data.port);

        // Create Client

        Client c = StartClient(data.domain, data.port);

        char* msg = "Hello from client";
        send(c.client_fd, msg, strlen(msg), 0);

        {
            char buffer[1024] = { 0 };
            read(c.client_fd, buffer, sizeof(buffer));
            printf("Server: %s\n", buffer);
        }

        sleep(10);
        char* msg2 = "Ok I am leaving now";
        send(c.client_fd, msg2, strlen(msg2), 0);
        {
            char buffer[1024] = { 0 };
            read(c.client_fd, buffer, sizeof(buffer));
            printf("Server: %s\n", buffer);
        }

        close(c.client_fd);
        return 0;
    }

    // endwin();
}