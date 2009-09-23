#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "linked_list.h"


#if UNIT_TESTING

/* Redefine assert for use with cmockery when unit testing. */
extern void mock_assert(const int result, const char* const expression,
        const char * const file, const int line);
#undef assert
#define assert(expression) \
        mock_assert((int)(expression), #expression, __FILE__, __LINE__);

/* Redefine malloc operations for use with cmockery when unit testing. */

extern void* _test_malloc(const size_t size, const char* file, const int
        line);
extern void* _test_calloc(const size_t number_of_elements, const size_t
        size, const char* file, const int line);
extern void _test_free(void* const ptr, const char* file, const int
        line);

#define malloc(size) _test_malloc(size, __FILE__, __LINE__)
#define calloc(num, size) _test_calloc(num, size, __FILE__, __LINE__)
#define free(ptr) _test_free(ptr, __FILE__, __LINE__)

#endif /* UNIT_TESTING */


struct ll_list* ll_init(size_t data_size)
{
    struct ll_list *ll;

    /* Create an empty linked list that knows the size of data it
     * stores. */
    ll = malloc(sizeof(struct ll_list));
    assert(ll != NULL);
    ll->head = NULL;
    ll->tail = NULL;
    ll->data_size = data_size;

    return ll;
}


void ll_append(struct ll_list *ll, void* data)
{
    struct ll_node* new_tail;

    assert(ll != NULL);
    assert((ll->head != NULL && ll->tail != NULL) ||
            (ll->head == NULL && ll->tail == NULL));

    /* Create new list node. */
    new_tail = malloc(sizeof(struct ll_node));
    assert(new_tail != NULL);

    new_tail->data = malloc(ll->data_size);
    assert(new_tail->data != NULL);

    memcpy(new_tail->data, data, ll->data_size);
    new_tail->next = NULL;

    if (ll->head == NULL && ll->tail == NULL)
    {
        /* If this is the first node in the list then the head needs to
         * be updated to also point to this node. */
        ll->head = new_tail;
        ll->tail = new_tail;
    }
    else
    {
        /* Else append the node to the linked list. */
        ll->tail->next = new_tail;
        ll->tail = new_tail;
    }

    return;
}


void* ll_pop(struct ll_list *ll)
{
    void* tmp_data;
    struct ll_node* old_head;

    assert(ll != NULL);
    assert(ll->head != NULL);

    /* Update the head of the list. */
    old_head = ll->head;
    ll->head = ll->head->next;

    /* Grab data from old head and release the node. */
    tmp_data = old_head->data;
    free(old_head);

    return tmp_data;
}


void ll_free(struct ll_list* ll)
{

    struct ll_node *tmp_node;

    assert(ll != NULL);

    /* Traverse the list freeing nodes and data. */
    while (ll->head != NULL)
    {
        tmp_node = ll->head;
        ll->head = ll->head->next;
        free(tmp_node->data);
        free(tmp_node);
    }

    /* Free the linked list container. */
    free(ll);

    return;
}


int ll_length(struct ll_list* ll)
{
    uint16_t length = 0;
    struct ll_node *tmp_node;

    assert(ll != NULL);

    /* Iterate through and count nodes. */
    for (tmp_node = ll->head; tmp_node != NULL; tmp_node = tmp_node->next)
    {
        length += 1;
    }
    return length;
}


struct ll_list* ll_copy(struct ll_list* ll)
{
    struct ll_list *copy;
    struct ll_node *tmp_node;

    /* Create new container for the copied list. */
    copy = ll_init(ll->data_size);

    /* Iterate through list making copies of nodes. */
    tmp_node = ll->head;
    for (tmp_node = ll->head; tmp_node != NULL; tmp_node = tmp_node->next)
    {
        ll_append(copy, tmp_node->data);
    }

    return copy;
}


#if 0
/* merge the lists.. */
static struct ll_list *ll_merge(struct ll_list* ll_1, struct ll_list*
        ll_2, int (*cmp)(void *, void *))
{
    struct ll_list *merged;

    /* TODO: Convert recursion into a loop. */
    if (head_one == NULL)
        return head_two;

    if (head_two == NULL)
        return head_one;

    if ((*cmp)(head_one->data, head_two->data) < 0)
    {
        head_three = head_one;
        head_three->next = merge(head_one->next, head_two, cmp);
    }
    else
    {
        head_three = head_two;
        head_three->next = merge(head_one, head_two->next, cmp);
    }

    return head_three;
}


struct ll_node *mergesort(struct ll_node *head, int (*cmp)(void *, void *))
{
    struct ll_node *head_one;
    struct ll_node *head_two;

    if ((head == NULL) || (head->next == NULL))
        return head;

    head_one = head;
    head_two = head->next;
    while ((head_two != NULL) && (head_two->next != NULL))
    {
        head = head->next;
        head_two = head->next->next;
    }
    head_two = head->next;
    head->next = NULL;

    return merge(mergesort(head_one, cmp), mergesort(head_two, cmp), cmp);
}


/* TODO / BUG: This assumes that data is sorted. */
int is_subset(struct ll_node *l_1, struct ll_node *l_2, int (*cmp)(void *, void *))
    /* Returns if l_2 is a subset of l_1 */
{
    unsigned char inside;
    struct ll_node *iter_1;
    struct ll_node *iter_2 = l_2;

    if (l_1 == NULL || l_2 == NULL)
        return FALSE;

    if (ll_length(l_1) < ll_length(l_2))
        return FALSE;

    while (iter_2 != NULL)
    {
        iter_1 = l_1;
        inside = FALSE;
        while (iter_1 != NULL)
        {
            if ((*cmp)(iter_1->data, iter_2->data) == 0)
                inside = TRUE;
            iter_1 = iter_1->next;
        }
        if (inside == FALSE)
            return FALSE;
        iter_2 = iter_2->next;
    }

    return TRUE;
}
#endif

/*
struct ll_node *get_node(struct ll_node *n, int index)
{
    int count = 0;
    while (n != NULL)
    {
        if (count == index)
        {
            return n;
        }
        count++;
        n = n->next;
    }

    return NULL;
}
*/
