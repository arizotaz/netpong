#include "game.h"

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
void DrawHeader(GameData data)
{
    move(TOP_ROW - 2, LEFT_COL);
    addch('P');
    addch('0' + (data.playerNumber + 1));
}