#include "board.h"
#include "utility.h"
#include "defines.h"
#include <math.h>
#include <stdlib.h>

struct Board {
    FieldState* fields;
    Vector2Int selectedBall;
    MoveList* possibleMoves;
    int fieldDimension;
};

static void addMoveIfValid(Board* board, Vector2Int from, Vector2Int to) {
    if (boardBallCanJump(board, from, to)) {
        moveListAdd(board->possibleMoves, (Move){ from, to });
    }
}

static void calculatePossibleMoves(Board* board) {
    moveListClear(board->possibleMoves);
    for (int x = 0; x < board->fieldDimension; ++x) {
        for (int y = 0; y < board->fieldDimension; ++y) {
            if (!boardAreCoordsValid(board, (Vector2Int){ x, y })) {
                continue;
            }
            const Vector2Int currentPosition = { x, y };
            addMoveIfValid(board, currentPosition, (Vector2Int){ currentPosition.x + 2, currentPosition.y });
            addMoveIfValid(board, currentPosition, (Vector2Int){ currentPosition.x - 2, currentPosition.y });
            addMoveIfValid(board, currentPosition, (Vector2Int){ currentPosition.x, currentPosition.y + 2 });
            addMoveIfValid(board, currentPosition, (Vector2Int){ currentPosition.x, currentPosition.y - 2 });
        }
    }
}

static void initFields(Board* board) {
    for (int x = 0; x < board->fieldDimension; ++x) {
        for (int y = 0; y < board->fieldDimension; ++y) {
            FieldState state = FIELDSTATE_BLOCKED;
            if (x == board->fieldDimension / 2 && y == board->fieldDimension / 2) {
                // middle field
                state = FIELDSTATE_FREE;
            } else if (boardAreCoordsValid(board, (Vector2Int){ x, y })) {
                state = FIELDSTATE_OCCUPIED;
            }
            boardSetFieldState(board, x, y, state);
        }
    }
}

Board* boardCreate(void) {
    Board* board = malloc(sizeof(*board));
    board->fields = malloc(sizeof(*board->fields) * BOARD_DIMENSION * BOARD_DIMENSION);
    board->selectedBall = (Vector2Int){ -1, -1 };
    board->possibleMoves = moveListCreate();
    board->fieldDimension = BOARD_DIMENSION;
    initFields(board);
    calculatePossibleMoves(board);
    return board;
}

bool boardAreCoordsValid(Board* board, Vector2Int coords) {
    return (coords.x >= 0 && coords.y >= 0 && coords.x < board->fieldDimension && coords.y < board->fieldDimension) &&
           ((coords.x > board->fieldDimension / 4 && coords.x < board->fieldDimension * 3 / 4) ||
            (coords.y > board->fieldDimension / 4 && coords.y < board->fieldDimension * 3 / 4));
}

void boardClearSelection(Board* board) {
    board->selectedBall = (Vector2Int){ -1, -1 };
}

Vector2Int boardGetSelectedBall(Board* board) {
    return board->selectedBall;
}

void boardSetSelectedBall(Board* board, Vector2Int selected) {
    board->selectedBall = selected;
}

const MoveList* boardGetPossibleMoves(Board* board) {
    return board->possibleMoves;
}

void boardSetFieldState(Board* board, int x, int y, FieldState state) {
    board->fields[x + y * board->fieldDimension] = state;
    calculatePossibleMoves(board);
}

FieldState boardGetFieldState(Board* board, int x, int y) {
    return board->fields[x + y * board->fieldDimension];
}

bool boardContainsPossibleMove(Board* board, Move move) {
    return moveListContains(board->possibleMoves, move);
}

bool boardIsSelectionActive(Board* board) {
    return board->selectedBall.x >= 0 && board->selectedBall.y >= 0;
}

bool boardBallCanJump(Board* board, Vector2Int from, Vector2Int to) {
    if (!boardAreCoordsValid(board, from) || !boardAreCoordsValid(board, to)) {
        return false;
    }
    const Vector2Int direction = { to.x - from.x, to.y - from.y };
    const int manhattanDistance = abs(direction.x) + abs(direction.y);
    if ((direction.x != 0 && direction.y != 0) || manhattanDistance != 2) {
        return false;
    }
    if (boardGetFieldState(board, from.x, from.y) != FIELDSTATE_OCCUPIED) {
        return false;
    }
    if (boardGetFieldState(board, to.x, to.y) != FIELDSTATE_FREE) {
        return false;
    }
    const Vector2Int inBetweenPosition = getMiddle(from, to);
    if (boardGetFieldState(board, inBetweenPosition.x, inBetweenPosition.y) != FIELDSTATE_OCCUPIED) {
        return false;
    }
    return true;
}

void boardDestroy(Board** board) {
    moveListDestroy(&(*board)->possibleMoves);
    free((*board)->fields);
    free(*board);
    *board = NULL;
}