#include "balllist.h"
#include <stdlib.h>
#include <assert.h>

void ballListNodeAdd(BallListNode** head, Vector2 position) {
    assert(head != NULL);
    BallListNode* oldHead = *head;
    BallListNode* newHead = malloc(sizeof(**head));
    if (newHead != NULL) {
        *head = newHead;
        (*head)->position = position;
        (*head)->next = oldHead;
    }
}

bool ballListNodeHasNext(const BallListNode* node) {
    assert(node != NULL);
    return node->next != NULL;
}

BallListNode* ballListNodeGetNext(const BallListNode* node) {
    assert(node != NULL);
    return node->next;
}

size_t ballListGetLength(const BallListNode* head) {
    if (head == NULL) {
        return 0;
    }
    size_t length = 1;
    const BallListNode* current = head;
    while (ballListNodeHasNext(current)) {
        length++;
        current = ballListNodeGetNext(current);
    }
    return length;
}

void ballListDestroy(BallListNode** head) {
    assert(head != NULL);
    BallListNode* current = *head;
    while (current != NULL) {
        BallListNode* next = current->next;
        free(current);
        current = next;
    }
    *head = NULL;
}
