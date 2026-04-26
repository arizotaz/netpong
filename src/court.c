#include "../include/game.h"

#include <curses.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void DrawWalls(GameData data)
{
    for (int i = LEFT_COL; i <= RIGHT_COL; i++) {
        move(TOP_ROW, i);
        addch('=');
        move(BOT_ROW, i);
        addch('-');
    }

    int col = LEFT_COL;
    if (data.playerNumber > 0)
        col = RIGHT_COL;
    for (int i = TOP_ROW; i <= BOT_ROW; i++) {
        move(i, col);
        addch('|');
    }

    refresh();
}
void DrawHeader(GameData* data, int* scores, int numOfBalls)
{
    mvprintw(TOP_ROW-1, LEFT_COL, "SCORES:  P1: %d     P2: %d                       Balls Left: %d", scores[0],scores[1],numOfBalls);
}