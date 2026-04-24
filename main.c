#include "game.h"
#include <curses.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * Main Fucntion
 */
int main(int argc, char* argv[])
{
    // Create GameData Object
    struct GameData data;

    // Detect Server or Client
    switch (argc) {
    case 2:
        // Start Server
        data.playerNumber = 0;
        data.port = atoi(argv[1]);
        break;
    case 3:
        // Start Client
        data.playerNumber = 1;
        data.domain = argv[1];
        data.port = atoi(argv[2]);
        break;
    default:
        data.playerNumber = 0;
        printf("Please read the following usage\n");
        printf("Server: %s <portnumber>\n", argv[0]);
        printf("Client: %s <domain> <port>\n", argv[0]);
        return 1;
    }

    srand(getpid());

    // Setup Terminal
    if (initscr() == NULL) {
        perror("initscr failed");
        exit(1);
    }
    noecho();
    cbreak();
    nodelay(stdscr, TRUE);

    DrawWalls(data);
    DrawHeader(data);

    int socket_fd = 0;

    while (1) {
        int ch = getch();
        if (ch == 'q')
            break;
    }

    endwin();
}