#ifndef LINKED_LIST_PRIVATE_
#define LINKED_LIST_PRIVATE_

#include "linked_list.h"

/* Basic data structure for a node. */
struct _ll_node
{
    void* data;
    struct _ll_node* next;
};

#endif /*LINKED_LIST_PRIVATE_ */

