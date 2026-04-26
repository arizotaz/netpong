#include "../include/object.h"

#include <curses.h>
#include <math.h>

Ball CreateBall(GameData* gd)
{
    Ball b;
    b.visible = false;
    b.c = 'O';
    b.y = TOP_ROW + 3;
    b.x = RIGHT_COL - 3;
    b.lastCharX = 0;
    b.lastCharY = 0;

    b.vx = 0;
    b.vy = 0;

    return b;
}
void UpdateBall(Ball* ball, Paddle* p)
{
    int x = ball->x;
    int y = ball->y;

    float nx = ball->x + ball->vx;
    float ny = ball->y + ball->vy;

    if (floor(ny) == TOP_ROW || floor(ny) == BOT_ROW) {
        ball->vy = -ball->vy;
        ny = ball->y + ball->vy;
    }

    if (floor(nx) == p->column) {
        if (ny >= p->pos && ny < p->pos + p->height) {
            ball->vx = -ball->vx;
            nx = ball->x + ball->vx;
        }
    }

    ball->x = nx;
    ball->y = ny;

    int ncx = ball->x;
    int ncy = ball->y;

    // if x y changes then re-draw
    if (ncx != x || ncy != y)
        DrawBall(ball);
}

int CheckBallLose(GameData* gd, Ball* ball)
{
    if (gd->host) {
        if (ball->x <= LEFT_COL) {
            return true;
        }
    } else if (ball->x >= RIGHT_COL)
        return true;
    return false;
}

int CheckBallCourtChange(GameData* gd, Ball* ball)
{
    if (gd->host) {
        if (ball->x >= RIGHT_COL) {
            return true;
        }
    } else if (ball->x <= LEFT_COL)
        return true;
    return false;
}

void DrawBall(Ball* ball)
{
    int x = ball->x;
    int y = ball->y;

    char lastChar = mvinch(ball->lastCharY, ball->lastCharX) & A_CHARTEXT;
    if (lastChar == ball->c)
        mvaddch(ball->lastCharY, ball->lastCharX, ' ');

    mvaddch(y, x, ball->c);
    ball->lastCharY = y;
    ball->lastCharX = x;

    refresh();
}

void HideBall(Ball* ball)
{
    ball->visible = false;
    char lastChar = mvinch(ball->lastCharY, ball->lastCharX) & A_CHARTEXT;
    if (lastChar == ball->c)
        mvaddch(ball->lastCharY, ball->lastCharX, ' ');
}

void ServeBall(Ball* ball, GameData* gd) {
    ball->visible = true;

    float middle = TOP_ROW + (BOT_ROW-TOP_ROW)/2;
    ball->y = middle;

    if (gd->host) 
    ball->x = RIGHT_COL-2;
    else ball->x = LEFT_COL + 2;


    ball->vx = -0.2;
    ball->vy = 0.09;
    if (!gd->host) ball->vx *= -1;
}