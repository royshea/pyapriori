#ifndef LINKED_LIST_
#define LINKED_LIST_

#include <stdint.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif


struct ll_list
{
    struct ll_node* head;
    struct ll_node* tail;
    uint8_t data_size;
};

struct ll_node
{
    void* data;
    struct ll_node* next;
};

/* Create a new linked list that stores data of size data_size. */
struct ll_list* ll_init(size_t data_size);

/* Add element data to the linked list ll. */
void ll_append(struct ll_list *ll, void* data);

/* Pop an element from the head of the linked list. */
void* ll_pop(struct ll_list *ll);

/* Free the linke list. */
void ll_free(struct ll_list *ll);

/* Count the number of nodes in a linked list. */
int ll_length(struct ll_list *ll);

/* Create a copy of the linked list. */
struct ll_list* ll_copy(struct ll_list* ll);

struct ll_node *mergesort(struct ll_node *head,int (*cmp)(void *,void *));

struct ll_node *merge(struct ll_node *head_one, struct ll_node *head_two,int (*cmp)(void *,void *));

int is_subset(struct ll_node *l_1, struct ll_node *l_2, int (*cmp)(void *,void *));

/* Functions that are used in apriori, but I'm not sure why... */
/*
struct ll_node *get_node(struct ll_node *n, int index);
*/

#endif /*LINKED_LIST_*/
