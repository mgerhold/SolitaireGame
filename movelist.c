#include "movelist.h"
#include <stdlib.h>
#include <assert.h>

struct MoveList {
    Move* data;
    size_t size, capacity;
};

bool moveEquals(Move lhs, Move rhs) {
    return lhs.from.x == rhs.from.x && lhs.from.y == rhs.from.y && lhs.to.x == rhs.to.x && lhs.to.y == rhs.to.y;
}

static void grow(MoveList* list) {
    const bool initialGrow = list->data == NULL;
    if (initialGrow) {
        list->data = malloc(sizeof(*list->data));
        list->capacity = 1;
    } else {
        const size_t newCapacity = list->capacity * 2;
        list->data = realloc(list->data, sizeof(*list->data) * newCapacity);
        list->capacity = newCapacity;
    }
}

MoveList* moveListCreate() {
    MoveList* list = malloc(sizeof(*list));
    list->data = NULL;
    list->capacity = 0;
    list->size = 0;
    return list;
}

void moveListAdd(MoveList* list, Move move) {
    const bool isBigEnough = list->capacity >= list->size + 1;
    if (!isBigEnough) {
        grow(list);
    }
    list->data[list->size] = move;
    list->size++;
}

void moveListClear(MoveList* list) {
    list->size = 0;
}

size_t moveListSize(const MoveList* list) {
    return list->size;
}

Move moveListGet(const MoveList* list, size_t index) {
    assert(index < list->size);
    return list->data[index];
}

void moveListDestroy(MoveList** list) {
    free((*list)->data);
    free(*list);
    *list = NULL;
}

bool moveListIsEmpty(MoveList* list) {
    return list->size == 0;
}

bool moveListContains(MoveList* list, Move move) {
    for (size_t i = 0; i < list->size; ++i) {
        if (moveEquals(list->data[i], move)) {
            return true;
        }
    }
    return false;
}