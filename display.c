#include "display.h"
/* create window */
void sdl_init(char const *felirat, int szeles, int magas, SDL_Window **pwindow, SDL_Renderer **prenderer) {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        SDL_Log("Couldn't start SDL: %s", SDL_GetError());
        exit(1);
    }
    SDL_Window *window = SDL_CreateWindow(felirat, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, szeles, magas, 0);
    if (window == NULL) {
        SDL_Log("Couldn't create window: %s", SDL_GetError());
        exit(1);
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == NULL) {
        SDL_Log("Couldn't create renderer: %s", SDL_GetError());
        exit(1);
    }
    SDL_RenderClear(renderer);

    *pwindow = window;
    *prenderer = renderer;
}
//stolen from infoc
bool input_text(char *dest, size_t length, SDL_Rect rect, SDL_Color bg, SDL_Color text, TTF_Font *font, SDL_Renderer *renderer)
{
    /* Ez tartalmazza az aktualis szerkesztest */
    char composition[SDL_TEXTEDITINGEVENT_TEXT_SIZE];
    composition[0] = '\0';
    /* Ezt a kirajzolas kozben hasznaljuk */
    char textandcomposition[length + SDL_TEXTEDITINGEVENT_TEXT_SIZE + 1];
    /* Max hasznalhato szelesseg */
    int maxw = rect.w - 2;
    int maxh = rect.h - 2;

    //dest[0] = '\0'; - no need to reset text

    bool enter = false;
    bool exit = false; //modification - activated on mouseclick too

    SDL_StartTextInput();
    while (!exit && !enter) {
        /* doboz kirajzolasa */
        boxRGBA(renderer, rect.x, rect.y, rect.x + rect.w - 1, rect.y + rect.h - 1, bg.r, bg.g, bg.b, 255);
        rectangleRGBA(renderer, rect.x, rect.y, rect.x + rect.w - 1, rect.y + rect.h - 1, text.r, text.g, text.b, 255);
        /* szoveg kirajzolasa */
        int w;
        strcpy(textandcomposition, dest);
        strcat(textandcomposition, composition);
        if (textandcomposition[0] != '\0') {
            SDL_Surface *label = TTF_RenderUTF8_Blended(font, textandcomposition, text);
            SDL_Texture *label_t = SDL_CreateTextureFromSurface(renderer, label);
            SDL_Rect cel = { rect.x, rect.y, label->w < maxw ? label->w : maxw, label->h < maxh ? label->h : maxh };
            SDL_RenderCopy(renderer, label_t, NULL, &cel);
            SDL_FreeSurface(label);
            SDL_DestroyTexture(label_t);
            w = cel.w;
        } else {
            w = 0;
        }
        /* kurzor kirajzolasa */
        if (w < maxw) {
            vlineRGBA(renderer, rect.x + w + 2, rect.y + 2, rect.y + rect.h - 3, text.r, text.g, text.b, 192);
        }
        /* megjeleniti a képernyon az eddig rajzoltakat */
        SDL_RenderPresent(renderer);

        SDL_Event event;
        SDL_WaitEvent(&event);
        switch (event.type) {
            /* Kulonleges karakter */
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_BACKSPACE) {
                    int textlen = strlen(dest);
                    do {
                        if (textlen == 0) {
                            break;
                        }
                        if ((dest[textlen-1] & 0x80) == 0x00)   {
                            /* Egy bajt */
                            dest[textlen-1] = 0x00;
                            break;
                        }
                        if ((dest[textlen-1] & 0xC0) == 0x80) {
                            /* Bajt, egy tobb-bajtos szekvenciabol */
                            dest[textlen-1] = 0x00;
                            textlen--;
                        }
                        if ((dest[textlen-1] & 0xC0) == 0xC0) {
                            /* Egy tobb-bajtos szekvencia elso bajtja */
                            dest[textlen-1] = 0x00;
                            break;
                        }
                    } while(true);
                }
                if (event.key.keysym.sym == SDLK_RETURN) {
                    enter = true;
                }
                break;

            /* A feldolgozott szoveg bemenete */
            case SDL_TEXTINPUT:
                if (strlen(dest) + strlen(event.text.text) < length) {
                    strcat(dest, event.text.text);
                }

                /* Az eddigi szerkesztes torolheto */
                composition[0] = '\0';
                break;

            /* Szoveg szerkesztese */
            case SDL_TEXTEDITING:
                strcpy(composition, event.edit.text);
                break;
            case SDL_MOUSEBUTTONDOWN:
                enter = true; //mouseclick also provides correct condition to exit the loop
                break;
            case SDL_QUIT:
                /* visszatesszuk a sorba ezt az eventet, mert
                 * sok mindent nem tudunk vele kezdeni */
                SDL_PushEvent(&event);
                exit = true;
                break;
        }
    }

    /* igaz jelzi a helyes beolvasast; = ha enter miatt allt meg a ciklus */
    SDL_StopTextInput();
    return enter;
}
/* This function supposes that the given window size is larger than the board size */
void createBoard(Size windowSize, Size boardSize, SDL_Renderer *renderer, int padding_bottom, Color const *blackSquare, Color const *whiteSquare)
{
    bool black = false;
    for(int y = windowSize.height-padding_bottom-boardSize.height; y < windowSize.height-padding_bottom; y += ceil((double)boardSize.height/8))
    {
        for(int x = (windowSize.width-boardSize.width)/2; x < windowSize.width-(windowSize.width-boardSize.width)/2; x += ceil((double)boardSize.width/8))
        {
            if(black)
                boxRGBA(renderer, x,y, x+boardSize.width/8, y+boardSize.height/8, blackSquare->R, blackSquare->G, blackSquare->B, blackSquare->A);
            else
                boxRGBA(renderer, x,y, x+boardSize.width/8, y+boardSize.height/8, whiteSquare->R, whiteSquare->G, whiteSquare->B, whiteSquare->A);
            black = !black;
        }
        black = !black;
    }
}
FieldCoordinates getBoardCoordinates(Size windowSize, Size boardSize, int padding_bottom)
{
    FieldCoordinates fc = {(windowSize.width-boardSize.width)/2, windowSize.height-padding_bottom-boardSize.height, windowSize.width-(windowSize.width-boardSize.width)/2, windowSize.height-padding_bottom };
    return fc;
}
//This function returns the top-left and bottom-right point of a field on board depending on its coordinates in the Field array.
FieldCoordinates getFieldCoordinates(int x, int y, FieldCoordinates board, PieceColor rotPos)
{
    Size boardSize = {board.y2-board.y1, board.x2-board.x1};
    Size fieldSize = {boardSize.height/8, boardSize.width/8};
    if(rotPos == White && (board.x1+(x+1)*fieldSize.width <= board.x2) && (board.y2-((y+1)*fieldSize.height) >= board.y1))
        return (FieldCoordinates) {board.x1+x*fieldSize.width, board.y2-((y+1)*fieldSize.height), board.x1+(x+1)*fieldSize.width, board.y2-(y*fieldSize.height)};
    if(rotPos == Black && (board.x2-((x+1)*fieldSize.width) >= board.x1) && (board.y1+(y+1)*fieldSize.height <= board.y2))
        return (FieldCoordinates) {board.x2-((x+1)*fieldSize.width), board.y1+y*fieldSize.height, board.x2-(x*fieldSize.width), board.y1+(y+1)*fieldSize.height};
    return (FieldCoordinates){-1,-1,-1,-1};
}
//This function creates the textures of pieces on the board
bool renderPieces(SDL_Texture *pieces, int pieceSize, Field fields[8][8], SDL_Renderer *renderer, FieldCoordinates board, PieceColor rotPos)
{
    SDL_Rect pieceImgs[12];
    getPieceImgs(pieceImgs, pieceSize);
    FieldCoordinates destCoord;
    SDL_Rect dst;
    int multiplier = 0;
    int index = 0;
    for(int y = 0; y < 8; ++y)
    {
        for(int x = 0; x < 8; ++x)
        {
            if(fields[x][y].piece != NULL && fields[x][y].piece->onBoard)
            {
                destCoord = getFieldCoordinates(x,y, board, rotPos);
                if(destCoord.x1 != -1)
                {
                    if(fields[x][y].piece->color == White)
                        multiplier = 0;
                    else
                        multiplier = 1;
                    /*
                    In pieceimgs the first six elements are the white pieces, the others are black,
                    and they follow each other in the order they are numbered in the enumeration.
                    */
                    index = multiplier*6+fields[x][y].piece->type;
                    dst = (SDL_Rect){destCoord.x1, destCoord.y1, destCoord.x2-destCoord.x1, destCoord.y2-destCoord.y1};
                    SDL_RenderCopy(renderer, pieces, &pieceImgs[index], &dst);
                }
            }
        }
    }
    return true;
}
Field *getFieldByClickPos(int x, int y, FieldCoordinates board, Field fields[8][8], PieceColor rotPos)
{
    if(x >= board.x1 && x <= board.x2 && y >= board.y1 && y <= board.y2)
    {
        int boardX = x-board.x1;
        int boardY = y-board.y1;
        Size boardSize = {board.y2-board.y1, board.x2-board.x1};
        Size fieldSize = {boardSize.height/8, boardSize.width/8};
        if(rotPos == White)
            return &fields[boardX/fieldSize.width][7-boardY/fieldSize.height];
        else
            return &fields[7-boardX/fieldSize.width][boardY/fieldSize.height];
    }
    return NULL;
}
void getPieceImgs(SDL_Rect coords[12], int pieceSize)
{
    for(int i = 0; i < 12; ++i)
    {
        coords[i] = (SDL_Rect){(pieceSize*i)%(pieceSize*6), (i/6)*pieceSize, pieceSize, pieceSize};
    }
}
//It was too late to realize that sdl had a built-in struct for RGBA colors
SDL_Color colorToSDL(Color color)
{
    return (SDL_Color){color.R, color.G, color.B, color.A};
}
Color SDLToColor(SDL_Color color)
{
    return (Color){color.r, color.g, color.b, color.a};
}
Field *selectField(int x, int y, FieldCoordinates board, Field fields[8][8], PieceColor movingColor)
{
    Field *clicked = getFieldByClickPos(x, y, board, fields, movingColor);
    if(clicked->piece->color == movingColor)
        return clicked;
    return NULL;
}
int pieceSelectorClick(int x, int y, FieldCoordinates board)
{
    Size fieldSize = (Size){(board.y2-board.y1)/8, (board.x2-board.x1)/8};
    if(x > board.x1 && x < board.x1+4*fieldSize.width && y < board.y1 && y > board.y1-fieldSize.height)
        return ((x-board.x1)/fieldSize.width)+1;
    return 0;
}
static void addCircle(SDL_Renderer *renderer, Field *dest, Color moveCircle, PieceColor rotPos, FieldCoordinates board)
{
    if(dest != NULL)
    {
        FieldCoordinates field = getFieldCoordinates(dest->x, dest->y, board, rotPos);
        int centerX = (field.x1+field.x2)/2, centerY = (field.y1+field.y2)/2, rad = (field.x2-field.x1)/6;
        filledCircleRGBA(renderer, centerX, centerY, rad, moveCircle.R, moveCircle.G, moveCircle.B, moveCircle.A);
    }
}
void pointDrawer(SDL_Renderer *renderer, PieceColor movingColor, PieceColor rotPos, Color moveCircle, FieldCoordinates board, Piece *selected, Field fields[8][8], GameMove *moveChain)
{
    if(selected != NULL)
    {
        Move *validMoves = NULL;
        int size = getPieceMoves(movingColor, selected, fields, &validMoves, true, moveChain);
        for(int i = 0; i < size; ++i)
        {
            addCircle(renderer, validMoves[i].destination, moveCircle, rotPos, board);
        }
        free(validMoves);
    }

}
static bool saveButtonClick(int x, int y, FieldCoordinates board)
{
    return (y <= 50+10) && (y >= 10) && (x >= board.x1) && (x <= board.x1+((board.x2-board.x1)/4));
}
static bool loadButtonClick(int x, int y, FieldCoordinates board)
{
    return (y <= 50+10) && (y >= 10) && (x >= board.x1+(3*(board.x2-board.x1)/8)) && (x <= board.x1+(5*(board.x2-board.x1)/8));
}
static bool newButtonClick(int x, int y, FieldCoordinates board)
{
    return (y <= 50+10) && (y >= 10) && (x >= board.x1-((board.x2-board.x1)/4)) && (x <= board.x2);
}
static bool textBoxClick(int x, int y, FieldCoordinates board)
{
    return (x >= board.x1 && x <= board.x2 && y >= 50+10+10 && y <= 100+10+10);
}
static bool undoClick(int x, int y, FieldCoordinates board)
{
    return (x >= board.x1-50-10 && x <= board.x2-10 && y >= 50+10+10 && y <= 100+10+10);
}
static bool redoClick(int x, int y, FieldCoordinates board)
{
    return (x >= board.x1+10 && x <= board.x2+50+10 && y >= 50+10+10 && y <= 100+10+10);
}
static bool beginningClick(int x, int y, FieldCoordinates board)
{
    return (x >= board.x1-50-10 && x <= board.x1-10 && y >= 100+10+10+10 && y <= 150+10+10+10);
}
static bool endClick(int x, int y, FieldCoordinates board)
{
    return (x >= board.x1+10 && x <= board.x2+50+10 && y >= 100+10+10+10 && y <= 150+10+10+10);
}
static bool autoRotClick(int x, int y, FieldCoordinates board)
{
    return( x >= board.x1-100-10 && x <= board.x1-10 && y >= board.y1-50 && y <= board.y1);
}
static bool manRotClick(int x, int y, FieldCoordinates board)
{
    return( x >= board.x2+10 && x <= board.x2+100+10 && y >= board.y1-50 && y <= board.y1);
}
static bool redoMove(GameMove **moveChain, Field fields[8][8], PieceColor *movingColor)
{
    if(*moveChain != NULL && (*moveChain)->next != NULL)
    {
        commitMove(&(*moveChain)->move, fields, (*moveChain)->transform);
        *moveChain = (*moveChain)->next;
        *movingColor = (*movingColor == White) ? Black : White;
        return true;
    }
    return false;
}
void clickHandler(SDL_Renderer *renderer, TTF_Font *font, Piece **selected, char textBox[50+1], int x, int y, FieldCoordinates board, Field fields[8][8], Piece pieces[32], PieceColor *movingColor, PieceColor *rotPos, bool *autoRotate, SLStat *slStatus, GameStat *status, PieceType *pawnTransform, GameMove **moveChain)
{
    Field *clicked = getFieldByClickPos(x, y, board, fields, *rotPos);
    if(clicked != NULL && (*status == NORMAL || *status == CHECK))
    {
        if(*selected == NULL)
        {
            if(clicked->piece != NULL && clicked->piece->color == *movingColor)
            {
                *selected = clicked->piece;
            }
        }
        else
        {
            bool transform = ((*movingColor == White && clicked->y == 7 && (*selected)->type == Pawn) || (*movingColor == Black && clicked->y == 0 && (*selected)->type == Pawn));
            bool enpassant = ((*selected)->type == Pawn && (*selected)->field->x != clicked->x && clicked->piece == NULL);
            Move attempt = (Move){*selected, (*selected)->field, clicked, transform, (enpassant) ? fields[clicked->x][(*selected)->field->y].piece : clicked->piece, (enpassant) ? &fields[clicked->x][(*selected)->field->y] : clicked};
            if(isValidMove(&attempt, *movingColor, fields, *moveChain))
            {
                commitMove(&attempt, fields, *pawnTransform);
                *selected = NULL;
                *movingColor = (*movingColor == White) ? Black : White;
                moveAdd(moveChain, attempt, *pawnTransform);
            }
            *selected = NULL;
        }
    }
    else if(pieceSelectorClick(x,y, board)>0)
    {
        *pawnTransform = pieceSelectorClick(x,y,board);
    }
    else if(saveButtonClick(x,y, board))
    {
        if(saveGame(textBox, *moveChain))
            *slStatus = S_SUC;
        else
            *slStatus = S_FAIL;
    }
    else if(loadButtonClick(x,y, board))
    {
        if(loadGame(textBox, moveChain, pieces, fields, movingColor))
            *slStatus = L_SUC;
        else
            *slStatus = L_FAIL;
    }
    else if(newButtonClick(x,y, board))
    {
        purgeMoves(moveChain);
        initFields(fields);
        initPieces(pieces, fields);
        *movingColor = White;
    }
    else if(textBoxClick(x, y, board))
    {
        SDL_Rect textRect = (SDL_Rect){board.x1, 50+10+10, board.x2-board.x1, 50};
        SDL_Color White = (SDL_Color){255,255,255};
        SDL_Color Black = (SDL_Color){0,0,0};
        input_text(textBox, 50, textRect, White, Black, font, renderer);
    }
    else if(undoClick(x, y, board))
    {
        if(*moveChain != NULL && (*moveChain)->previous != NULL)
        {
            *moveChain = (*moveChain)->previous;
            //undo move
            (*moveChain)->move.destination->piece = NULL;
            (*moveChain)->move.from->piece = (*moveChain)->move.piece;
            (*moveChain)->move.piece->field = (*moveChain)->move.from;
            //undo capture
            if((*moveChain)->move.capField != NULL && (*moveChain)->move.captured != NULL)
            {
                (*moveChain)->move.capField->piece = (*moveChain)->move.captured;
                (*moveChain)->move.captured->field = (*moveChain)->move.capField;
                (*moveChain)->move.captured->onBoard = true;
            }
            //undo transform
            if((*moveChain)->move.transformed)
                (*moveChain)->move.piece->type = Pawn;
            //undo castling
            //lcastle
            if((*moveChain)->move.piece->type == King && (*moveChain)->move.destination->x == (*moveChain)->move.from->x-2 && fields[(*moveChain)->move.destination->x+1][(*moveChain)->move.destination->y].piece->type == Rook)
            {
                Piece *RookP = fields[3][(*moveChain)->move.destination->y].piece;
                RookP->field = &fields[0][(*moveChain)->move.from->y];
                fields[0][(*moveChain)->move.from->y].piece = RookP;
                fields[3][(*moveChain)->move.destination->y].piece = NULL;
            }
            //scastle
            if((*moveChain)->move.piece->type == King && (*moveChain)->move.destination->x == (*moveChain)->move.from->x+2 && fields[(*moveChain)->move.destination->x-1][(*moveChain)->move.destination->y].piece->type == Rook)
            {
                Piece *RookP = fields[5][(*moveChain)->move.destination->y].piece;
                RookP->field = &fields[7][(*moveChain)->move.from->y];
                fields[7][(*moveChain)->move.from->y].piece = RookP;
                fields[5][(*moveChain)->move.destination->y].piece = NULL;
            }
            *movingColor = (*movingColor == White) ? Black : White;
        }
    }
    else if(redoClick(x, y, board))
    {
        redoMove(moveChain, fields, movingColor);
    }
    else if(beginningClick(x, y, board))
    {
        if(*moveChain != NULL && (*moveChain)->previous != NULL)
        {
            while((*moveChain)->previous != NULL)
                *moveChain = (*moveChain)->previous;
            initFields(fields);
            initPieces(pieces, fields);
            *movingColor = White;
        }
    }
    else if(endClick(x, y, board))
    {
        while(redoMove(moveChain, fields, movingColor))
            ;
    }
    else if(autoRotClick(x, y, board))
    {
        *autoRotate = !(*autoRotate);
        /*
        if autorotate has been switched off, the rotpos will be equal to the movingColor variable, but the visualizer function
        rotates the view of the pieces.
        if autorotate was switched off without this clause, the board would rotate at the end of the game
        */
        if(!*autoRotate && *status != CHECK && *status != NORMAL)
            *rotPos = (*rotPos == White) ? Black : White;
    }
    else if(manRotClick(x, y, board) && !(*autoRotate))
    {
        *rotPos = (*rotPos == White) ? Black : White;
    }
    if(*autoRotate)
    {
        *rotPos = *movingColor;
    }
}
void showTransformPieces(SDL_Texture *piecefile, int pieceSize, SDL_Renderer *renderer, FieldCoordinates boardCoordinates, PieceType transformPiece, Color highlight)
{
    SDL_Rect pieceImgs[12];
    getPieceImgs(pieceImgs, pieceSize);
    Size fieldSize = (Size){(boardCoordinates.y2-boardCoordinates.y1)/8, (boardCoordinates.x2-boardCoordinates.x1)/8};
    boxRGBA(renderer, boardCoordinates.x1+(transformPiece-1)*fieldSize.width, boardCoordinates.y1-fieldSize.height, boardCoordinates.x1+transformPiece*fieldSize.width, boardCoordinates.y1-1, highlight.R, highlight.G, highlight.B, highlight.A);
    SDL_Rect dst;
    for(int i = 1; i < 5; ++i)
    {
        dst = (SDL_Rect){boardCoordinates.x1+(i-1)*fieldSize.width, boardCoordinates.y1-fieldSize.height, fieldSize.width, fieldSize.height};
        SDL_RenderCopy(renderer, piecefile, &pieceImgs[i], &dst);
    }
}
void textDisplay(SDL_Renderer *renderer, SDL_Rect position, char *text, Color color_in, TTF_Font *font)
{
    SDL_Color color = colorToSDL(color_in);
    SDL_Surface *label;
    SDL_Texture *label_t;
    label = TTF_RenderUTF8_Blended(font, text, color);
    if(label == NULL)
        return;
    label_t = SDL_CreateTextureFromSurface(renderer, label);
    SDL_Rect cel = (SDL_Rect){position.x, position.y, label->w < position.w ? label->w : position.w, label->h < position.h ? label->h : position.h };
    SDL_RenderCopy(renderer, label_t, NULL, &cel);
    SDL_FreeSurface(label);
    SDL_DestroyTexture(label_t);
}
//returns if game should go on, writes situation to box
bool evaluate(FieldCoordinates board, SDL_Renderer *renderer, TTF_Font *font, PieceColor movingColor, Field fields[8][8], Piece pieces[32], GameMove *moveChain, GameConfig const * boardConf, GameStat *status)
{
    Size fieldSize = (Size){(board.y2-board.y1)/8, (board.x2-board.x1)/8};
    FieldCoordinates textField = (FieldCoordinates){board.x1+4*fieldSize.width+1, board.y1-fieldSize.height, board.x2, board.y1-1};
    SDL_Rect textField_r = (SDL_Rect) {board.x1+4*fieldSize.width+1, board.y1-fieldSize.height, 4*fieldSize.width, fieldSize.height};
    boxRGBA(renderer, textField.x1, textField.y1, textField.x2, textField.y2, boardConf->reportFieldColor.R, boardConf->reportFieldColor.G, boardConf->reportFieldColor.B, boardConf->reportFieldColor.A);
    Move *validMoves[32];
    int moveSizes[32];
    int numMoves = getValidMoves(movingColor, pieces, fields, moveChain, true, validMoves, moveSizes);
    //We know the number of possible moves, so we can free the validMoves pointer, before returning
    freeValidMoves(validMoves);
    int onBoardPieceCount = countOnBoard(pieces);
    //everything is normal, unless found different
    *status = NORMAL;
    if(numMoves > 0)
    {
        if(isCheck(movingColor, fields, moveChain))
        {
            textDisplay(renderer, textField_r, "CHECK", boardConf->reportTextColor, font);
            *status = CHECK;
        }
        if(onBoardPieceCount == 2)
        {
            //the two kings only case
            textDisplay(renderer, textField_r, "DRAW", boardConf->reportTextColor, font);
            *status = DRAW;
            return false;
        }
        else if(onBoardPieceCount == 3)
        {
            for(int i = 0; i < 32; ++i)
            {
                //the knight or bishop only case - surely there will be two kings as well
                if(pieces[i].onBoard && (pieces[i].type == Knight || pieces[i].type == Bishop))
                {
                    textDisplay(renderer, textField_r, "DRAW", boardConf->reportTextColor, font);
                    *status = DRAW;
                    return false;
                }
            }
        }
        else if(onBoardPieceCount > 3)
        {
            //Bishops only? Same side and color only?
            bool bishopsOnly = true;
            for(int i = 0; i < 32 && bishopsOnly; ++i)
            {
                if(pieces[i].onBoard && (pieces[i].type != Bishop && pieces[i].type != King))
                    bishopsOnly = false;
            }

            if(bishopsOnly)
            {
                //count the pieces of black and white
                int blackCount = 0, whiteCount = 0;
                for(int i = 0; i < 32; ++i)
                {
                    if(pieces[i].onBoard)
                    {
                        if(pieces[i].color == White)
                            whiteCount++;
                        else
                            blackCount++;
                    }
                }
                //if only one side has bishops
                if(whiteCount == 1 || blackCount == 1)
                {
                    int bishopColor = -1; //the default value: -1 - considered "unset", real values could be 0 or 1;
                    bool diffColorFound = false;
                    for(int i = 0; i < 32 && !diffColorFound; ++i)
                    {
                        if(pieces[i].onBoard && pieces[i].type == Bishop)
                        {
                            if(bishopColor == -1)
                            {
                                bishopColor = getBishopFieldColor(&pieces[i]);
                            }
                            else if(bishopColor != getBishopFieldColor(&pieces[i]))
                            {
                                diffColorFound = true;
                            }
                        }
                    }
                    if(!diffColorFound)
                    {
                        textDisplay(renderer, textField_r, "DRAW", boardConf->reportTextColor, font);
                        *status = DRAW;
                        return false;
                    }
                }
            }

        }
    }
    else
    {
        if(isCheck(movingColor, fields, moveChain))
        {
            textDisplay(renderer, textField_r, "CHECKMATE", boardConf->reportTextColor, font);
            *status = CHECKMATE;
        }
        else
        {
            textDisplay(renderer, textField_r, "STALEMATE", boardConf->reportTextColor, font);
            *status = STALEMATE;
        }
        return false;
    }
    return true;
}
static void drawButton(char *text, SDL_Renderer *renderer, TTF_Font *font, SDL_Rect position, Color textColor_in, Color bg)
{
    SDL_Color textColor = colorToSDL(textColor_in);
    boxRGBA(renderer, position.x, position.y, position.x+position.w, position.y+position.h, bg.R, bg.G, bg.B, bg.A);
    SDL_Surface *label;
    SDL_Texture *label_t;
    //rewrite for text position
    position = (SDL_Rect){position.x+5, position.y+5, position.w-10, position.h-10};
    label = TTF_RenderUTF8_Blended(font, text, textColor);
    label_t = SDL_CreateTextureFromSurface(renderer, label);
    SDL_RenderCopy(renderer, label_t, NULL, &position);
    SDL_FreeSurface(label);
    SDL_DestroyTexture(label_t);
}
static void showButtons(SDL_Renderer *renderer, Size windowSize, FieldCoordinates board, TTF_Font *font, GameConfig const *boardConf)
{
    Size boardSize = (Size){board.y2-board.y1, board.x2-board.x1};
    SDL_Rect savePos = (SDL_Rect){board.x1, 10, boardSize.width/4, 50};
    SDL_Rect loadPos = (SDL_Rect){savePos.x+savePos.w+boardSize.width/8, savePos.y, savePos.w, savePos.h};
    SDL_Rect newPos = (SDL_Rect){loadPos.x+loadPos.w+boardSize.width/8, loadPos.y, savePos.w, savePos.h};
    //save
    drawButton("SAVE", renderer, font, savePos, boardConf->buttonTextColor, boardConf->buttonColor);
    //load
    drawButton("LOAD", renderer, font, loadPos, boardConf->buttonTextColor, boardConf->buttonColor);
    //new
    drawButton("NEW", renderer, font, newPos, boardConf->buttonTextColor, boardConf->buttonColor);
}
bool visualizeBoard(SDL_Texture *piecefile, SDL_Texture *bgfile, FieldCoordinates boardCoordinates, SDL_Renderer *renderer, TTF_Font *font, GameConfig const *boardConf, PieceColor movingColor, PieceColor rotPos, bool autoRotate, SLStat *slStatus, GameStat *status, PieceType transformPiece, Field fields[8][8], Piece pieces[32], char filename[50+1], GameMove *moveChain)
{
    SDL_RenderClear(renderer);
    //background
    if(bgfile != NULL)
    {
        SDL_Rect dst = (SDL_Rect){0, 0, boardConf->windowSize.width, boardConf->windowSize.height};
        SDL_RenderCopy(renderer, bgfile, &dst, &dst);
    }
    else
        boxRGBA(renderer, 0,0, boardConf->windowSize.width, boardConf->windowSize.height, boardConf->background.R, boardConf->background.G, boardConf->background.B, boardConf->background.A);
    createBoard(boardConf->windowSize, boardConf->boardSize, renderer, boardConf->padding_bottom, &boardConf->blackSquare, &boardConf->whiteSquare);
    if(piecefile == NULL)
        return false;
    showTransformPieces(piecefile, boardConf->pieceSize, renderer, boardCoordinates, transformPiece, boardConf->transformSelectColor);
    showButtons(renderer, boardConf->windowSize, boardCoordinates, font, boardConf);
    //show textfield
    SDL_Rect textField = (SDL_Rect) {boardCoordinates.x1, 50+10+10, boardCoordinates.x2-boardCoordinates.x1, 50};
    SDL_Rect slField = (SDL_Rect) {textField.x, textField.y + 50 + 10, boardCoordinates.x2-boardCoordinates.x1, 50};
    //undo and redo buttons
    SDL_Rect undo = (SDL_Rect) {textField.x-50-10, textField.y, 50, 50};
    SDL_Rect redo = (SDL_Rect) {textField.x+textField.w+10, textField.y, 50, 50};
    SDL_Rect end = (SDL_Rect) {slField.x+textField.w+10, slField.y, 50, 50}; //Perhaps it's more obvious this way that it will be located next to the slField
    SDL_Rect beginning = (SDL_Rect) {slField.x-50-10, slField.y, 50, 50};
    Color undoBg = (moveChain == NULL || (moveChain->previous==NULL && moveChain->next != NULL)) ? boardConf->buttonDisabledColor : boardConf->buttonColor;
    Color redoBg = (moveChain == NULL || moveChain->move.destination == NULL) ? boardConf->buttonDisabledColor : boardConf->buttonColor;
    drawButton("<", renderer, font, undo, boardConf->buttonTextColor, undoBg);
    drawButton("<<", renderer, font, beginning, boardConf->buttonTextColor, undoBg);
    drawButton(">", renderer, font, redo, boardConf->buttonTextColor, redoBg);
    drawButton(">>", renderer, font, end, boardConf->buttonTextColor, redoBg);
    //autoRotate and manual rotate buttons
    SDL_Rect autoRect = {boardCoordinates.x1-100-10, boardCoordinates.y1-50, 100, 50};
    SDL_Rect manRect = {boardCoordinates.x2+10, boardCoordinates.y1-50, 100, 50};
    Color autoBg = (autoRotate) ? boardConf->autoOnColor : boardConf->autoOffColor;
    Color manBg = (autoRotate) ? boardConf->buttonDisabledColor : boardConf->buttonColor;
    drawButton("AUTO", renderer, font, autoRect, boardConf->buttonTextColor, autoBg);
    drawButton("ROT", renderer, font, manRect, boardConf->buttonTextColor, manBg);
    boxRGBA(renderer, boardCoordinates.x1, 50+10+10, boardCoordinates.x2, 50+10+10+50, 255, 255, 255, 255);
    textDisplay(renderer, textField, filename, boardConf->reportTextColor, font);
    switch(*slStatus)
    {
        case S_SUC:
            textDisplay(renderer, slField, "SAVE SUCCESSFUL", boardConf->reportTextColor, font);
            break;
        case S_FAIL:
            textDisplay(renderer, slField, "SAVE FAILED", boardConf->reportTextColor, font);
            break;
        case L_SUC:
            textDisplay(renderer, slField, "LOAD SUCCESSFUL", boardConf->reportTextColor, font);
            break;
        case L_FAIL:
            textDisplay(renderer, slField, "LOAD FAILED", boardConf->reportTextColor, font);
            break;
        default:
            break;
    }
    *slStatus = IDLE;
    if(!evaluate(boardCoordinates, renderer, font, movingColor, fields, pieces, moveChain, boardConf, status) && autoRotate) //do not rotate board when game ended
    {

        rotPos = (rotPos == White) ? Black : White; //only locally
    }
    renderPieces(piecefile, boardConf->pieceSize, fields, renderer, boardCoordinates, rotPos);
    return true;
}
