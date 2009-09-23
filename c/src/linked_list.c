#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "linked_list.h"


#if UNIT_TESTING
#include "unit_testing.h"
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
        count += 1;

    return count;
}

void* ll_copy(Node *head, void*(*deep_copy)(void*))
{
    void *data_copy;
    Node *list_copy;
    Node *tail;

    list_copy = NULL;

    /* Copy data into a new list.  Push each data item into the tail of
     * the list using a tail pointer. */
    for (; head != NULL; head = head->next)
    {
        data_copy = deep_copy(head->data);
        if (list_copy == NULL)
        {
            /* Special case for first item put into the list_copy. */
            ll_push(&list_copy, data_copy);
            tail = list_copy;
        }
        else
        {
            ll_push(&tail->next, data_copy);
            tail = tail->next;
        }
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


static Node* ll_merge_sorted(Node **head_a, Node **head_b,
        int16_t(*compare)(void*, void*))
{
    Node merged;
    Node *tail;

    /* Dummy node used to jump start the list that we merge onto. */
    merged.next = NULL;
    tail = &merged;

    /* Walk through head_a and head_b constructing a new list. */
    while (TRUE)
    {
        /* Special cases for empty a or b lists. */
        if (*head_a == NULL)
        {
            tail->next = *head_b;
            *head_b = NULL;
            break;
        }
        else if (*head_b == NULL)
        {
            tail->next = *head_a;
            *head_a = NULL;
            break;
        }

        /* Push the smaller of the two onto the tail of the constructed
         * list.  Note that this requires careful maintenance of the
         * tail pointer. */
        if (compare((*head_a)->data, (*head_b)->data) <= 0)
            ll_push(&tail->next, ll_pop(head_a));
        else
            ll_push(&tail->next, ll_pop(head_b));
        tail = tail->next;
    }

    return merged.next;
}


void ll_sort(Node **head, int16_t(*compare)(void*, void*))
{

    Node *split_a;
    Node *split_b;

   /* Base case for merge sort where the list is one or zero elements
    * long.  Simply return the list. */
   if (*head == NULL || (*head)->next == NULL)
       return;

   /* Split into two lists. */
   split_a = NULL;
   split_b = NULL;
   while (*head != NULL)
   {
       ll_push(&split_a, ll_pop(head));
       if(*head != NULL)
           ll_push(&split_b, ll_pop(head));
   }

   /* Sort each of the split lists. */
   ll_sort(&split_a, compare);
   ll_sort(&split_b, compare);

   /* Merge sorted lists back together. */
   *head = ll_merge_sorted(&split_a, &split_b, compare);
}


uint8_t ll_is_subset_of(Node *subset, Node *head,
        int16_t(*compare)(void *,void *))
{
    Node *subset_index;
    Node *head_index;
    uint8_t found;

    /* Verify that each element of subset is somewhere within head. */
    for (subset_index = subset; subset_index != NULL; subset_index =
            subset_index->next)
    {
        found = FALSE;
        for (head_index = head; found == FALSE && head_index != NULL;
                head_index = head_index->next)
        {
            if (compare(subset_index->data, head_index->data) == 0)
                found = TRUE;
        }

        /* Return immediately with FALSE if an element was not found. */
        if (found == FALSE)
            return FALSE;
    }
    return TRUE;
}


#if 0
/* TODO: Not implementing this function until I understand how it is
 * used within the hash tree. */
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
#endif
