#include "movelist.h"
#include <stdlib.h>
#include <stdbool.h>

static void grow(MoveList* list) {
    if (list->data == NULL) {
        list->data = malloc(sizeof(Move));
        list->capacity = 1;
    } else {
        const size_t newCapacity = list->capacity * 2;
        list->data = realloc(list->data, sizeof(Move) * newCapacity);
        list->capacity = newCapacity;
    }
}

void moveListCreate(MoveList* list) {
    list->data = NULL;
    list->capacity = 0;
    list->size = 0;
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

size_t moveListSize(MoveList* list) {
    return list->size;
}

void moveListDestroy(MoveList* list) {
    free(list->data);
    moveListCreate(list);
}