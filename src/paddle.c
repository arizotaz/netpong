#include "../include/object.h"

#include <curses.h>

Paddle CreatePaddle(GameData* gd)
{
    Paddle p;
    GameData data = *gd;

    // Set Column
    if (data.playerNumber == 0)
        p.column = LEFT_COL + 2;
    else
        p.column = RIGHT_COL - 2;

    // Paddle height
    p.height = 5;

    // Find Middle of screen;
    float middle = (BOT_ROW - TOP_ROW) / 2.0f;
    middle -= p.height / 2;

    // Set Y value to the top of the screen
    p.pos = TOP_ROW + middle;

    // Set char
    p.c = '#';

    return p;
}

void DrawPaddle(Paddle* paddle)
{
    Paddle p = *paddle;
    for (int i = 0; i < p.height; ++i) {
        move(p.pos + i, p.column);
        addch(p.c);
    }

    char topchar = mvinch(p.pos - 1, p.column) & A_CHARTEXT;
    if (topchar == p.c)
        mvaddch(p.pos - 1, p.column, ' ');
    char bottomchar = mvinch(p.pos + p.height, p.column) & A_CHARTEXT;
    if (bottomchar == p.c)
        mvaddch(p.pos + p.height, p.column, ' ');

    move(p.pos, p.column);
    refresh();
}

void MovePaddleUP(Paddle* paddle)
{
    int newPos = paddle->pos - 1;

    if (newPos > TOP_ROW) {
        paddle->pos = newPos;
    }
    DrawPaddle(paddle);
}

void MovePaddleDown(Paddle* paddle)
{
    int newPos = paddle->pos + 1;

    if (newPos + paddle->height <= BOT_ROW) {
        paddle->pos = newPos;
    }
    DrawPaddle(paddle);
}