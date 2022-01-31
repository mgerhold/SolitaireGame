#include "raylib.h"
#include "vector2int.h"
#include "movelist.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

typedef enum {
    FIELDSTATE_BLOCKED, // no ball can ever reach this field
    FIELDSTATE_FREE,    // this is a field that can be reached
    FIELDSTATE_OCCUPIED,// this is a field that can be reached with a ball on it
} FieldState;

#define FIELD_DIMENSION 7

static Camera2D camera = { 0 };
static Vector2 cameraTarget = { .x = 0.0f, .y = 0.0f };
static const int screenWidth = 800;
static const int screenHeight = 600;
static const float boardRadius = (float) screenHeight * 0.45f;
static const float innerFieldHalfWidth = boardRadius * 0.8f;
static const float fieldMargin = 2.0f * innerFieldHalfWidth / (FIELD_DIMENSION - 1);// distance between fields
static const float fieldRadius = boardRadius / FIELD_DIMENSION / 2.0f;
static const Color boardColor = { 135, 62, 35, 255 };
static const Color ballColor = WHITE;
static const Color emptyFieldColor = { 33, 19, 13, 255 };
static const Color selectedBallColor = RED;
static const Color possibleMoveTargetColor = YELLOW;

static FieldState fields[FIELD_DIMENSION * FIELD_DIMENSION];
static Vector2Int selectedBall = { -1, -1 };
static char debugText[128] = "";
static MoveList possibleMoves;

static bool areCoordsValid(Vector2Int coords);
static void initFields(void);
static void updateDrawFrame(void);
static void setFieldState(int x, int y, FieldState state);
static FieldState getFieldState(int x, int y);
static bool isSelectionActive(void);
static void handleInput(void);
static Vector2Int getMiddle(Vector2Int a, Vector2Int b);
static Vector2 ballCoordsToScreenCoords(Vector2Int coords);
static Vector2Int screenCoordsToBallCoords(Vector2 coords);
static bool canJumpTo(Vector2Int from, Vector2Int to);
static void clearSelection(void);
static void addMoveIfValid(Vector2Int from, Vector2Int to);
static void calculatePossibleMoves(void);

int main() {
    InitWindow(screenWidth, screenHeight, "Solitaire");
    initFields();
    moveListCreate(&possibleMoves);
    calculatePossibleMoves();

    camera = (Camera2D){ 0 };
    camera.target = cameraTarget;
    camera.offset = (Vector2){ (float) screenWidth / 2.0f, (float) screenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        handleInput();
        updateDrawFrame();
    }

    moveListDestroy(&possibleMoves);
    CloseWindow();
    return 0;
}

static void updateDrawFrame(void) {
    BeginDrawing();
    {

        ClearBackground(BLACK);

        BeginMode2D(camera);
        {
            DrawCircle(0, 0, boardRadius, boardColor);
            for (int x = 0; x < FIELD_DIMENSION; ++x) {
                for (int y = 0; y < FIELD_DIMENSION; ++y) {
                    FieldState fieldState = getFieldState(x, y);
                    if (fieldState == FIELDSTATE_BLOCKED) {
                        continue;
                    }
                    Color fieldColor = emptyFieldColor;
                    if (isSelectionActive() && (int) x == selectedBall.x && (int) y == selectedBall.y) {
                        fieldColor = selectedBallColor;
                    } else if (isSelectionActive() && canJumpTo(selectedBall, (Vector2Int){ (int) x, (int) y })) {
                        sprintf(debugText, "can jump");
                        fieldColor = possibleMoveTargetColor;
                    } else if (fieldState == FIELDSTATE_OCCUPIED) {
                        fieldColor = ballColor;
                    }
                    DrawCircle((int) (-innerFieldHalfWidth + (float) x * fieldMargin),
                               (int) (-innerFieldHalfWidth + (float) y * fieldMargin), fieldRadius, fieldColor);
                }
            }
        }

        EndMode2D();

        DrawText(debugText, 10, 50, 30, WHITE);
        DrawFPS(10, 10);
    }
    EndDrawing();
}

static bool areCoordsValid(Vector2Int coords) {
    return (coords.x >= 0 && coords.y >= 0 && coords.x < FIELD_DIMENSION && coords.y < FIELD_DIMENSION) &&
           ((coords.x > FIELD_DIMENSION / 4 && coords.x < FIELD_DIMENSION * 3 / 4) ||
            (coords.y > FIELD_DIMENSION / 4 && coords.y < FIELD_DIMENSION * 3 / 4));
}

static void initFields(void) {
    for (int x = 0; x < FIELD_DIMENSION; ++x) {
        for (int y = 0; y < FIELD_DIMENSION; ++y) {
            FieldState state = FIELDSTATE_BLOCKED;
            if (x == FIELD_DIMENSION / 2 && y == FIELD_DIMENSION / 2) {
                // middle field
                state = FIELDSTATE_FREE;
            } else if (areCoordsValid((Vector2Int){ x, y })) {
                state = FIELDSTATE_OCCUPIED;
            }
            setFieldState(x, y, state);
        }
    }
}

static void setFieldState(int x, int y, FieldState state) {
    fields[x + y * FIELD_DIMENSION] = state;
}

static FieldState getFieldState(int x, int y) {
    return fields[x + y * FIELD_DIMENSION];
}

static bool isSelectionActive(void) {
    return selectedBall.x >= 0 && selectedBall.y >= 0;
}

static void handleInput(void) {
    const Vector2 mousePosition = GetScreenToWorld2D(GetMousePosition(), camera);
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        const Vector2Int ballCoords = screenCoordsToBallCoords(mousePosition);
        if (!areCoordsValid(ballCoords)) {
            clearSelection();
            return;
        }
        selectedBall = ballCoords;
    } else if (isSelectionActive() && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        const Vector2Int targetCoords = screenCoordsToBallCoords(mousePosition);
        if (!areCoordsValid(targetCoords)) {
            return;
        }
        if (!canJumpTo(selectedBall, targetCoords)) {
            return;
        }
        const Vector2Int inBetweenPosition = getMiddle(selectedBall, targetCoords);
        assert(getFieldState(selectedBall.x, selectedBall.y) == FIELDSTATE_OCCUPIED);
        assert(getFieldState(inBetweenPosition.x, inBetweenPosition.y) == FIELDSTATE_OCCUPIED);
        assert(getFieldState(targetCoords.x, targetCoords.y) == FIELDSTATE_FREE);
        setFieldState(selectedBall.x, selectedBall.y, FIELDSTATE_FREE);
        setFieldState(inBetweenPosition.x, inBetweenPosition.y, FIELDSTATE_FREE);
        setFieldState(targetCoords.x, targetCoords.y, FIELDSTATE_OCCUPIED);
        clearSelection();
    }
}

static Vector2Int getMiddle(Vector2Int a, Vector2Int b) {
    return (Vector2Int){ (a.x + b.x) / 2, (a.y + b.y) / 2 };
}

static Vector2 ballCoordsToScreenCoords(Vector2Int coords) {
    return (Vector2){ -innerFieldHalfWidth + (float) coords.x * fieldMargin,
                      -innerFieldHalfWidth + (float) coords.y * fieldMargin };
}

static Vector2Int screenCoordsToBallCoords(Vector2 coords) {
    for (int x = 0; x < FIELD_DIMENSION; ++x) {
        for (int y = 0; y < FIELD_DIMENSION; ++y) {
            if (!areCoordsValid((Vector2Int){ x, y })) {
                continue;
            }
            Vector2 ballPosition = ballCoordsToScreenCoords((Vector2Int){ x, y });
            const float squaredDistance = (coords.x - ballPosition.x) * (coords.x - ballPosition.x) +
                                          (coords.y - ballPosition.y) * (coords.y - ballPosition.y);
            if (squaredDistance <= fieldRadius * fieldRadius) {
                return (Vector2Int){ x, y };
            }
        }
    }
    return (Vector2Int){ -1, -1 };
}

static bool canJumpTo(Vector2Int from, Vector2Int to) {
    if (!areCoordsValid(from) || !areCoordsValid(to)) {
        return false;
    }
    const Vector2Int direction = { to.x - from.x, to.y - from.y };
    const int manhattanDistance = abs(direction.x) + abs(direction.y);
    if ((direction.x != 0 && direction.y != 0) || manhattanDistance != 2) {
        return false;
    }
    if (getFieldState(from.x, from.y) != FIELDSTATE_OCCUPIED) {
        return false;
    }
    if (getFieldState(to.x, to.y) != FIELDSTATE_FREE) {
        return false;
    }
    const Vector2Int inBetweenPosition = getMiddle(from, to);
    if (getFieldState(inBetweenPosition.x, inBetweenPosition.y) != FIELDSTATE_OCCUPIED) {
        return false;
    }
    return true;
}

static void clearSelection(void) {
    selectedBall.x = -1;
    selectedBall.y = -1;
}

static void addMoveIfValid(Vector2Int from, Vector2Int to) {
    if (canJumpTo(from, to)) {
        moveListAdd(&possibleMoves, (Move){ from, to });
    }
}

static void calculatePossibleMoves(void) {
    moveListClear(&possibleMoves);
    for (int x = 0; x < FIELD_DIMENSION; ++x) {
        for (int y = 0; y < FIELD_DIMENSION; ++y) {
            if (!areCoordsValid((Vector2Int){ x, y })) {
                continue;
            }
            const Vector2Int currentPosition = { x, y };
            addMoveIfValid(currentPosition, (Vector2Int){ currentPosition.x + 2, currentPosition.y });
            addMoveIfValid(currentPosition, (Vector2Int){ currentPosition.x - 2, currentPosition.y });
            addMoveIfValid(currentPosition, (Vector2Int){ currentPosition.x, currentPosition.y + 2 });
            addMoveIfValid(currentPosition, (Vector2Int){ currentPosition.x, currentPosition.y - 2 });
        }
    }
    sprintf(debugText, "Possible moves: %zu", moveListSize(&possibleMoves));
}