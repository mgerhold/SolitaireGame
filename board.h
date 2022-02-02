#ifndef BOARD_H
#define BOARD_H

#include "raylib.h"
#include "vector2int.h"
#include "movelist.h"
#include <stdbool.h>

typedef struct {
    int fieldDimension;
    float boardRadius;
    float innerFieldHalfWidth;
    float fieldMargin;
    float fieldRadius;
    Color boardColor;
    Color ballColor;
    Color emptyFieldColor;
    Color selectedBallColor;
    Color possibleMoveTargetColor;
} BoardSettings;

typedef enum {
    FIELDSTATE_BLOCKED, // no ball can ever reach this field
    FIELDSTATE_FREE,    // this is a field that can be reached
    FIELDSTATE_OCCUPIED,// this is a field that can be reached with a ball on it
} FieldState;

struct Board;
typedef struct Board Board;

Board* boardCreate(void);
bool boardAreCoordsValid(Board* board, Vector2Int coords);
void boardClearSelection(Board* board);
Vector2Int boardGetSelectedBall(Board* board);
void boardSetSelectedBall(Board* board, Vector2Int selected);
const MoveList* boardGetPossibleMoves(Board* board);
void boardSetFieldState(Board* board, int x, int y, FieldState state);
FieldState boardGetFieldState(Board* board, int x, int y);
bool boardContainsPossibleMove(Board* board, Move move);
bool boardIsSelectionActive(Board* board);
bool boardBallCanJump(Board* board, Vector2Int from, Vector2Int to);
void boardDestroy(Board** board);

#endif