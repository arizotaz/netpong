#ifndef GAME_H
#define GAME_H 1

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


void DrawWalls(GameData);
void DrawHeader(GameData);

#endif