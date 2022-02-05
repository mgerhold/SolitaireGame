#ifndef UTILITY_H
#define UTILITY_H

#include "board_fwd.h"
#include "vector2int.h"
#include "raylib.h"

Vector2Int getMiddle(Vector2Int lhs, Vector2Int rhs);
Vector2 ballCoordsToScreenCoords(Vector2Int coords);
Vector2Int screenCoordsToBallCoords(Board const * const board, Vector2 coords);
float vectorDistance(Vector2 lhs, Vector2 rhs);
Vector2 randomPointOnCircle(float radius);

#endif
