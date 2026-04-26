#include "../include/game.h"
#include "../include/object.h"
#include "../include/network/packet.h"
#include <curses.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

long GetCurrentMS()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    long ms = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
    return ms;
}

void CreateGameServer(Server* serv, GameData* gd)
{

    Server s = *serv;
    Paddle paddle = CreatePaddle(gd);

    Ball ball = CreateBall(gd);
    long lastBallUpdate = GetCurrentMS();

    bool game_running = true;

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(s.server_fd, &fds);

    struct timeval tv = { 0, 0 }; // non-blocking check

    SetupTermWin();

    DrawWalls(*gd);
    DrawPaddle(&paddle);
    // DrawBall(&ball);

    GameAlert("Waiting for Client");
    int allowServe = false;

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

                    ProcessBallPacket(buffer,&ball,gd);

                    // Example: echo back
                    //send(fd, buffer, bytes, 0);
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

        char ch = getch();
        if (ch == 'w')
            MovePaddleUP(&paddle);
        if (ch == 's')
            MovePaddleDown(&paddle);

        // Process Ball only if on court
        if (ball.visible && s.client_count > 0) {
            if (GetCurrentMS() > lastBallUpdate + 10) {
                lastBallUpdate = GetCurrentMS();
                UpdateBall(&ball, &paddle);
            }
            if (CheckBallCourtChange(gd,&ball)) {
                GameAlert("Ball is in other court");
                HideBall(&ball);

                char msg[1024];
                snprintf(msg, sizeof(msg), "BTRANS %f %f %f", ball.y,ball.vx,ball.vy);
                send(s.clients[0], msg, strlen(msg), 0);
            }
        }

        if (ch == 'q') {
            GameAlert("Quitting...");
            sleep(1);
            game_running = false;
            break;
        }

        // if (s.client_count < 1) {
        //     char msg[64];
        //     snprintf(msg, sizeof(msg), "Waiting for other player\n");
        //     GameAlert(msg);
        // } else {
        //     // Process Game Data

        // }
    }

    CloseTermWin();
}

void RunGameClient(Client* client, GameData* gd)
{
    Client c = *client;

    Paddle paddle = CreatePaddle(gd);
    Ball ball = CreateBall(gd);


    long lastBallUpdate = GetCurrentMS();

    bool game_running = true;

    SetupTermWin();

    GameAlert("Connected to server");
    int allowServe = true;

    DrawWalls(*gd);
    DrawPaddle(&paddle);

    while (game_running) {

        fd_set read_fds;
        FD_ZERO(&read_fds);

        // Watch server socket
        FD_SET(c.client_fd, &read_fds);

        int max_fd = c.client_fd;

        // Small timeout (same idea as server)
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 1000; // 1ms

        int ret = select(max_fd + 1, &read_fds, NULL, NULL, &tv);

        if (ret < 0) {
            perror("select");
            break;
        }

        // Handle server messages
        if (FD_ISSET(c.client_fd, &read_fds)) {
            char buffer[1024];
            int bytes = recv(c.client_fd, buffer, sizeof(buffer) - 1, 0);

            if (bytes > 0) {
                buffer[bytes] = '\0';

                char msg[1024];
                snprintf(msg, sizeof(msg), "Server says: %s\r", buffer);
                GameAlert(msg);

                ProcessBallPacket(buffer,&ball,gd);

            } else if (bytes == 0) {
                GameAlert("Server disconnected");
                break;
            } else {
                perror("recv");
                break;
            }
        }

        char ch = getch();
        if (ch == 'w')
            MovePaddleUP(&paddle);
        if (ch == 's')
            MovePaddleDown(&paddle);

        // Process Ball only if on court
        if (ball.visible) {
            if (GetCurrentMS() > lastBallUpdate + 10) {
                lastBallUpdate = GetCurrentMS();
                UpdateBall(&ball, &paddle);
            }
            if (CheckBallCourtChange(gd,&ball)) {
                GameAlert("Ball is in other court");
                HideBall(&ball);

                char msg[1024];
                snprintf(msg, sizeof(msg), "BTRANS %f %f %f", ball.y,ball.vx,ball.vy);
                send(c.client_fd, msg, strlen(msg), 0);
            }
        }

        if (ch == 'q') {
            GameAlert("Quitting...");
            sleep(1);
            game_running = false;
            break;
        }

        if (ch == ' ') {
            allowServe = false;
            ServeBall(&ball,gd);
        }
    }

    CloseTermWin();
}