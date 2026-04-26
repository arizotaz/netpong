#ifndef GAME_PACKET_H
#define GAME_PACKET_H 1

#include "../game.h"
#include "../object.h"

int ProcessBallPacket(char* msg, Ball* ball, GameData* gd);

#endif