#include "../../include/network/packet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int ProcessBallPacket(char* msg, Ball* ball, GameData* gd)
{
    // 1. Verify prefix
    if (strncmp(msg, "BTRANS", 6) != 0) {
        printf("Invalid prefix\n");
        printf("\nMSG: %s\n\n\n", msg);
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