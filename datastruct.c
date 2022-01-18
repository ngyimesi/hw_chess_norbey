#include "datastruct.h"
/*
Appends a sentinel-like element at the end of the chain (given that moveChain points to the end)
*/
bool initMoveChain(GameMove **moveChain)
{
    Move defMove = (Move){NULL,NULL,NULL,false,NULL,NULL};
    if(*moveChain == NULL)
    {
        GameMove sentinel = (GameMove){defMove, Pawn, NULL, NULL};
        GameMove *newElem = (GameMove*)malloc(sizeof(GameMove));
        if(newElem == NULL)
            return false;
        *newElem = sentinel;
        *moveChain = newElem;

    }
    else
    {
        GameMove sentinel = (GameMove){defMove, Pawn, *moveChain, NULL};
        GameMove *newElem = (GameMove*)malloc(sizeof(GameMove));
        if(newElem == NULL)
            return false;
        *newElem = sentinel;
        *moveChain = newElem;
    }
    return true;
}
/*
This function can be used to purge moves before current
*/
void purgeMovesBackward(GameMove **curpos)
{
    /*
    The function doesn't purge the current move, so no need to reset the pointer
    */
    if(*curpos != NULL)
    {
        GameMove *position = (*curpos)->previous;
        if(position != NULL)
        {
            while(position->previous != NULL)
            {
                GameMove *newpos = position->previous;
                free(position);
                position = newpos;
            }
            free(position);
            //All the moves purged backward
            (*curpos)->previous = NULL;
        }
    }
}
/*
This function could be used to purge moves after current
(e.g if step could be undone, it could be practical)
*/
void purgeMovesForward(GameMove **curpos)
{
    /*
    The function doesn't purge the current move, so no need to reset the pointer
    */
    if(*curpos != NULL)
    {
        GameMove *position = (*curpos)->next;
        if(position != NULL)
        {
            while(position->next != NULL)
            {
                GameMove *newpos = position->next;
                free(position);
                position = newpos;
            }
            free(position);
            //All the moves purged forward
            (*curpos)->next = NULL;
        }
    }
}
//frees the whole chain of moves
void purgeMoves(GameMove **curpos)
{
    purgeMovesForward(curpos);
    purgeMovesBackward(curpos);
    free(*curpos);
    *curpos = NULL;
}
/*
This function gets the current chain pointer as a pointer, so that it can set it into the position of the new move
DO NOT CALL free() FOR THE CURPOS POINTER IRRESPONIBLY, since it only points to one element of a dynamically allocated chain
However, DO USE purgeMovesForward(), purgeMovesBackward() or purgeMoves() functions instead
*/
bool moveAdd(GameMove **curpos, Move newmove, PieceType pawnTransform)
{
    if(*curpos == NULL)
    {
        //if function is called, when the list wasn't initialized
        if(!initMoveChain(curpos))
            return false;
    }
    //We overwrite the current move - if it's a sentinel, we just append.
    //In case the user made some steps back and moves again the next moves need to be deallocated.
    if((*curpos)->next != NULL)
    {
        //if moves were undone
        purgeMovesForward(curpos);
        //sentinelize current element
        Move defMove = (Move){NULL,NULL,NULL,false,NULL,NULL};
        (*curpos)->move = defMove;
        (*curpos)->transform = Pawn;
        //now we are looking at the sentinel, modify the previous move to the new one
    }
    //We are looking at the sentinel element
    GameMove *insert = (GameMove*)malloc(sizeof(GameMove));
    GameMove *prev = (*curpos)->previous;
    if(insert == NULL)
        return false;
    *insert = (GameMove){newmove, pawnTransform, prev, *curpos};
    //insert is now linked to curpos and the previous element, now we need to relink the previous element - only if exists
    if(prev != NULL)
        prev->next = insert;
    //and the sentinel
    (*curpos)->previous = insert;
    return true;
}
/* Initializes field set, no size taken, supposes that the array has (minimum) 64 values */
void initFields(Field fields[8][8])
{
    for(int y = 0; y < 8; ++y)
    {
        for(int x = 0; x < 8; ++x)
        {
            fields[x][y].piece = NULL;
            fields[x][y].name[0] = 'a'+x;
            fields[x][y].name[1] = '1'+y;
            fields[x][y].name[2] = '\0';
            fields[x][y].x = x;
            fields[x][y].y = y;
        }
    }
}

/*
This function supposes that in a legal chess game there should be no more than 32 pieces in play, and that the fields are located in a 8x8 sized matrix
Shortened code
*/
void initPieces(Piece pieces[32], Field fields[8][8])
{
    PieceType curtype = Pawn;
    int actRow = 0;
    bool dir = true; //true -> up, false -> down
    for (int i = 0; i < 32; ++i)
    {
        //Init pawns
        if(i<16)
        {
            pieces[i].type = Pawn;
            if(i < 8)
                actRow = 1;
            else
                actRow = 6;
        }
        else
        {
            /*
            Init other pieces
            This part is probably somewhat more difficult to follow:
            The code uses the fact, that the piece constants are defined in the order of placement on the board.
            1. It counts up when dir is true, otherwise down.
            2. When it reaches 5 (the type id of the king) or 0 (pawn), it changes direction
            3. When it reaches 5, it also decreases the value of the type by two, so that there will be only one queen placed
            Note: Due to the previous step, decreasing the value of the current type happens with the post increment operator,
            that's what makes it possible to place the first rook of black correctly after placing the second rook of white
            */
            if(curtype < 5 && dir)
                pieces[i].type = ++curtype;
            else if(curtype > 0 && !dir)
                pieces[i].type = curtype--;
            if(curtype == 5 || curtype == 0)
                dir = !dir;
            if(curtype == 5)
                curtype -= 2;
            if(i < 16+8)
                actRow = 0;
            else
                actRow = 7;
        }
        pieces[i].field = &fields[i%8][actRow];
        fields[i%8][actRow].piece = &pieces[i];
        pieces[i].color = (actRow < 4) ? White : Black;
        pieces[i].id = i;
        pieces[i].onBoard = true;
    }
}
//The follwoing three are quite obvious...
bool pieceMoved(Piece const *piece, GameMove const *curpos)
{
    if(curpos != NULL && curpos->previous != NULL)
    {
        curpos = curpos->previous;
        while(curpos != NULL)
        {
            if(curpos->move.piece == piece)
                return true;
            curpos = curpos->previous;
        }
    }

    return false;
}
bool fieldExists(int x, int y)
{
    return (x < 8 && x >=0 && y < 8 && y >= 0);
}
bool hasPiece(Field const *field)
{
    return (field->piece != NULL);
}
/*
This function copies modified situation to dstBoard.
Attention! It doesn't check move validity, or remove references in the pieces set either.
There are other functions for those.
However, it can manage properly, when srcBoard == dstBoard
*/
void moveToBoard(Field const srcBoard[8][8], Field dstBoard[8][8], Move const *move)
{
    //copy board if field set is not the same
    if(srcBoard != dstBoard)
    {
        for(int x = 0; x < 8; ++x)
        {
            for(int y = 0; y < 8; ++y)
            {
                dstBoard[x][y] = srcBoard[x][y];
            }
        }
    }
    //castling and en passant handling
    //en passant
    if(move->piece->type == Pawn && (move->destination->x == move->piece->field->x+1 || move->destination->x == move->piece->field->x-1) && move->destination->piece == NULL)
    {
        if(move->piece->color == White)
            dstBoard[move->destination->x][move->destination->y-1].piece = NULL;
        else
            dstBoard[move->destination->x][move->destination->y+1].piece = NULL;
    }
    //castling
    if(move->piece->type == King)
    {
        //scastle - rook moves too
        if(move->destination->x == move->piece->field->x+2)
        {
            Piece *castlingRook = dstBoard[7][move->destination->y].piece;
            dstBoard[7][move->destination->y].piece = NULL;
            dstBoard[5][move->destination->y].piece = castlingRook;
        }
        else if(move->destination->x == move->piece->field->x-2) //lcastle
        {
            Piece *castlingRook = dstBoard[0][move->destination->y].piece;
            dstBoard[0][move->destination->y].piece = NULL;
            dstBoard[3][move->destination->y].piece = castlingRook;
        }
    }
    //general captures
    dstBoard[move->piece->field->x][move->piece->field->y].piece = NULL;
    //move to board
    dstBoard[move->destination->x][move->destination->y].piece = move->piece;
}
/*
This is the function to be called after the validity of the move was confirmed.
However, it does not append the move to the moveChain.
*/
void commitMove(Move *move, Field fields[8][8], PieceType pawnTransform)
{
    //handle captures
    if(move->destination->piece != NULL)
    {
        move->destination->piece->onBoard = false;
        move->destination->piece->field = NULL;
    }
    //en passant - capture handled before move commited
    if(move->piece->type == Pawn && (move->destination->x == move->piece->field->x+1 || move->destination->x == move->piece->field->x-1) && move->destination->piece == NULL)
    {
        if(move->piece->color == White)
        {
            fields[move->destination->x][move->destination->y-1].piece->field = NULL;
            fields[move->destination->x][move->destination->y-1].piece->onBoard = false;
        }

        else
        {
            fields[move->destination->x][move->destination->y+1].piece->field = NULL;
            fields[move->destination->x][move->destination->y+1].piece->onBoard = false;
        }

    }
    //to board
    moveToBoard(fields, fields, move);
    //in castling there is no capture, we just overwrite the position of the rook too
    if(move->piece->type == King)
    {
        //scastle
        if(move->destination->x == move->piece->field->x+2)
        {
            fields[5][move->destination->y].piece->field = &fields[5][move->destination->y];
        }
        else if(move->destination->x == move->piece->field->x-2) //lcastle
        {
            fields[3][move->destination->y].piece->field = &fields[3][move->destination->y];
        }
    }
    move->piece->field = move->destination;
    if(move->piece->type == Pawn)
    {
        if((move->destination->y == 7 && move->piece->color == White) || (move->destination->y == 0 && move->piece->color == Black))
            move->piece->type = pawnTransform;
    }
}
/*
Returns if a situation on field is check for one side.
Recalls the getPieceMoves function, however the removeChecks parameter will become false, therefore isCheck won't be recalled.
*/
bool isCheck(PieceColor checkFor, Field fields[8][8], GameMove const *moveChain)
{
    //search for the king of the color
    int kingX, kingY;
    bool found = false; //otherwise we would need to break from two loops
    for(int x = 0; (x < 8) && !found; ++x)
    {
        for(int y = 0; (y < 8) && !found; ++y)
        {
            Piece *piece = fields[x][y].piece;
            if(piece != NULL)
            {
                if(piece->color == checkFor && piece->type == King)
                {
                    //The moves might be simulated, then the pointer solution would be more complex
                    kingX = x;
                    kingY = y;
                    found = true;
                }
            }

        }
    }
    bool check = false;
    if(found) //the game goes on, whenever a king was captured somehow...
    {
        for(int x = 0; x < 8 && !check; ++x)
        {
            for(int y = 0; y < 8 && !check; ++y)
            {
                if(fields[x][y].piece != NULL && fields[x][y].piece->color != checkFor)
                {
                    PieceColor attacker = (checkFor == White) ? Black : White;
                    Move *pieceMoves = NULL;
                    int moveNum = getPieceMoves(attacker, fields[x][y].piece, fields, &pieceMoves, false, moveChain);
                    for(int i = 0; i < moveNum && !check; ++i)
                    {
                        if(pieceMoves[i].destination->x == kingX && pieceMoves[i].destination->y == kingY)
                            check = true;
                    }
                    free(pieceMoves);
                }
            }
        }
    }
    return check;
}
//Simulates a move, and returns whether it would cause check.
bool wouldBeCheck(PieceColor checkFor, Move const *move, Field fields[8][8], GameMove const *moveChain)
{
    Field tempFields[8][8];
    moveToBoard(fields, tempFields, move);
    return isCheck(checkFor, tempFields, moveChain);
}
//Removes moves from an array that would cause check for a given player
void removeCheckMoves(Move *moves, int size, int *validCount, PieceColor color, Field fields[8][8], GameMove const *moveChain)
{
    for(int i = 0; i < size; ++i)
    {
        if(moves[i].piece != NULL && moves[i].destination != NULL && wouldBeCheck(color, &moves[i], fields, moveChain))
        {
            moves[i].piece = NULL;
            moves[i].destination = NULL;
            --(*validCount);
        }
    }
}
//returns if piece was found and sets xc and yc to the position of the field of the piece
bool searchPieceInFields(Piece *piece, Field fields[8][8], int *xc, int *yc)
{
    if(piece != NULL && piece->onBoard) //at least i tried to prevent misuse...
    {
        for(int x = 0; x < 8; ++x)
        {
            for(int y = 0; y < 8; ++y)
            {
                if(fields[x][y].piece != NULL && fields[x][y].piece == piece)
                {
                    *xc = x;
                    *yc = y;
                    return true;
                }
            }
        }
    }
    return false;
}
bool initCoordinates(Piece *piece, Field fields[8][8], bool removeChecks, int *x, int *y)
{
    //When wouldBeCheck functions recalls this function, the coordinates would come from a simulated field set instead of the original one.
    if(removeChecks)
    {
        *x = piece->field->x;
        *y = piece->field->y;
    }
    else
    {
		//prevents incorrect references when field set is simulated (piece->field would point to the original field array)
        if(!searchPieceInFields(piece, fields, x, y))
        {
            return false;
        }
    }
    return true;
}
/*
From getPawnMoves till getKingMoves every function returns the size(!!!) of a dynamically allocated move set for the corresponding piece,
given to them as parameters along with the field set, a moves pointer, the move chain and a boolean parameter for indicating if checks should be removed from the final set of moves.
This is useful, when we search for pieces that could capture the king in isCheck()/wouldBeCheck() function.
Calling free() for the moves pointer would be the responsibility of the caller, if they weren't static.
However, I DO NOT RECOMMEND MAKING THESE FUNCTIONS PUBLIC, their full functionality is available from getPieceMoves() functions
This is the 'heavy part'...
*/
static int getPawnMoves(Piece *piece, Field fields[8][8], Move **moves, GameMove const *moveChain, bool removeChecks)
{
    int x = 0, y = 0;
    if(!initCoordinates(piece, fields, removeChecks, &x, &y))
    {
        *moves = NULL;
        return 0;
    }
    //A pawn can have altogether 4 moves 0: one step forward, 1: two steps forward, 2: capture right cross, 3: capture leftcross (including en passant)
    Move tempMoves[4];
    for(int i = 0; i < 4; ++i)
    {
        tempMoves[i] = (Move){NULL, NULL, NULL, false, NULL, NULL};
    }
    int count = 0;
    if(piece->color == White)
    {
        if(fieldExists(x,y+1) && !hasPiece(&fields[x][y+1]))
        {
            /*
            If someone's interested:
            Defining tempMoves values has the following logic:
            A Move structure contains the moving piece, its destination, its current field, whether the move caused a pawn to transform,
            the captured piece, and the field from where the captured piece was taken - Note that in case of en passant, it could be different from the destination.
            The variables referring to a captured piece are NULL, when no capture occured. This structure makes it possible to undo Moves easily
            */
            tempMoves[0] = (Move){piece, &fields[x][y], &fields[x][y+1], (y==7), NULL, NULL};
            ++count;
            if(y == 1 && fieldExists(x,y+2) && !hasPiece(&fields[x][y+2]))
            {
                //pawn can't transform upon its first move, and cannot capture frontally
                tempMoves[1] = (Move){piece, &fields[x][y], &fields[x][y+2], false, NULL, NULL};
                ++count;
            }
        }
        if(moveChain != NULL)
        {
            //rcross - does capture, but might be en passant, therefore when a cross move is considered valid, and the destination field doesn't have a piece, the captured piece has a differnet position.
            if(fieldExists(x+1,y+1) && (((hasPiece(&fields[x+1][y+1]) && fields[x+1][y+1].piece->color == Black)) || (y == 4 && fieldExists(x+1,y) && moveChain->previous->move.destination->x == x+1 && moveChain->previous->move.destination->y == y && hasPiece(&fields[x+1][y]) && fields[x+1][y].piece->color == Black && fields[x+1][y].piece->type == Pawn && !pieceMoved(fields[x+1][y].piece, moveChain->previous->previous))))
            {
                tempMoves[2] = (Move){piece, &fields[x][y], &fields[x+1][y+1], (y==7), (fields[x+1][y+1].piece == NULL) ? fields[x+1][y].piece : fields[x+1][y+1].piece, (fields[x+1][y+1].piece == NULL) ? &fields[x+1][y] : &fields[x+1][y+1]};
                ++count;
            }
            //lcross
            if(fieldExists(x-1,y+1) && (((hasPiece(&fields[x-1][y+1]) && fields[x-1][y+1].piece->color == Black)) || (y == 4 && fieldExists(x-1,y) && moveChain->previous->move.destination->x == x-1 && moveChain->previous->move.destination->y == y && hasPiece(&fields[x-1][y]) && fields[x-1][y].piece->color == Black && fields[x-1][y].piece->type == Pawn && !pieceMoved(fields[x-1][y].piece, moveChain->previous->previous))))
            {
                tempMoves[3] = (Move){piece, &fields[x][y], &fields[x-1][y+1], (y==7), (fields[x+1][y+1].piece == NULL) ? fields[x-1][y].piece : fields[x-1][y+1].piece, (fields[x-1][y+1].piece == NULL) ? &fields[x-1][y] : &fields[x-1][y+1]};
                ++count;
            }
        }
    }
    else
    {
        if(fieldExists(x,y-1) && !hasPiece(&fields[x][y-1]))
        {
            tempMoves[0] = (Move){piece, &fields[x][y], &fields[x][y-1], (y==0), NULL, NULL};
            ++count;
            if(y == 6 && fieldExists(x,y-2) && !hasPiece(&fields[x][y-2]))
            {
                tempMoves[1] = (Move){piece, &fields[x][y], &fields[x][y-2], (y==0), NULL, NULL};
                ++count;
            }
        }
        if(moveChain != NULL)
        {
            //rcross - from white's perspective
            if(fieldExists(x+1,y-1) && (((hasPiece(&fields[x+1][y-1]) && fields[x+1][y-1].piece->color == White)) || (y == 3 && fieldExists(x+1,y) && moveChain->previous->move.destination->x == x+1 && moveChain->previous->move.destination->y == y && hasPiece(&fields[x+1][y]) && fields[x+1][y].piece->color == White && fields[x+1][y].piece->type == Pawn && !pieceMoved(fields[x+1][y].piece, moveChain->previous->previous))))
            {
                tempMoves[2] = (Move){piece, &fields[x][y], &fields[x+1][y-1], (y==0), (fields[x+1][y-1].piece == NULL) ? fields[x+1][y].piece : fields[x+1][y-1].piece, (fields[x+1][y-1].piece == NULL) ? &fields[x+1][y] : &fields[x+1][y-1]};
                ++count;
            }
            //lcross
            if(fieldExists(x-1,y-1) && (((hasPiece(&fields[x-1][y-1]) && fields[x-1][y-1].piece->color == White)) || ( y == 3 && fieldExists(x-1,y) && moveChain->previous->move.destination->x == x-1 && moveChain->previous->move.destination->y == y && hasPiece(&fields[x-1][y]) && fields[x-1][y].piece->color == White && fields[x-1][y].piece->type == Pawn && !pieceMoved(fields[x-1][y].piece, moveChain->previous->previous))))
            {
                tempMoves[3] = (Move){piece, &fields[x][y], &fields[x-1][y-1], (y==0), (fields[x-1][y-1].piece == NULL) ? fields[x-1][y].piece : fields[x-1][y-1].piece, (fields[x-1][y-1].piece == NULL) ? &fields[x-1][y] : &fields[x-1][y-1]};
                ++count;
            }
        }

    }
    //remove moves that discover check for moving color - only when needed
    if(removeChecks)
    {
        removeCheckMoves(tempMoves, 4 , &count, piece->color, fields, moveChain);
    }
    //now we can allocate the memory
    Move *validMoves = (Move*)malloc(count*sizeof(Move));
    int validIndex = 0;
    for(int i = 0; i < 4; ++i)
    {
        if(tempMoves[i].destination != NULL && tempMoves[i].piece != NULL)
            validMoves[validIndex++] = tempMoves[i];
    }
    *moves = validMoves;
    return count;
}
static int getRookMoves(Piece *piece, Field fields[8][8], Move **moves, GameMove const *moveChain, bool removeChecks)
{
    int x = 0, y = 0;
    if(!initCoordinates(piece, fields, removeChecks, &x, &y))
    {
        *moves = NULL;
        return 0;
    }
    Move horizontal[7], vertical[7];
    int hIndex = 0, vIndex = 0;
    for(int i = 0; i < 7; ++i)
    {
        horizontal[i] = (Move){NULL, NULL, NULL, false, NULL, NULL};
        vertical[i] = (Move){NULL, NULL, NULL, false, NULL, NULL};
    }
    //field surely exists
    for(int i = x+1; i < 8; ++i)
    {
        if(hasPiece(&fields[i][y]))
        {
            if(fields[i][y].piece->color != piece->color) //capture case
                horizontal[hIndex++] = (Move){piece, &fields[x][y], &fields[i][y], false, fields[i][y].piece, &fields[i][y]};
            break;
        }
        else
        {
            horizontal[hIndex++] = (Move){piece, &fields[x][y], &fields[i][y], false, NULL, NULL};
        }
    }
    for(int i = x-1; i >= 0; --i)
    {
        if(hasPiece(&fields[i][y]))
        {
            if(fields[i][y].piece->color != piece->color) //capture case
                horizontal[hIndex++] = (Move){piece, &fields[x][y], &fields[i][y], false, fields[i][y].piece, &fields[i][y]};
            break;
        }
        else
        {
            horizontal[hIndex++] = (Move){piece, &fields[x][y], &fields[i][y], false, NULL, NULL};
        }
    }
    for(int i = y+1; i < 8; ++i)
    {
        if(hasPiece(&fields[x][i]))
        {
            if(fields[x][i].piece->color != piece->color) //capture case
                vertical[vIndex++] = (Move){piece, &fields[x][y], &fields[x][i], false, fields[i][y].piece, &fields[i][y]};
            break;
        }
        else
        {
            vertical[vIndex++] = (Move){piece, &fields[x][y], &fields[x][i], false, NULL, NULL};
        }
    }
    for(int i = y-1; i >= 0; --i)
    {
        if(hasPiece(&fields[x][i]))
        {
            if(fields[x][i].piece->color != piece->color) //capture case
                vertical[vIndex++] = (Move){piece, &fields[x][y], &fields[x][i], false, fields[i][y].piece, &fields[i][y]};
            break;
        }
        else
        {
            vertical[vIndex++] = (Move){piece, &fields[x][y], &fields[x][i], false, NULL, NULL};
        }
    }
    int count = vIndex+hIndex;
    if(removeChecks)
    {
        removeCheckMoves(vertical, 7 , &count, piece->color, fields, moveChain);
        removeCheckMoves(horizontal, 7 , &count, piece->color, fields, moveChain);
    }
    Move *validMoves = (Move*)malloc(count*sizeof(Move));
    if(validMoves == NULL)
    {
        *moves = NULL;
        return 0;
    }
    int validIndex = 0;
    for(int i = 0; i < vIndex; ++i)
    {
        if(vertical[i].destination != NULL && vertical[i].piece != 0)
            validMoves[validIndex++] = vertical[i];
    }
    for(int i = 0; i < hIndex; ++i)
    {
        if(horizontal[i].destination != NULL && horizontal[i].piece != 0)
            validMoves[validIndex++] = horizontal[i];
    }
    *moves = validMoves;
    return count;
}
static int getKnightMoves(Piece *piece, Field fields[8][8], Move **moves, GameMove const *moveChain, bool removeChecks)
{
    int x = 0, y = 0;
    if(!initCoordinates(piece, fields, removeChecks, &x, &y))
    {
        *moves = NULL;
        return 0;
    }
    Move tempMoves[8];
    for(int i = 0; i < 8; ++i)
    {
        tempMoves[i] = (Move){NULL, NULL, NULL, false, NULL};
    }
    int horizontal = 0, vertical = 0, count = 0;
    for(int i = 0; i < 8; ++i)
    {
        switch(i)
        {
            case 0:
                horizontal = 1;
                vertical = 2;
                break;
            case 1:
                horizontal = -1;
                vertical = 2;
                break;
            case 2:
                horizontal = 2;
                vertical = 1;
                break;
            case 3:
                horizontal = -2;
                vertical = 1;
                break;
            case 4:
                horizontal = 1;
                vertical = -2;
                break;
            case 5:
                horizontal = -1;
                vertical = -2;
                break;
            case 6:
                horizontal = 2;
                vertical = -1;
                break;
            case 7:
                horizontal = -2;
                vertical = -1;
                break;

        }
        if(fieldExists(x+horizontal,y+vertical))
        {
            if(hasPiece(&fields[x+horizontal][y+vertical]))
            {
                if(fields[x+horizontal][y+vertical].piece->color != piece->color) //capture case
                    tempMoves[count++] = (Move){piece, &fields[x][y], &fields[x+horizontal][y+vertical], false, fields[x][y].piece, &fields[x][y]};
            }
            else
            {
                tempMoves[count++] = (Move){piece, &fields[x][y], &fields[x+horizontal][y+vertical], false, NULL, NULL};
            }
        }
    }
    if(removeChecks)
    {
        removeCheckMoves(tempMoves, 8, &count, piece->color, fields, moveChain);
    }
    Move *validMoves = (Move*)malloc(count*sizeof(Move));
    if(validMoves == NULL)
    {
        *moves = NULL;
        return 0;
    }
    int validIndex = 0;
    for(int i = 0; i < 8; ++i)
    {
        if(tempMoves[i].destination != NULL && tempMoves[i].piece != NULL)
            validMoves[validIndex++] = tempMoves[i];
    }
    *moves = validMoves;
    return count;
}
static int getBishopMoves(Piece *piece, Field fields[8][8], Move **moves, GameMove const *moveChain, bool removeChecks)
{
    int x = 0, y = 0;
    if(!initCoordinates(piece, fields, removeChecks, &x, &y))
    {
        *moves = NULL;
        return 0;
    }
    Move slash[7], backslash[7];
    int sIndex = 0, bIndex = 0;
    for(int i = 0; i < 7; ++i)
    {
        slash[i] = (Move){NULL, NULL};
        backslash[i] = (Move){NULL, NULL, NULL};
    }
    //field surely exists
    int i = x, j = y;
    while(fieldExists(++i,++j))
    {
        if(hasPiece(&fields[i][j]))
        {
            if(fields[i][j].piece->color != piece->color) //capture case
                slash[sIndex++] = (Move){piece, &fields[x][y], &fields[i][j], false, fields[i][j].piece, &fields[i][j]};
            break;
        }
        else
        {
            slash[sIndex++] = (Move){piece, &fields[x][y], &fields[i][j], false, NULL, NULL};
        }
    }
    i = x, j = y;
    while(fieldExists(--i,--j))
    {
        if(hasPiece(&fields[i][j]))
        {
            if(fields[i][j].piece->color != piece->color) //capture case
                slash[sIndex++] = (Move){piece, &fields[x][y], &fields[i][j], false, fields[i][j].piece, &fields[i][j]};
            break;
        }
        else
        {
            slash[sIndex++] = (Move){piece, &fields[x][y], &fields[i][j], false, NULL, NULL};
        }
    }
    i = x, j = y;
    while(fieldExists(--i,++j))
    {
        if(hasPiece(&fields[i][j]))
        {
            if(fields[i][j].piece->color != piece->color) //capture case
                slash[sIndex++] = (Move){piece, &fields[x][y], &fields[i][j], false, fields[i][j].piece, &fields[i][j]};
            break;
        }
        else
        {
            backslash[bIndex++] = (Move){piece, &fields[x][y], &fields[i][j], false, NULL, NULL};
        }
    }
    i = x, j = y;
    while(fieldExists(++i,--j))
    {
        if(hasPiece(&fields[i][j]))
        {
            if(fields[i][j].piece->color != piece->color)
                backslash[bIndex++] = (Move){piece, &fields[x][y], &fields[i][j], false, fields[i][j].piece, &fields[i][j]};
            break;
        }
        else
        {
            backslash[bIndex++] = (Move){piece, &fields[x][y], &fields[i][j], false, NULL, NULL};
        }
    }
    int count = sIndex+bIndex;
    if(removeChecks)
    {
        removeCheckMoves(slash, 7, &count, piece->color, fields, moveChain);
        removeCheckMoves(backslash, 7, &count, piece->color, fields, moveChain);
    }
    Move *validMoves = (Move*)malloc(count*sizeof(Move));
    if(validMoves == NULL)
    {
        *moves = NULL;
        return 0;
    }
    int validIndex = 0;
    for(int i = 0; i < 7; ++i)
    {
        if(slash[i].destination != NULL && slash[i].piece != NULL)
            validMoves[validIndex++] = slash[i];
    }
    for(int i = 0; i < 7; ++i)
    {
        if(backslash[i].destination != NULL && backslash[i].piece != NULL)
            validMoves[validIndex++] = backslash[i];
    }
    *moves = validMoves;
    return count;
}
/*
Warning! This function does not check whether the returned pointers of the getRookMoves() and getBishopMoves are NULL
Even if it happens, it allocates a zero-sized memory, that still needs to be freed.
*/
static int getQueenMoves(Piece *piece, Field fields[8][8], Move **moves, GameMove const *moveChain, bool removeChecks)
{
    Move *rookMoves = NULL, *bishopMoves = NULL;
    int rookNum = getRookMoves(piece, fields, &rookMoves, moveChain, removeChecks);
    int bishopNum = getBishopMoves(piece, fields, &bishopMoves, moveChain, removeChecks);
    Move *validMoves = (Move*)malloc((rookNum+bishopNum)*sizeof(Move));
    int count = 0;
    for(int i = 0; i < rookNum; ++i)
    {
        validMoves[count++] = rookMoves[i];
    }
    for(int i = 0; i < bishopNum; ++i)
    {
        validMoves[count++] = bishopMoves[i];
    }
    free(bishopMoves);
    free(rookMoves);
    *moves = validMoves;
    return count;
}
static int getKingMoves(Piece *piece, Field fields[8][8], Move **moves, GameMove const *moveChain, bool removeChecks)
{
    int x = 0, y = 0;
    if(!initCoordinates(piece, fields, removeChecks, &x, &y))
    {
        *moves = NULL;
        return 0;
    }
    Move tempMoves[8];
    for(int i = 0; i < 8; ++i)
    {
        tempMoves[i] = (Move){NULL, NULL, NULL};
    }
    int horizontal = 0, vertical = 0, count = 0;
    for(int i = 0; i < 8; ++i)
    {
        switch(i)
        {
            case 0:
                horizontal = 0;
                vertical = 1;
                break;
            case 1:
                horizontal = 0;
                vertical = -1;
                break;
            case 2:
                horizontal = 1;
                vertical = 0;
                break;
            case 3:
                horizontal = -1;
                vertical = 0;
                break;
            case 4:
                horizontal = 1;
                vertical = 1;
                break;
            case 5:
                horizontal = -1;
                vertical = 1;
                break;
            case 6:
                horizontal = 1;
                vertical = -1;
                break;
            case 7:
                horizontal = -1;
                vertical = -1;
                break;
        }
        if(fieldExists(x+horizontal,y+vertical))
        {
            if(hasPiece(&fields[x+horizontal][y+vertical]))
            {
                if(fields[x+horizontal][y+vertical].piece->color != piece->color) //capture case
                    tempMoves[count++] = (Move){piece, &fields[x][y], &fields[x+horizontal][y+vertical], false, fields[x][y].piece, &fields[x][y]};
            }
            else
            {
                tempMoves[count++] = (Move){piece, &fields[x][y], &fields[x+horizontal][y+vertical], false, NULL, NULL};
            }
        }
    }
    if(removeChecks)
    {
        removeCheckMoves(tempMoves, 8, &count, piece->color, fields, moveChain);
    }
    //Add castling
    bool sCastle = false, lCastle = false;
    if(!pieceMoved(piece, moveChain) && removeChecks) //only that double move of the king doesn't cause check for opponent
    {
        //short castle
        if(hasPiece(&fields[7][y]) && !hasPiece(&fields[6][y]) && !hasPiece(&fields[5][y]))
        {
            if(fields[7][y].piece->type == Rook && !pieceMoved(fields[7][y].piece, moveChain))
            {
                Move first = (Move){piece, &fields[x][y], &fields[5][y], false, NULL, NULL};
                Move castle = (Move){piece, &fields[x][y], &fields[6][y], false, NULL, NULL};
                if(!isCheck(piece->color, fields, moveChain) && !wouldBeCheck(piece->color, &first, fields, moveChain) && !wouldBeCheck(piece->color, &castle, fields, moveChain))
                {
                    sCastle = true;
                    ++count;
                }
            }
        }
        //long castle
        if(hasPiece(&fields[0][y]) && !hasPiece(&fields[1][y]) && !hasPiece(&fields[2][y]) && !hasPiece(&fields[3][y]))
        {
            if(fields[0][y].piece->type == Rook && !pieceMoved(fields[0][y].piece, moveChain))
            {
                Move first = (Move){piece, &fields[x][y], &fields[3][y], false, NULL, NULL};
                Move castle = (Move){piece, &fields[x][y], &fields[2][y], false, NULL, NULL};
                if(!isCheck(piece->color, fields, moveChain) && !wouldBeCheck(piece->color, &first, fields, moveChain) && !wouldBeCheck(piece->color, &castle, fields, moveChain))
                {
                    lCastle = true;
                    ++count;
                }
            }
        }
    }
    Move *validMoves = (Move*)malloc(count*sizeof(Move));
    if(validMoves == NULL)
    {
        *moves = NULL;
        return 0;
    }
    int validIndex = 0;
    for(int i = 0; i < 8; ++i)
    {
        if(tempMoves[i].destination != NULL && tempMoves[i].piece != NULL)
            validMoves[validIndex++] = tempMoves[i];
    }
    if(sCastle)
        validMoves[validIndex++] = (Move){piece, &fields[x][y], &fields[6][y], false, NULL, NULL};
    if(lCastle)
        validMoves[validIndex++] = (Move){piece, &fields[x][y], &fields[2][y], false, NULL, NULL};
    *moves = validMoves;
    return count;
}
/*
This function gets all possible moves for a given piece. Expects the moving color, a piece pointer, the fields set
and a pointer that will be set to the beginning of a dynamically allocated set of moves, for which calling free() is the responsibility of the caller.
Returns the size(!!) of a dynamically allocated array of moves.
*/
int getPieceMoves(PieceColor movingColor, Piece *piece, Field fields[8][8], Move **result, bool removeChecks, GameMove const *moveChain)
{
    Move *pieceMoves = NULL;
    int size = 0;
    if(piece->onBoard && piece->color == movingColor)
    {
        switch(piece->type)
        {
            case Pawn:
                size = getPawnMoves(piece, fields, &pieceMoves, moveChain, removeChecks);
                break;
            case Rook:
                size = getRookMoves(piece, fields, &pieceMoves, moveChain, removeChecks);
                break;
            case Knight:
                size = getKnightMoves(piece, fields, &pieceMoves, moveChain, removeChecks);
                break;
            case Bishop:
                size = getBishopMoves(piece, fields, &pieceMoves, moveChain, removeChecks);
                break;
            case Queen:
                size = getQueenMoves(piece, fields, &pieceMoves, moveChain, removeChecks);
                break;
            case King:
                size = getKingMoves(piece, fields, &pieceMoves, moveChain, removeChecks);
                break;
            default:
                size = 0;
                break;
        }
    }
    *result = pieceMoves;
    return size;

}
/*
Returns the sum of possible moves for every piece.
Expects the moving color, the piece set, field set, an array of pointers for the moves and another array for the corresponding sizes of the matrix.
Attention! Multiple dynamically allocated arrays will be returned, all of which need to be freed iteratively after use.
*/
int getValidMoves(PieceColor movingColor, Piece pieces[32], Field fields[8][8], GameMove *moveChain, bool removeChecks, Move *moves[32], int sizes[32])
{
    int sum = 0;
    for(int i = 0; i < 32; ++i)
    {
        sizes[i] = getPieceMoves(movingColor, &pieces[i], fields, &moves[i], removeChecks, moveChain);
        sum += sizes[i];
    }
    return sum;
}
/*
This could be used to free an array from getValidMoves
*/
void freeValidMoves(Move *validMoves[32])
{
    for(int i = 0; i < 32; ++i)
        free(validMoves[i]);
}
//removes checks
bool isValidMove(Move *move, PieceColor movingColor, Field fields[8][8], GameMove *movechain)
{
    Move *validMoves = NULL;
    int size = getPieceMoves(movingColor, move->piece, fields, &validMoves, true, movechain);
    if(validMoves != NULL)
    {
        for(int i = 0; i < size; ++i)
        {
            if(validMoves[i].destination == move->destination && validMoves[i].piece == move->piece)
            {
                free(validMoves);
                return true;
            }
        }
    }
    free(validMoves);
    return false;
}
/*
supposes that the given piece is a bishop - good for determining the color of the field of any piece
returns integer (0 or 1), so that we can choose a default value when determining whether the bishops are of the same color
0 means black, 1 means white field
*/
int getBishopFieldColor(Piece *bishop)
{
    return ((bishop->field->x + bishop->field->y) % 2 == 0) ? 0 : 1;
}
int countOnBoard(Piece pieces[32])
{
    int onBoard = 0;
    for(int i = 0; i < 32; ++i)
    {
        if(pieces[i].onBoard)
            ++onBoard;
    }
    return onBoard;
}
