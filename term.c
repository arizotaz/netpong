#include "game.h"

#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void SetupTermWin()
{
    srand(getpid());

    // Setup Terminal
    if (initscr() == NULL) {
        perror("initscr failed");
        exit(1);
    }
    noecho();
    cbreak();
    nodelay(stdscr, TRUE);
}

void CloseTermWin()
{
    endwin();
}

void GameAlert(char* str)
{
    MGameAlert(0,0,str);
}

void MGameAlert(int y, int x, char* str)
{
    move(y, x);
    clrtoeol();
    mvaddstr(y, x, str);
    refresh();
}
