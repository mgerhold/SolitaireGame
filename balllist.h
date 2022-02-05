#ifndef BALLLIST_H
#define BALLLIST_H

#include "raylib.h"
#include <stddef.h>
#include <stdbool.h>

typedef struct BallListNode {
    Vector2 position;
    struct BallListNode* next;
} BallListNode;

void ballListNodeAdd(BallListNode** head, Vector2 position);
bool ballListNodeHasNext(const BallListNode* node);
BallListNode* ballListNodeGetNext(const BallListNode* node);
size_t ballListGetLength(const BallListNode* head);
void ballListDestroy(BallListNode** head);

#endif