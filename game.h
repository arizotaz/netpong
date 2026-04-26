#ifndef GAME_H
#define GAME_H 1

#include "network/networking.h"


// Setup game board
#define TOP_ROW 4
#define BOT_ROW 21
#define LEFT_COL 9
#define RIGHT_COL 70

typedef struct
{
    int host;
    int playerNumber;
    char* domain;
    int port;

} GameData;

void SetupTermWin();
void CloseTermWin();

void CreateGameServer(Server* serv,GameData*gd);
void RunGameClient(Client* c);

void DrawWalls(GameData);
void DrawHeader(GameData);

void MGameAlert(int y, int x, char* str);
void GameAlert(char* str);

#endif