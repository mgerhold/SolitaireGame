#include "utility.h"
#include "defines.h"
#include "board.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

Vector2Int getMiddle(Vector2Int lhs, Vector2Int rhs) {
    return (Vector2Int){ (lhs.x + rhs.x) / 2, (lhs.y + rhs.y) / 2 };
}

Vector2 ballCoordsToScreenCoords(Vector2Int coords) {
    return (Vector2){ -INNER_FIELD_HALF_WIDTH + (float) coords.x * FIELD_MARGIN,
                      -INNER_FIELD_HALF_WIDTH + (float) coords.y * FIELD_MARGIN };
}

Vector2Int screenCoordsToBallCoords(Board const * const board, Vector2 coords) {
    for (int x = 0; x < BOARD_DIMENSION; ++x) {
        for (int y = 0; y < BOARD_DIMENSION; ++y) {
            if (!boardAreCoordsValid(board, (Vector2Int){ x, y })) {
                continue;
            }
            Vector2 ballPosition = ballCoordsToScreenCoords((Vector2Int){ x, y });
            const float squaredDistance = (coords.x - ballPosition.x) * (coords.x - ballPosition.x) +
                                          (coords.y - ballPosition.y) * (coords.y - ballPosition.y);
            if (squaredDistance <= FIELD_RADIUS * FIELD_RADIUS) {
                return (Vector2Int){ x, y };
            }
        }
    }
    return (Vector2Int){ -1, -1 };
}

float vectorDistance(Vector2 lhs, Vector2 rhs) {
    return sqrtf((lhs.x - rhs.x) * (lhs.x - rhs.x) + (lhs.y - rhs.y) * (lhs.y - rhs.y));
}

Vector2 randomPointOnCircle(float radius) {
    const float randomValue = (float)rand();
    const float angle = randomValue / (float)RAND_MAX * 2.0f * (float)M_PI;
    return (Vector2){ .x = cosf(angle) * radius, .y = sinf(angle) * radius };
}
