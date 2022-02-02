#include "utility.h"

Vector2Int getMiddle(Vector2Int a, Vector2Int b) {
    return (Vector2Int){ (a.x + b.x) / 2, (a.y + b.y) / 2 };
}