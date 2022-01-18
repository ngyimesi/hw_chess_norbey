#ifndef DISPLAY_H
#define DISPLAY_H
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include "datastruct.h"
#include "files.h"
void sdl_init(char const *felirat, int szeles, int magas, SDL_Window **pwindow, SDL_Renderer **prenderer);
bool input_text(char *dest, size_t length, SDL_Rect rect, SDL_Color bg, SDL_Color text, TTF_Font *font, SDL_Renderer *renderer);
void createBoard(Size windowSize, Size boardSize, SDL_Renderer *renderer, int padding_bottom, Color const *blackSquare, Color const *whiteSquare);
FieldCoordinates getBoardCoordinates(Size windowSize, Size boardSize, int padding_bottom);
FieldCoordinates getFieldCoordinates(int x, int y, FieldCoordinates board, PieceColor rotPos);
bool renderPieces(SDL_Texture *pieces, int pieceSize, Field fields[8][8], SDL_Renderer *renderer, FieldCoordinates board, PieceColor movingColor);
Field *getFieldByClickPos(int x, int y, FieldCoordinates board, Field fields[8][8], PieceColor rotPos);
void getPieceImgs(SDL_Rect coords[12], int pieceSize);
SDL_Color colorToSDL(Color color);
Color SDLToColor(SDL_Color color);
Field *selectField(int x, int y, FieldCoordinates board, Field fields[8][8], PieceColor movingColor);
void pointDrawer(SDL_Renderer *renderer, PieceColor movingColor, PieceColor rotPos, Color moveCircle, FieldCoordinates board, Piece *selected, Field fields[8][8], GameMove *movechain);
int pieceSelectorClick(int x, int y, FieldCoordinates board);
void clickHandler(SDL_Renderer *renderer, TTF_Font *font, Piece **selected, char textBox[50+1], int x, int y, FieldCoordinates board, Field fields[8][8], Piece pieces[32], PieceColor *movingColor, PieceColor *rotPos, bool *autoRotate, SLStat *slStatus, GameStat *status, PieceType *pawnTransform, GameMove **moveChain);
void showTransformPieces(SDL_Texture *piecefile, int pieceSize, SDL_Renderer *renderer, FieldCoordinates boardCoordinates, PieceType transformPiece, Color highlight);
void textDisplay(SDL_Renderer *renderer, SDL_Rect position, char *text, Color color_in, TTF_Font *font);
bool visualizeBoard(SDL_Texture *piecefile, SDL_Texture *bgfile, FieldCoordinates boardCoordinates, SDL_Renderer *renderer, TTF_Font *font, GameConfig const *boardConf, PieceColor movingColor, PieceColor rotPos, bool autoRotate, SLStat *slStatus, GameStat *status, PieceType transformPiece, Field fields[8][8], Piece pieces[32], char filename[50+1], GameMove *moveChain);
bool evaluate(FieldCoordinates board, SDL_Renderer *renderer, TTF_Font *font, PieceColor movingColor, Field fields[8][8], Piece pieces[32], GameMove *moveChain, GameConfig const *boardConf, GameStat *status);
#endif
