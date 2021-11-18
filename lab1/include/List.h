struct ListNode {
    char *id;
    int number;
    int hash_val;
};

struct List {
    struct ListNode *head;
    int max_len;
    int curr_len;
};

struct List *ListInit();
int ListPush(struct List *l, char *id);
int isInList(struct List *l, char *id);
void ResizeList(struct List *l);
