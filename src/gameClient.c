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

void RunGameClient(Client* client, GameData* gd)
{
    Client c = *client;

    Paddle paddle = CreatePaddle(gd);
    Ball ball = CreateBall(gd);
    int scores[2] = { 0, 0 };

    long lastBallUpdate = GetCurrentMS();

    int numOfBalls = 1;
    bool game_running = true;

    SetupTermWin();

    GameAlert("Connected to server");
    MGameAlert(BOT_ROW + 1, LEFT_COL, "Press space to Serve");

    int allowServe = true;

    DrawWalls(*gd);
    DrawPaddle(&paddle);
    DrawHeader(gd, scores, numOfBalls);

    SendGameStatus(c.client_fd,"Waiting for serve");

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
                GameAlert(buffer);

                if (!ProcessBallPacket(buffer, &ball, gd)) {SendGameStatus(c.client_fd,"Ball is in other court");
                    MGameAlert(BOT_ROW+1,LEFT_COL,"");}
                ProcessAlert(buffer);
                ProcessGMSG(buffer);
                if (!ProcessNumOfBalls(buffer, &numOfBalls)) {
                    DrawHeader(gd, scores, numOfBalls);
                }
                ProcessBallLost(buffer, c.client_fd, gd, scores, &allowServe, numOfBalls);
                if (!ProcessScore(buffer, scores)) {
                    DrawHeader(gd, scores, numOfBalls);
                }

            } else if (bytes == 0) {
                GameAlert("Server disconnected.  Quitting...");
                sleep(2);
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
            if (CheckBallCourtChange(gd, &ball)) {
                GameAlert("Ball is in other court");
                HideBall(&ball);

                char msg[1024];
                snprintf(msg, sizeof(msg), "BTRANS %f %f %f", ball.y, ball.vx, ball.vy);
                send(c.client_fd, msg, strlen(msg), 0);
            }
            if (CheckBallLose(gd, &ball)) {
                HideBall(&ball);
                DrawWalls(*gd);
                GameAlert("You lost the ball, other player serves");
                char msg[1024];
                snprintf(msg, sizeof(msg), "BALL_LOST Other player point");
                send(c.client_fd, msg, strlen(msg), 0);
            }
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
            SendGameStatus(c.client_fd,"Ball is in other court");
        }
    }

    CloseTermWin();
}