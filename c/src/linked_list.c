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


void ll_push(Node **head, void *data)
{
    Node *new_head;
    new_head = malloc(sizeof(Node));
    assert(new_head != NULL);

    new_head->data = data;
    new_head->next = *head;
    *head = new_head;
}


void* ll_pop(Node **head)
{
    void *data;
    Node *old_head;

    assert(*head != NULL);

    old_head = *head;
    *head = (*head)->next;

    data = old_head->data;
    free(old_head);

    return data;
}

uint16_t ll_length(Node *head)
{
    uint16_t count;

    count = 0;
    for (; head != NULL; head = head->next)
    {
        count += 1;
    }

    return count;
}

void* ll_copy(Node *head, void*(*deep_copy)(void*))
{
    void *data_copy;
    Node *rev_list_copy;
    Node *list_copy;

    rev_list_copy = NULL;
    list_copy = NULL;

    for (; head != NULL; head = head->next)
    {
        data_copy = deep_copy(head->data);
        ll_push(&rev_list_copy, data_copy);
    }

    while (rev_list_copy != NULL)
    {
        ll_push(&list_copy, ll_pop(&rev_list_copy));
    }

    return list_copy;
}


void ll_free(Node **head, void(*free_data)(void*))
{
    if (*head == NULL)
        return;

    while (*head != NULL)
        free_data(ll_pop(head));

    return;
}


/* Sort a linked list using the comparison function defined by cmp. */
void ll_sort(Node **head, int16_t(*compare)(void*, void*));

/* TODO */
uint16_t ll_is_subset(Node head_a, Node head_b, uint16_t(*compare)(void *,void *));


#if 0
static struct ll_list* ll_merge(struct ll_list* ll_1, struct ll_list* ll_2,
        int (*cmp)(void *, void *))
{
    struct ll_list *merged;

    ll_init(merged, ll_1->data_size);
    while (ll_1->head != NULL || ll_2->head != NULL)
    {

        if (ll_1->head == NULL)
            ll_add(merged, ll_pop(ll_2));

        else if (ll_2->head == NULL)
            ll_add(merged, ll_pop(ll_1));

        else
        {
            if ((*cmp)(ll_1->head->data, ll_2->head->data) < 0)
                ll_add(merged, ll_pop(ll_2));

            else
                ll_add(merged, ll_pop(ll_1));
        }
    }

    ll_free(ll_1);
    ll_free(ll_2);

    return merged;
}


struct ll_list* ll_sort(struct ll_list* ll, int (*cmp)(void *, void *))
{
    struct ll_list* split_list_a;
    struct ll_list* split_list_b;
    uint16_t index;

    /* Short cuts for diginerate cases. */
    if (ll->head == NULL || ll->head->next == NULL)
        return ll;

    /* Split the list into two halfs. */
    split_list_a = ll_init(ll->data_size);
    split_list_b = ll_init(ll->data_size);
    index = 0;
    while (ll->head != NULL)
    {
        if (index % 2 == 0)
            ll_append(split_list_a, ll_pop(ll));
        else
            ll_append(split_list_b, ll_pop(ll));
    }

    merged_list = ll_merge(ll_sort(ll, cmp), ll_sort(split_list, cmp), cmp);

    return
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
