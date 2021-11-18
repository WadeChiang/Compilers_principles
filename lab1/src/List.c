#include "List.h"
#include "HashMap.h"
#include <stdlib.h>
#include <string.h>

const int K = 1000;

struct List* ListInit() {
    struct List *list = malloc(sizeof(struct List));
    list->head = malloc(sizeof(struct ListNode) * K);
    list->max_len = K;
    list->curr_len = 0;
}

int ListPush(struct List* l, char* id) {
    int index = isInList(l, id);
    if (index != -1)
    {
        return index;
    }
    if (l->curr_len == l->max_len)
    {
        ResizeList(l);
    }

    l->head[l->curr_len].id = id;
    l->head[l->curr_len].number = l->curr_len;
    l->head[l->curr_len].hash_val = getHashValue(id);
    return l->curr_len++;
}

int isInList(struct List *l, char *id) {
    unsigned int value = getHashValue(id);
    for (unsigned int i = 0; i < l->curr_len; i++)
    {
        if ((l->head[i].hash_val == value) && strcmp(l->head[i].id, id) == 0)
        {
            return l->head[i].number;
        }
    }
    return -1;
}

void ResizeList(struct List *l) {
    struct ListNode *new_head = malloc(l->max_len + K);
    for (size_t i = 0; i < l->max_len; i++)
    {
        new_head[i].id = l->head[i].id;
        new_head[i].hash_val = l->head[i].hash_val;
        new_head[i].number = l->head[i].number;
        l->head[i].id = NULL;
    }
    l->max_len += K;
    free(l->head);
    l->head = new_head;
    return;
}