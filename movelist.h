#ifndef MOVELIST_H
#define MOVELIST_H

#include "vector2int.h"
#include <stddef.h>

typedef struct {
    Vector2Int from, to;
} Move;

typedef struct {
    Move* data;
    size_t size, capacity;
} MoveList;

void moveListCreate(MoveList* list);
void moveListAdd(MoveList* list, Move move);
void moveListClear(MoveList* list);
size_t moveListSize(MoveList* list);
Move* moveListGet(MoveList* list, size_t index);
void moveListDestroy(MoveList* list);

#endif