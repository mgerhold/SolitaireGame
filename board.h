#ifndef BOARD_H
#define BOARD_H

#include "board_fwd.h"
#include "raylib.h"
#include "vector2int.h"
#include "movelist.h"
#include "balllist.h"
#include <stdbool.h>

typedef enum {
    FIELDSTATE_BLOCKED, // no ball can ever reach this field
    FIELDSTATE_FREE,    // this is a field that can be reached
    FIELDSTATE_OCCUPIED,// this is a field that can be reached with a ball on it
} FieldState;

Board* boardCreate(void);
bool boardAreCoordsValid(const Board* board, Vector2Int coords);
void boardClearSelection(Board* board);
Vector2Int boardGetSelectedBall(const Board* board);
void boardSetSelectedBall(Board* board, Vector2Int selected);
const MoveList* boardGetPossibleMoves(const Board* board);
void boardSetFieldState(Board* board, int x, int y, FieldState state);
FieldState boardGetFieldState(const Board* board, int x, int y);
bool boardContainsPossibleMove(const Board* board, Move move);
bool boardHasPossibleMoves(const Board* board);
bool boardIsSelectionActive(const Board* board);
bool boardBallCanJump(const Board* board, Vector2Int from, Vector2Int to);
int boardGetNumberOfBalls(const Board* board);
void boardDoJump(Board* board, Vector2Int from, Vector2Int to);
BallListNode* boardGetDepositedBalls(const Board* board);
void boardDestroy(Board** board);

#endif
