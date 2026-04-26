#include "../../include/network/packet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int ProcessBallPacket(char* msg, Ball* ball, GameData* gd)
{
    if (strncmp(msg, "BTRANS", 6) != 0) {
        return 1;
    }

    int matched = sscanf(msg, "BTRANS %f %f %f",
        &ball->y, &ball->vx, &ball->vy);
    if (matched != 3) {
        printf("Parsing failed\n");
        return 1;
    }

    if (gd->host)
        ball->x = RIGHT_COL - 1;
    else
        ball->x = LEFT_COL + 1;
    ball->visible = 1;

    return 0;
}

int ProcessAlert(char* msg)
{
    if (strncmp(msg, "ALERT:", 6) != 0) {
        return 1;
    }
    char* alertText = msg + 6;
    if (*alertText == ' ') {
        alertText++;
    }

    GameAlert(alertText);
    return 0;
}

int ProcessGMSG(char* msg)
{
    if (strncmp(msg, "GMSG:", 5) != 0) {
        return 1;
    }
    char* alertText = msg + 5;
    if (*alertText == ' ') {
        alertText++;
    }

    MGameAlert(BOT_ROW+1,LEFT_COL,alertText);
    return 0;
}

int ProcessScore(char* msg, int* scores)
{
    if (strncmp(msg, "SCORE", 5) != 0) {
        return 1;
    }

    int matched = sscanf(msg, "SCORE %d %d", &scores[0], &scores[1]);
    if (matched != 2) {
        printf("Parsing failed");
        return 1;
    }

    return 0;
}

int ProcessBallLost(char* msg, int fd, GameData* gd, int* scores, int* allowserve, int numOfBalls)
{
    if (strncmp(msg, "BALL_LOST", 9) != 0) {
        return 1;
    }

    scores[!gd->host]++;
    *allowserve = 1;

    char rmsg[1024];
    snprintf(rmsg, sizeof(rmsg), "SCORE %d %d", scores[0], scores[1]);
    send(fd, rmsg, strlen(rmsg), 0);

    DrawHeader(gd, scores,numOfBalls);

    GameAlert("You scored a point!");
    MGameAlert(BOT_ROW + 1, LEFT_COL, "Press space to Serve");

    return 0;
}

int ProcessNumOfBalls(char* msg, int* numOfBalls) {
    if (strncmp(msg, "NUM_OF_BALLS", 12) != 0) {
        return 1;
    }

    int matched = sscanf(msg, "NUM_OF_BALLS %d", numOfBalls);
    if (matched != 1) {
        printf("Parsing failed");
        return 1;
    }

    return 0;
}

void SendGameStatus(int fd, char* msg) {
    char rmsg[1024];
    snprintf(rmsg, sizeof(rmsg), "GMSG:%s", msg);
    send(fd, rmsg, strlen(rmsg), 0);
}