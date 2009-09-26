#ifndef LINKED_LIST_PRIVATE_
#define LINKED_LIST_PRIVATE_

#include "linked_list.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/* Basic data structure for a node. */
typedef struct _ll_node
{
    void* node_data;
    struct _ll_node* next;
} Node;


/* Linked list. */
struct _ll_list
{
    /* Head of the linked list. */
    Node *head;
    Node *tail;

    /* Functions for manipulating list entries. */
    int16_t (*compare)(void *,void *);
    void* (*deep_copy) (void*);
    void (*free_data) (void*);
};

List* merge_sorted(List *list_a, List *list_b);

#endif /*LINKED_LIST_PRIVATE_ */
