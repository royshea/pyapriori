#ifndef LINKED_LIST_
#define LINKED_LIST_

#include <stdint.h>

/* Abstract linked list node. */
typedef struct _ll_list List;

/* Create a new list. */
List* ll_create(int16_t(*data_compare)(void *,void *),
        void*(*data_copy) (void*), void(*data_data) (void*));

/* Add element data to the head of the linked list. */
void ll_push(List *list, void *data);

/* Add element data to the tail of the linked list. */
void ll_push_tail(List *list, void *data);

/* Pop an element from the head of the linked list. */
void* ll_pop(List *list);

/* Count the number of nodes in a linked list. */
uint16_t ll_length(List *list);

/* Create a (deep) copy of the linked list. */
void* ll_copy(List *list);

/* Free the linked list. */
void ll_free(List *list);

/* Sort a linked list using the comparison function defined by compare. */
void ll_sort(List *list);

/* Search for data in a list and, if found, return its reference. */
void* ll_search(List *list, void* data);

/* Return reference to the nth element in the list (starting with zero). */
void* ll_get_nth(List *list, uint8_t index);

/* Search for data in a list and, if found, remove from the list and
 * return its reference. */
void* ll_remove(List *list, void* data);

/* Check if subset is contained within the list head. */
uint8_t ll_is_subset_of(List *list, List *subset);

#endif /*LINKED_LIST_*/
