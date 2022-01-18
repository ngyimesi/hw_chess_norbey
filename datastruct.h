#ifndef DATASTRUCT_H
#define DATASTRUCT_H
#include <stdbool.h>
#include <stdlib.h>
typedef enum PieceColor {
    Black, White
} PieceColor;
typedef enum PieceType {
    Pawn = 0, Rook = 1, Knight = 2, Bishop = 3, Queen = 4, King = 5
} PieceType;
typedef enum SLStat {
    S_SUC, L_SUC, S_FAIL, L_FAIL, IDLE
} SLStat; //Save-Load status
typedef enum GameStat {
    NORMAL, CHECK, CHECKMATE, STALEMATE, DRAW
} GameStat;
typedef struct Field Field;
typedef struct Piece Piece;
typedef struct Move Move;
typedef struct GameMove GameMove;
typedef struct FieldCoordinates FieldCoordinates;
typedef struct Size Size;
typedef struct Color Color;
typedef struct GameColors GameColors;

typedef struct Field {
    int x,y;
    char name[3];
    Piece *piece;
} Field;
typedef struct Piece {
    int id;
    PieceType type;
    Field *field;
    PieceColor color;
    bool onBoard;
} Piece;
//This is the structure for one gameMove, or one possible move
typedef struct Move {
    Piece *piece;
    Field *from;
    Field *destination;
    bool transformed;
    Piece *captured;
    Field *capField;
} Move;
//And this one stores a move played in the game, don't mix it up with Move. It's a structure for a chained list
typedef struct GameMove {
    Move move;
    PieceType transform;
    GameMove *previous;
    GameMove *next;
} GameMove;
typedef struct FieldCoordinates {
    int x1,y1,x2,y2;
} FieldCoordinates;
typedef struct Size {
    int height, width;
} Size;
typedef struct Color {
    int R,G,B,A;
} Color;
typedef struct GameConfig {
    Color background, whiteSquare, blackSquare, buttonColor, buttonDisabledColor, buttonTextColor, transformSelectColor, autoOnColor, autoOffColor, moveCircleColor, reportFieldColor, reportTextColor;
    Size windowSize, boardSize;
    int padding_bottom, pieceSize;
    char piecefilename[50+1], fontfilename[50+1], bgfilename[50+1];
} GameConfig;
#include "display.h"
#include "files.h"
bool initMoveChain(GameMove **moveChain);
void purgeMovesBackward(GameMove **curpos);
void purgeMovesForward(GameMove **curpos);
void purgeMoves(GameMove **curpos);
bool moveAdd(GameMove **curpos, Move newmove, PieceType pawnTransform);
void initFields(Field fields[8][8]);
void initPieces(Piece pieces[32], Field fields[8][8]);
bool pieceMoved(Piece const *piece, GameMove const *curpos);
bool fieldExists(int x, int y);
bool hasPiece(Field const *field);
void moveToBoard(Field const srcBoard[8][8], Field dstBoard[8][8], Move const *move);
void commitMove(Move *move, Field fields[8][8], PieceType pawnTransform);
bool isCheck(PieceColor checkFor, Field fields[8][8], GameMove const *moveChain);
bool wouldBeCheck(PieceColor checkFor, Move const *move, Field fields[8][8], GameMove const *moveChain);
void removeCheckMoves(Move *moves, int size, int *validCount, PieceColor color, Field fields[8][8], GameMove const *moveChain);
bool searchPieceInFields(Piece *piece, Field fields[8][8], int *xc, int *yc);
bool initCoordinates(Piece *piece, Field fields[8][8], bool removeChecks, int *x, int *y);
int getPieceMoves(PieceColor movingColor, Piece *piece, Field fields[8][8], Move **result, bool removeChecks, GameMove const *moveChain);
int getValidMoves(PieceColor movingColor, Piece pieces[32], Field fields[8][8], GameMove *moveChain, bool removeChecks, Move *moves[32], int sizes[32]);
void freeValidMoves(Move *validMoves[32]);
bool isValidMove(Move *move, PieceColor movingColor, Field fields[8][8], GameMove *movechain);
int getBishopFieldColor(Piece *bishop);
int countOnBoard(Piece pieces[32]);
#endif
