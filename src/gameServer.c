#include "../include/game.h"
#include "../include/network/packet.h"
#include "../include/object.h"
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
    int scores[2] = { 0, 0 };

    int numOfBalls = 1;
    bool game_running = true;

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(s.server_fd, &fds);

    SetupTermWin();

    DrawWalls(*gd);
    DrawHeader(gd, scores, 0);
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

                numOfBalls = NUM_OF_BALLS;
                HideBall(&ball);
                for (int i = 0; i < 2; ++i)
                    scores[i] = 0;
                DrawWalls(*gd);
                DrawHeader(gd, scores, numOfBalls);

                char bmsg[1024];
                snprintf(bmsg, sizeof(bmsg), "NUM_OF_BALLS %d", numOfBalls);
                send(s.clients[0], bmsg, strlen(bmsg), 0);
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

                    if (!ProcessBallPacket(buffer, &ball, gd)) {
                        SendGameStatus(s.clients[0], "Ball is in other court");
                        MGameAlert(BOT_ROW + 1, LEFT_COL, "");
                    }
                    ProcessAlert(buffer);
                    ProcessGMSG(buffer);
                    if (!ProcessBallLost(buffer, s.clients[0], gd, scores, &allowServe, numOfBalls)) {
                        numOfBalls--;
                        DrawHeader(gd, scores, numOfBalls);
                        char bmsg[1024];
                        snprintf(bmsg, sizeof(bmsg), "NUM_OF_BALLS %d", numOfBalls);
                        send(s.clients[0], bmsg, strlen(bmsg), 0);
                    }
                    if (!ProcessScore(buffer, scores)) {
                        DrawHeader(gd, scores, numOfBalls);
                    }

                    // Example: echo back
                    // send(fd, buffer, bytes, 0);
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
            if (CheckBallCourtChange(gd, &ball)) {
                GameAlert("Ball is in other court");
                HideBall(&ball);

                char msg[1024];
                snprintf(msg, sizeof(msg), "BTRANS %f %f %f", ball.y, ball.vx, ball.vy);
                send(s.clients[0], msg, strlen(msg), 0);
            }
            if (CheckBallLose(gd, &ball)) {
                HideBall(&ball);
                DrawWalls(*gd);
                numOfBalls--;
                DrawHeader(gd, scores, numOfBalls);
                GameAlert("You lost the ball, other player serves");
                char bmsg[1024];
                snprintf(bmsg, sizeof(bmsg), "NUM_OF_BALLS %d", numOfBalls);
                send(s.clients[0], bmsg, strlen(bmsg), 0);
                sleep(1);
                char msg[1024];
                snprintf(msg, sizeof(msg), "BALL_LOST Other player point");
                send(s.clients[0], msg, strlen(msg), 0);
            }
        }

        if (numOfBalls == 0) {

            sleep(1);
            if (scores[0] == scores[1]) {
                MGameAlert(BOT_ROW + 1, LEFT_COL, "Game over, it's a tie");
                SendGameStatus(s.clients[0], "Game over, it's a tie");
            }
            if (scores[0] > scores[1]) {
                MGameAlert(BOT_ROW + 1, LEFT_COL, "Game over, you win!");
                SendGameStatus(s.clients[0], "Game over, you lost :(");
            }
            if (scores[0] < scores[1]) {
                MGameAlert(BOT_ROW + 1, LEFT_COL, "Game over, you lost :(");
                SendGameStatus(s.clients[0], "Game over, you win!");
            }

            sleep(3);
            ch = 'q';
        }

        if (ch == 'q') {
            GameAlert("Quitting...");
            sleep(1);
            game_running = false;
            break;
        }

        if (ch == ' ' && allowServe) {
            allowServe = false;
            ServeBall(&ball, gd);
            MGameAlert(BOT_ROW + 1, LEFT_COL, "");
            SendGameStatus(s.clients[0], "Ball is in other court");
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
