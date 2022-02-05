#include "raylib.h"
#include "vector2int.h"
#include "movelist.h"
#include "board.h"
#include "utility.h"
#include "defines.h"
#include "balllist.h"
#include "debugtext.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <time.h>

char debugText[128] = "";

static void updateDrawFrame(void);
static void handleInput(void);

static Board* board;
static Camera2D camera = { .offset = { (float) SCREEN_WIDTH / 2.0f, (float) SCREEN_HEIGHT / 2.0f },
                           .target = { 0.0f, 0.0f },
                           .rotation = 0.0f,
                           .zoom = 1.0f };

int main() {    
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Solitaire");
    board = boardCreate();

    SetTargetFPS(200);

    srand((unsigned int)time(NULL) % 100);
    while (!WindowShouldClose()) {
        handleInput();
        updateDrawFrame();
    }

    boardDestroy(&board);
    CloseWindow();
    return 0;
}

static void updateDrawFrame(void) {
    BeginDrawing();
    {

        ClearBackground(BLACK);

        BeginMode2D(camera);
        {
            DrawCircle(0, 0, BOARD_RADIUS, BOARD_COLOR); // board
            for (int x = 0; x < BOARD_DIMENSION; ++x) {
                for (int y = 0; y < BOARD_DIMENSION; ++y) {
                    FieldState fieldState = boardGetFieldState(board, x, y);
                    if (fieldState == FIELDSTATE_BLOCKED) {
                        continue;
                    }
                    Color fieldColor = EMPTY_FIELD_COLOR;
                    const Vector2Int selectedBall = boardGetSelectedBall(board);
                    if (boardIsSelectionActive(board) && x == selectedBall.x && y == selectedBall.y) {
                        fieldColor = SELECTED_BALL_COLOR;
                    } else if (fieldState == FIELDSTATE_FREE && boardIsSelectionActive(board) &&
                               boardContainsPossibleMove(board,
                                                         (Move){ .from = selectedBall, .to = (Vector2Int){ x, y } })) {
                        fieldColor = POSSIBLE_MOVE_TARGET_COLOR;
                    } else if (fieldState == FIELDSTATE_OCCUPIED) {
                        fieldColor = BALL_COLOR;
                    }
                    DrawCircle((int) (-INNER_FIELD_HALF_WIDTH + (float) x * FIELD_MARGIN),
                               (int) (-INNER_FIELD_HALF_WIDTH + (float) y * FIELD_MARGIN), FIELD_RADIUS, fieldColor);
                }
            }

            const BallListNode* currentBall = boardGetDepositedBalls(board);
            while (currentBall != NULL) {
                DrawCircle((int)currentBall->position.x, (int)currentBall->position.y, FIELD_RADIUS, BALL_COLOR);
                currentBall = currentBall->next;
            }
        }

        EndMode2D();

        DrawText(debugText, 10, 50, 30, RED);
        DrawFPS(10, 10);
    }
    EndDrawing();
}

static void handleInput(void) {
    const Vector2 mousePosition = GetScreenToWorld2D(GetMousePosition(), camera);
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        const Vector2Int ballCoords = screenCoordsToBallCoords(board, mousePosition);
        if (!boardAreCoordsValid(board, ballCoords) ||
            boardGetFieldState(board, ballCoords.x, ballCoords.y) != FIELDSTATE_OCCUPIED) {
            boardClearSelection(board);
            return;
        }
        boardSetSelectedBall(board, ballCoords);
    } else if (boardIsSelectionActive(board) && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        const Vector2Int targetCoords = screenCoordsToBallCoords(board, mousePosition);
        if (!boardAreCoordsValid(board, targetCoords)) {
            return;
        }
        if (!boardBallCanJump(board, boardGetSelectedBall(board), targetCoords)) {
            return;
        }
        const Vector2Int inBetweenPosition = getMiddle(boardGetSelectedBall(board), targetCoords);
        const Vector2Int selectedBall = boardGetSelectedBall(board);
        assert(boardGetFieldState(board, selectedBall.x, selectedBall.y) == FIELDSTATE_OCCUPIED);
        assert(boardGetFieldState(board, inBetweenPosition.x, inBetweenPosition.y) == FIELDSTATE_OCCUPIED);
        assert(boardGetFieldState(board, targetCoords.x, targetCoords.y) == FIELDSTATE_FREE);
        boardDoJump(board, selectedBall, targetCoords);
        boardClearSelection(board);
        // set selection to moved ball if there's another move possible from the new position
        bool anotherMoveIsPossible = false;
        const MoveList* possibleMoves = boardGetPossibleMoves(board);
        const size_t listLength = moveListSize(possibleMoves);
        for (size_t i = 0; i < listLength; ++i) {
            const Move move = moveListGet(possibleMoves, i);
            if (move.from.x == targetCoords.x && move.from.y == targetCoords.y) {
                anotherMoveIsPossible = true;
                break;
            }
        }
        if (anotherMoveIsPossible) {
            boardSetSelectedBall(board, targetCoords);
        }

        if (!boardHasPossibleMoves(board)) {
            sprintf(debugText, "Game Over! Score: %d", SCORE_CAP - boardGetNumberOfBalls(board));
        }
    }
}
