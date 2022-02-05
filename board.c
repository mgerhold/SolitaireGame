#include "board.h"
#include "utility.h"
#include "defines.h"
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <float.h>

struct Board {
    FieldState* fields;
    Vector2Int selectedBall;
    MoveList* possibleMoves;
    BallListNode* depositedBalls;
    int fieldDimension;
};

static void addMoveIfValid(Board* const board, const Vector2Int from, const Vector2Int to) {
    if (boardBallCanJump(board, from, to)) {
        moveListAdd(board->possibleMoves, (Move){ from, to });
    }
}

static void calculatePossibleMoves(Board* const board) {
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

static void initFields(Board* const board) {
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
    board->depositedBalls = NULL;
    initFields(board);
    calculatePossibleMoves(board);
    return board;
}

bool boardAreCoordsValid(Board const* const board, const Vector2Int coords) {
    return (coords.x >= 0 && coords.y >= 0 && coords.x < board->fieldDimension && coords.y < board->fieldDimension) &&
           ((coords.x > board->fieldDimension / 4 && coords.x < board->fieldDimension * 3 / 4) ||
            (coords.y > board->fieldDimension / 4 && coords.y < board->fieldDimension * 3 / 4));
}

void boardClearSelection(Board* const board) {
    board->selectedBall = (Vector2Int){ -1, -1 };
}

Vector2Int boardGetSelectedBall(Board const* const board) {
    return board->selectedBall;
}

void boardSetSelectedBall(Board* const board, const Vector2Int selected) {
    board->selectedBall = selected;
}

const MoveList* boardGetPossibleMoves(Board const* const board) {
    return board->possibleMoves;
}

void boardSetFieldState(Board* const board, const int x, const int y, const FieldState state) {
    board->fields[x + y * board->fieldDimension] = state;
    calculatePossibleMoves(board);
}

FieldState boardGetFieldState(Board const* const board, const int x, const int y) {
    return board->fields[x + y * board->fieldDimension];
}

bool boardContainsPossibleMove(Board const* const board, const Move move) {
    return moveListContains(board->possibleMoves, move);
}

bool boardHasPossibleMoves(Board const* const board) {
    return !moveListIsEmpty(board->possibleMoves);
}

bool boardIsSelectionActive(Board const* const board) {
    return board->selectedBall.x >= 0 && board->selectedBall.y >= 0;
}

bool boardBallCanJump(Board const* const board, const Vector2Int from, const Vector2Int to) {
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

int boardGetNumberOfBalls(const Board* board) {
    int sum = 0;
    for (int x = 0; x < BOARD_DIMENSION; ++x) {
        for (int y = 0; y < BOARD_DIMENSION; ++y) {
            if (boardGetFieldState(board, x, y) == FIELDSTATE_OCCUPIED) {
                sum++;
            }
        }
    }
    return sum;
}

void boardDoJump(Board* const board, const Vector2Int from, const Vector2Int to) {
    assert(boardBallCanJump(board, from, to));
    const Vector2Int inBetweenPosition = getMiddle(from, to);
    boardSetFieldState(board, from.x, from.y, FIELDSTATE_FREE);
    boardSetFieldState(board, inBetweenPosition.x, inBetweenPosition.y, FIELDSTATE_FREE);
    boardSetFieldState(board, to.x, to.y, FIELDSTATE_OCCUPIED);

    // put ball into outer ring
    Vector2 position;
    float minDistance = FLT_MAX;
    do {
        position = randomPointOnCircle(OUTER_RING_RADIUS);
        BallListNode* current = board->depositedBalls;
        minDistance = FLT_MAX;
        while (current != NULL) {
            const float distance = vectorDistance(position, current->position);
            if (distance < minDistance) {
                minDistance = distance;
            }
            current = current->next;
        }
    } while (minDistance != FLT_MAX &&
             (minDistance < OUTER_RING_BALL_MIN_DISTANCE || minDistance > OUTER_RING_BALL_MAX_DISTANCE));
    ballListNodeAdd(&board->depositedBalls, position);
}

BallListNode* boardGetDepositedBalls(Board const* const board) {
    return board->depositedBalls;
}

void boardDestroy(Board** board) {
    ballListDestroy(&(*board)->depositedBalls);
    moveListDestroy(&(*board)->possibleMoves);
    free((*board)->fields);
    free(*board);
    *board = NULL;
}
