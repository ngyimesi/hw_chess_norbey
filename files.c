#include "files.h"
static bool readStrParam(FILE *fp, char const *name, char dst[50+1])
{
    const int len = strlen(name);
    char param[50+len+1];
    if(fgets(param, 50+len, fp) == NULL)
        return false;
    if(strncmp(name, param, len) != 0) //name contains = at the ends
        return false;
    char *useful = param+len; //reindex it, so that we can simply copy
    strncpy(dst, useful, 50);
    int dstlen = strlen(dst);
    //the last two characters will be: \n, \0
    if(dstlen > 1 && dst[dstlen-1] == '\n')
        dst[dstlen-1] = '\0'; //remove \n from end
    return true;
}
bool loadConfig(GameConfig *boardConf)
{
    FILE *fp = fopen("config.txt", "rt");
    if(fp == NULL)
        return false;
    bool ok = true; //we need to use it, since files have to be closed before returning
    //1: padding_bottom
    if(ok && fscanf(fp, "padding_bottom=%d", &boardConf->padding_bottom) != 1)
        ok = false;
    fscanf(fp, " "); //C magic
    if(ok && fscanf(fp, "pieceSize=%d", &boardConf->pieceSize) != 1)
        ok = false;
    fscanf(fp, " ");
    if(ok && fscanf(fp, "boardSize=%d, %d", &boardConf->boardSize.height, &boardConf->boardSize.width) != 2)
        ok = false;
    fscanf(fp, " ");
    if(ok && fscanf(fp, "windowSize=%d, %d", &boardConf->windowSize.height, &boardConf->windowSize.width) != 2)
        ok = false;
    fscanf(fp, " ");
    if(ok && fscanf(fp, "background=%d, %d, %d, %d", &boardConf->background.R, &boardConf->background.G, &boardConf->background.B, &boardConf->background.A) != 4)
        ok = false;
    fscanf(fp, " ");
    if(ok && fscanf(fp, "blackSquare=%d, %d, %d, %d", &boardConf->blackSquare.R, &boardConf->blackSquare.G, &boardConf->blackSquare.B, &boardConf->blackSquare.A) != 4)
        ok = false;
    fscanf(fp, " ");
    if(ok && fscanf(fp, "whiteSquare=%d, %d, %d, %d", &boardConf->whiteSquare.R, &boardConf->whiteSquare.G, &boardConf->whiteSquare.B, &boardConf->whiteSquare.A) != 4)
        ok = false;
    fscanf(fp, " ");
    if(ok && fscanf(fp, "moveCircleColor=%d, %d, %d, %d", &boardConf->moveCircleColor.R, &boardConf->moveCircleColor.G, &boardConf->moveCircleColor.B, &boardConf->moveCircleColor.A) != 4)
        ok = false;
    fscanf(fp, " ");
    if(ok && fscanf(fp, "autoOffColor=%d, %d, %d, %d", &boardConf->autoOffColor.R, &boardConf->autoOffColor.G, &boardConf->autoOffColor.B, &boardConf->autoOffColor.A) != 4)
        ok = false;
    fscanf(fp, " ");
    if(ok && fscanf(fp, "autoOnColor=%d, %d, %d, %d", &boardConf->autoOnColor.R, &boardConf->autoOnColor.G, &boardConf->autoOnColor.B, &boardConf->autoOnColor.A) != 4)
        ok = false;
    fscanf(fp, " ");
    if(ok && fscanf(fp, "buttonColor=%d, %d, %d, %d", &boardConf->buttonColor.R, &boardConf->buttonColor.G, &boardConf->buttonColor.B, &boardConf->buttonColor.A) != 4)
        ok = false;
    fscanf(fp, " ");
    if(ok && fscanf(fp, "buttonDisabledColor=%d, %d, %d, %d", &boardConf->buttonDisabledColor.R, &boardConf->buttonDisabledColor.G, &boardConf->buttonDisabledColor.B, &boardConf->buttonDisabledColor.A) != 4)
        ok = false;
    fscanf(fp, " ");
    if(ok && fscanf(fp, "buttonTextColor=%d, %d, %d, %d", &boardConf->buttonTextColor.R, &boardConf->buttonTextColor.G, &boardConf->buttonTextColor.B, &boardConf->buttonTextColor.A) != 4)
        ok = false;
    fscanf(fp, " ");
    if(ok && fscanf(fp, "reportFieldColor=%d, %d, %d, %d", &boardConf->reportFieldColor.R, &boardConf->reportFieldColor.G, &boardConf->reportFieldColor.B, &boardConf->reportFieldColor.A) != 4)
        ok = false;
    fscanf(fp, " ");
    if(ok && fscanf(fp, "reportTextColor=%d, %d, %d, %d", &boardConf->reportTextColor.R, &boardConf->reportTextColor.G, &boardConf->reportTextColor.B, &boardConf->reportTextColor.A) != 4)
        ok = false;
    fscanf(fp, " ");
    if(ok && fscanf(fp, "transformSelectColor=%d, %d, %d, %d", &boardConf->transformSelectColor.R, &boardConf->transformSelectColor.G, &boardConf->transformSelectColor.B, &boardConf->transformSelectColor.A) != 4)
        ok = false;
    fscanf(fp, " ");
    //background
    if(ok && !readStrParam(fp, "bgfilename=", boardConf->bgfilename))
        ok = false;
    fscanf(fp, " ");
    //font
    if(ok && !readStrParam(fp, "fontfilename=", boardConf->fontfilename))
        ok = false;
    fscanf(fp, " ");
    //pieces
    if(ok && !readStrParam(fp, "piecefilename=", boardConf->piecefilename))
        ok = false;
    fclose(fp);
    return ok;
}
bool createConfig(GameConfig *boardConf)
{
    FILE *fp = fopen("config.txt", "wt");
    if(fp == NULL)
        return false;
    fprintf(fp, "padding_bottom=%d\n", boardConf->padding_bottom);
    fprintf(fp, "pieceSize=%d\n", boardConf->pieceSize);
    fprintf(fp, "boardSize=%d, %d\n", boardConf->boardSize.height, boardConf->boardSize.width);
    fprintf(fp, "windowSize=%d, %d\n", boardConf->windowSize.height, boardConf->windowSize.width);
    fprintf(fp, "background=%d, %d, %d, %d\n", boardConf->background.R, boardConf->background.G, boardConf->background.B, boardConf->background.A);
    fprintf(fp, "blackSquare=%d, %d, %d, %d\n", boardConf->blackSquare.R, boardConf->blackSquare.G, boardConf->blackSquare.B, boardConf->blackSquare.A);
    fprintf(fp, "whiteSquare=%d, %d, %d, %d\n", boardConf->whiteSquare.R, boardConf->whiteSquare.G, boardConf->whiteSquare.B, boardConf->whiteSquare.A);
    fprintf(fp, "moveCircleColor=%d, %d, %d, %d\n", boardConf->moveCircleColor.R, boardConf->moveCircleColor.G, boardConf->moveCircleColor.B, boardConf->moveCircleColor.A);
    fprintf(fp, "autoOffColor=%d, %d, %d, %d\n", boardConf->autoOffColor.R, boardConf->autoOffColor.G, boardConf->autoOffColor.B, boardConf->autoOffColor.A);
    fprintf(fp, "autoOnColor=%d, %d, %d, %d\n", boardConf->autoOnColor.R, boardConf->autoOnColor.G, boardConf->autoOnColor.B, boardConf->autoOnColor.A);
    fprintf(fp, "buttonColor=%d, %d, %d, %d\n", boardConf->buttonColor.R, boardConf->buttonColor.G, boardConf->buttonColor.B, boardConf->buttonColor.A);
    fprintf(fp, "buttonDisabledColor=%d, %d, %d, %d\n", boardConf->buttonDisabledColor.R, boardConf->buttonDisabledColor.G, boardConf->buttonDisabledColor.B, boardConf->buttonDisabledColor.A);
    fprintf(fp, "buttonTextColor=%d, %d, %d, %d\n", boardConf->buttonTextColor.R, boardConf->buttonTextColor.G, boardConf->buttonTextColor.B, boardConf->buttonTextColor.A);
    fprintf(fp, "reportFieldColor=%d, %d, %d, %d\n", boardConf->reportFieldColor.R, boardConf->reportFieldColor.G, boardConf->reportFieldColor.B, boardConf->reportFieldColor.A);
    fprintf(fp, "reportTextColor=%d, %d, %d, %d\n", boardConf->reportTextColor.R, boardConf->reportTextColor.G, boardConf->reportTextColor.B, boardConf->reportTextColor.A);
    fprintf(fp, "transformSelectColor=%d, %d, %d, %d\n", boardConf->transformSelectColor.R, boardConf->transformSelectColor.G, boardConf->transformSelectColor.B, boardConf->transformSelectColor.A);
    fprintf(fp, "bgfilename=%s\n", boardConf->bgfilename);
    fprintf(fp, "fontfilename=%s\n", boardConf->fontfilename);
    fprintf(fp, "piecefilename=%s\n", boardConf->piecefilename);
    fclose(fp);
    return true;
}
bool loadGame(char *filename, GameMove **moveChain, Piece pieces[32], Field fields[8][8], PieceColor *movingColor)
{
    if(strlen(filename) == 0) //forbids empty filename
        return false;
    int fullSize = strlen(filename) + strlen(".txt") + strlen("./saves/")+1;
    char *fullFileName = (char*)malloc(fullSize*sizeof(char));
    if(fullFileName == NULL) //malloc might fail
        return false;
    strcpy(fullFileName, "./saves/");
    strcat(fullFileName, filename);
    strcat(fullFileName, ".txt");
    FILE *fp = fopen(fullFileName, "rt");
    free(fullFileName);
    if(fp == NULL) //if file could not be opened
        return false;
    if(*moveChain != NULL) //we don't need to quit, just purge the current game
    {
        purgeMoves(moveChain);
        initFields(fields);
        initPieces(pieces, fields);
        *movingColor = White;
    }
    int pieceId, x, y, transform;
    bool valid = true;
    while(fscanf(fp, "%d\t%d\t%d\t%d", &pieceId, &x, &y, &transform) == 4 && valid)
    {
        if(fgetc(fp) == '\n') //checks file format
        {
            if(pieceId >= 0 && pieceId < 32 && fieldExists(x,y) && transform >= 1 && transform <= 4) //format check for read input
            {
                bool pawnTrans = pieces[pieceId].type == Pawn && (((pieces[pieceId].color == Black) && (y==0)) || ((pieces[pieceId].color == White) && (y==7)));
                bool enpassant = (pieces[pieceId].type == Pawn && pieces[pieceId].field->x != x && fields[x][y].piece == NULL);
                Move newMove = (Move){&pieces[pieceId], pieces[pieceId].field, &fields[x][y], pawnTrans, (enpassant) ? fields[x][pieces[pieceId].field->y].piece : fields[x][y].piece, (enpassant) ? &fields[x][pieces[pieceId].field->y] : &fields[x][y]};
                if(isValidMove(&newMove, *movingColor, fields, *moveChain))
                {
                    commitMove(&newMove, fields, (PieceType)transform);
                    *movingColor = (*movingColor == White) ? Black : White;
                    moveAdd(moveChain, newMove, (PieceType)transform);
                }
            }
            else
                valid = false;
        }
        else
            valid = false;
    }
    fclose(fp);
    return true;
}
bool saveGame(char *filename, GameMove *moveChain)
{
    //saves directory exists?
    struct stat s;
    if (stat("./saves", &s) < 0)
    {
        mkdir("./saves");
        stat("./saves", &s);
    }
    if(!S_ISDIR(s.st_mode)) //if accidentally a file exists in the folder with the same name, doesn't allow saving
        return false;
    if(strlen(filename) == 0) //forbids saving without a filename
        return false;
    if(moveChain == NULL) //forbids saving empty games
        return false;
    int fullSize = strlen(filename) + strlen(".txt") + strlen("./saves/")+1;
    char *fullFileName = (char*)malloc(fullSize*sizeof(char));
    if(fullFileName == NULL)
        return false;
    strcpy(fullFileName, "./saves/");
    strcat(fullFileName, filename);
    strcat(fullFileName, ".txt");
    FILE *check = fopen(fullFileName, "rt");
    if(check != NULL)
    {
        free(fullFileName);
        fclose(check);
        return false;
    }
    FILE *fp = fopen(fullFileName, "wt");
    free(fullFileName);
    if(fp == NULL)
        return false;
    while(moveChain->previous != NULL) //rollback - not neccessarily effective, but easier than passing the n-th argument for the pointer to the beginning
        moveChain = moveChain->previous;
    for(GameMove *i = moveChain; i->next != NULL; i = i->next)
    {
        fprintf(fp, "%d\t%d\t%d\t%d\n", i->move.piece->id, i->move.destination->x, i->move.destination->y, i->transform);
    }
    fclose(fp);
    return true;
}
