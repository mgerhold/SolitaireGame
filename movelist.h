#ifndef MOVELIST_H
#define MOVELIST_H

#include "vector2int.h"
#include <stddef.h>
#include <stdbool.h>

typedef struct {
    Vector2Int from, to;
} Move;

struct MoveList;
typedef struct MoveList MoveList;

bool moveEquals(Move lhs, Move rhs);

MoveList* moveListCreate();
void moveListAdd(MoveList* list, Move move);
void moveListClear(MoveList* list);
size_t moveListSize(const MoveList* list);
Move moveListGet(const MoveList* list, size_t index);
void moveListDestroy(MoveList** list);
bool moveListIsEmpty(MoveList* list);
bool moveListContains(MoveList* list, Move move);

#endif