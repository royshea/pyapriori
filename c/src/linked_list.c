#include <stdlib.h>
#include <assert.h>

#include "linked_list_private.h"


#if UNIT_TESTING
#include "../test/unit_testing.h"
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
    void *data;

    if (*head == NULL)
        return;

    while (*head != NULL)
    {
        data = ll_pop(head);
        free_data(data);
    }

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


void* ll_search(Node *head, void* data, int16_t(*compare)(void *,void *))
{
    Node *index;
    for (index = head; index != NULL; index = index->next)
    {
        if (compare(data, index->data) == 0)
            return index->data;
    }
    return NULL;
}


void* ll_get_nth(Node *head, uint8_t index)
{
    /* Return NULL in case of empty list. */
    if (head == NULL)
        return NULL;

    /* Traverse the list to the requested element.  Return NULL if the
     * end of the list is reached. */
    for(; index > 0; index--)
    {
        if (head->next == NULL)
            return NULL;
        head = head->next;
    }

    return head->data;
}


void* ll_remove(Node **head, void* data, int16_t(*compare)(void *,void *))
{
    Node *matched_node;
    Node *index;
    void *match;

    /* Initialize matched node to NULL assuming no match. */
    matched_node = NULL;

    /* Basic case for empty list. */
    if (*head == NULL)
        return NULL;

    if ((compare(data, (*head)->data)) == 0)
    {
        /* Special case for removing the first item in the linked list. */
        matched_node = *head;
        *head = matched_node->next;
    }
    else
    {
        /* Walk through list always looking for a match one step ahead. */
        for (index = *head; index->next != NULL; index = index->next)
        {
            if (compare(data, index->next->data) == 0)
            {
                matched_node = index->next;
                index->next = matched_node->next;
                break;
            }
        }
    }

    /* If a matched node is found remove it from the list. */
    if (matched_node != NULL)
    {
        match = matched_node->data;
        free(matched_node);
        return match;
    }

    return NULL;
}


uint8_t ll_is_subset_of(Node *head, Node *subset,
        int16_t(*compare)(void *,void *))
{
    Node *subset_index;

    /* Verify that each element of subset is somewhere within head. */
    for (subset_index = subset; subset_index != NULL; subset_index =
            subset_index->next)
    {
        if (ll_search(head, subset_index->data, compare) == NULL)
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
