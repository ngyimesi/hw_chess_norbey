#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include "datastruct.h"
#include "display.h"

int main(int argc, char *argv[]) {
    /* ablak letrehozasa */
    SDL_Window *window;
    //init constants, board, fields, and the starting position
    PieceColor movingColor = White;
    GameConfig boardConf;
    boardConf.padding_bottom = 50;
    boardConf.boardSize = (Size){504, 504}; //Recommended to be divisible by 8.
    boardConf.windowSize = (Size){800,800};
    boardConf.background = (Color){155, 226, 226, 255};
    boardConf.blackSquare = (Color){195, 160, 130, 255};
    boardConf.whiteSquare = (Color){242, 225, 195, 255};
    boardConf.moveCircleColor = (Color){123, 201, 113, 127};
    boardConf.autoOffColor = (Color){227, 38, 54, 255}; //"Alizarin crimson"
    boardConf.autoOnColor = (Color){76, 187, 23, 255}; //"Kelly green"
    boardConf.buttonColor = (Color){25, 123, 189, 255};
    boardConf.buttonDisabledColor = (Color){191, 191, 191, 255};
    boardConf.buttonTextColor = (Color){255, 255, 255, 255};
    boardConf.reportFieldColor = (Color){255, 255, 255, 255};
    boardConf.reportTextColor = (Color){0, 0, 0, 255};
    boardConf.transformSelectColor = (Color){123, 239, 178, 255};
    strcpy(boardConf.piecefilename, "pieces_scaled.png");
    strcpy(boardConf.fontfilename, "LiberationSerif-Regular.ttf");
    strcpy(boardConf.bgfilename, "bg_scaled.jpg");
    boardConf.pieceSize = 63;
    if(!loadConfig(&boardConf))
        createConfig(&boardConf);
    char textBox[50+1] = "";
    bool autoRotate = true;
    PieceColor rotPos = movingColor;
    SLStat slStatus = IDLE;
    //transform needs to be finished
    PieceType pawnTransform = Queen;
    FieldCoordinates boardCoordinates = getBoardCoordinates(boardConf.windowSize, boardConf.boardSize, boardConf.padding_bottom);
    Piece *selected = NULL;
    Field fields[8][8];
    Piece pieces[32];
    GameStat status = NORMAL;
    GameMove *moveChain = NULL;

    SDL_Renderer *renderer;
    sdl_init("Chess", boardConf.windowSize.width, boardConf.windowSize.height, &window, &renderer);
    int flags=IMG_INIT_JPG|IMG_INIT_PNG;
    IMG_Init(flags);
    SDL_Texture *piecefile = IMG_LoadTexture(renderer, boardConf.piecefilename);
    if(!piecefile)
    {
        SDL_Log("Couldn't load pieces file. %s\n", TTF_GetError());
        exit(1);
    }
    SDL_Texture *bgfile = IMG_LoadTexture(renderer, boardConf.bgfilename); //if null, there exists a fallback color
    TTF_Init();
    TTF_Font *font = TTF_OpenFont(boardConf.fontfilename, boardConf.boardSize.height/12);
    if (!font)
    {
        SDL_Log("Couldn't open font. %s\n", TTF_GetError());
        exit(1);
    }
    initFields(fields);
    initPieces(pieces, fields);
    visualizeBoard(piecefile, bgfile, boardCoordinates, renderer, font, &boardConf, movingColor, rotPos, autoRotate, &slStatus, &status, pawnTransform, fields, pieces, textBox, moveChain);
    /* draw to screeen */
    SDL_RenderPresent(renderer);
    /* whait for clicks */
    SDL_Event event;
    while (SDL_WaitEvent(&event) && event.type != SDL_QUIT) {
            switch(event.type)
            {
                case SDL_MOUSEBUTTONUP:
                    if(event.button.button == SDL_BUTTON_LEFT)
                    {
                        clickHandler(renderer, font, &selected, textBox, event.button.x, event.button.y, boardCoordinates, fields, pieces, &movingColor, &rotPos, &autoRotate, &slStatus, &status, &pawnTransform, &moveChain);
                        visualizeBoard(piecefile, bgfile, boardCoordinates, renderer, font, &boardConf, movingColor, rotPos, autoRotate, &slStatus, &status, pawnTransform, fields, pieces, textBox, moveChain);
                        pointDrawer(renderer, movingColor, rotPos, boardConf.moveCircleColor, boardCoordinates, selected, fields, moveChain);
                        SDL_RenderPresent(renderer);
                    }
                    break;
                default:
                    break;
            }
    }
    purgeMoves(&moveChain);
    SDL_DestroyTexture(piecefile);
    SDL_DestroyTexture(bgfile);
    TTF_CloseFont(font);
    SDL_Quit();

    return 0;
}
