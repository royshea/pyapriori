#ifndef LINKED_LIST_
#define LINKED_LIST_

#include <stdint.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/* Abstract linked list node. */
typedef struct _ll_node Node;

/* Add element data to the head of the linked list. */
void ll_push(Node **head, void *data);

/* Pop an element from the head of the linked list. */
void* ll_pop(Node **head);

/* Count the number of nodes in a linked list. */
uint16_t ll_length(Node *head);

/* Create a (deep) copy of the linked list. */
void* ll_copy(Node *head, void*(*deep_copy)(void*));

/* Free the linked list. */
void ll_free(Node **head, void(*free_data)(void*));

/* Sort a linked list using the comparison function defined by compare. */
void ll_sort(Node **head, int16_t(*compare)(void*, void*));

/* Search for data in a list and, if found, return its reference. */
void* ll_search(Node *head, void* data, int16_t(*compare)(void *,void *));

/* Return reference to the nth element in the list (starting with zero). */
void* ll_get_nth(Node *head, uint8_t index);

/* Search for data in a list and, if found, remove from the list and
 * return its reference. */
void* ll_remove(Node **head, void* data, int16_t(*compare)(void *,void *));

/* Check if subset is contained within the list head. */
uint8_t ll_is_subset_of(Node *head, Node *subset,
        int16_t(*compare)(void *,void *));

#endif /*LINKED_LIST_*/
