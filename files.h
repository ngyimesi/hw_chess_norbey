#ifndef FILES_H
#define FILES_H
#include <stdio.h> //only this module need stdio
#include <string.h>
//used for checking directory access
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
//till this point
#include <stdbool.h>
#include "datastruct.h"
#include "display.h"
bool loadConfig(GameConfig *boardConf);
bool createConfig(GameConfig *boardConf);
bool loadGame(char *filename, GameMove **moveChain, Piece pieces[32], Field fields[8][8], PieceColor *movingColor);
bool saveGame(char *filename, GameMove *moveChain);
#endif // FILES_H
