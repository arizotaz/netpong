#ifndef GAME_PADDLE_H
#define GAME_PADDLE_H 1

#include "game.h"

typedef struct {
    int column;
    int pos;
    int height;
    char c;
} Paddle;

Paddle CreatePaddle(GameData* gd);
void DrawPaddle(Paddle*); 
void MovePaddleUP(Paddle*);
void MovePaddleDown(Paddle*);

typedef struct {
    float x;
    float y;

    int lastCharX;
    int lastCharY;
    char c;
    int visible;
    float vx;
    float vy;
} Ball;

Ball CreateBall(GameData* gd);
void DrawBall(Ball*);
void UpdateBall(Ball* ball, Paddle* paddle);
int CheckBallLose(GameData* gd,Ball* ball);
int CheckBallCourtChange(GameData* gd,Ball* ball);
void HideBall(Ball* ball);
void ServeBall(Ball* ball, GameData* gd);
#endif