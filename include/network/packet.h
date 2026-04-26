#ifndef GAME_PACKET_H
#define GAME_PACKET_H 1

#include "../game.h"
#include "../object.h"

int ProcessBallPacket(char* msg, Ball* ball, GameData* gd);
int ProcessAlert(char* msg);
int ProcessGMSG(char* msg);
int ProcessScore(char* msg, int* scores);
int ProcessBallLost(char* msg, int fd, GameData* gd, int* scores, int* allowSever, int numOfBalls);
int ProcessNumOfBalls(char* msg, int* numOfBalls);

void SendGameStatus(int fd, char* msg);

#endif