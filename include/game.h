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

/**
 * Setups the curses terminal environment
 */
void SetupTermWin();
/**
 * Cleans up the curses terminal environment
 */
void CloseTermWin();

/**
 * Creates the Game Server that is responsible for the Host's input and processing
 */
void CreateGameServer(Server* serv,GameData*gd);

/**
 * Creates the Client game loop
 */
void RunGameClient(Client* c);

/**
 * Draws the walls based on player 1 or 2
 */
void DrawWalls(GameData);
/**
 * Draws the scoreboard and header
 */
void DrawHeader(GameData);

/**
 * Creates text at a location on screen
 */
void MGameAlert(int y, int x, char* str);
/**
 * Creates text at the top left of the screen for alerts
 */
void GameAlert(char* str);

#endif