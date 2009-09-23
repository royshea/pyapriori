#ifndef LINKED_LIST_
#define LINKED_LIST_

#include <stdint.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif


typedef struct _ll_node
{
    void* data;
    struct _ll_node* next;
} Node;


/* Add element data to the head of the linked list. */
void ll_push(Node **head, void *data);

/* Pop an element from the head of the linked list. */
void* ll_pop(Node **head);

/* Count the number of nodes in a linked list. */
uint16_t ll_length(Node *head);

/* Create a (deep) copy of the linked list. */
void* ll_copy(Node *head, void*(*deep_copy)(void*));

/* Free the linke list. */
void ll_free(Node **head, void(*free_data)(void*));

/* Sort a linked list using the comparison function defined by cmp. */
void ll_sort(Node **head, int16_t(*compare)(void*, void*));

/* TODO */
uint16_t ll_is_subset(Node head_a, Node head_b, uint16_t(*compare)(void *,void *));

/* Functions that are used in apriori, but I'm not sure why... */
/*
struct ll_node *get_node(struct ll_node *n, int index);
*/

#endif /*LINKED_LIST_*/
