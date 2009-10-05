/*
 * "Copyright (c) 2009 The Regents of the University  of California.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written
 * agreement is hereby granted, provided that the above copyright
 * notice, the following two paragraphs and the author appear in all
 * copies of this software.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
 * FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN
 * IF THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
 * CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT,
 * UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
 *
 * Author: Roy Shea (royshea@gmail.com)
 * Date 10/2009
 */

#include <stdlib.h>
#include <assert.h>

#include "linked_list_private.h"


#if UNIT_TESTING
#include "../test/unit_testing.h"
#endif /* UNIT_TESTING */



List* ll_create(int16_t(*data_compare)(void *,void *),
        void*(*data_copy) (void*), void(*data_free) (void*))
{
    List *list;

    list = malloc(sizeof(List));
    assert(list != NULL);

    list->head = NULL;
    list->tail = NULL;
    list->length = 0;
    list->data_compare = data_compare;
    list->data_copy = data_copy;
    list->data_free = data_free;

    return list;
}


void ll_push(List *list, void *data)
{
    Node *new_head;
    new_head = malloc(sizeof(Node));
    assert(new_head != NULL);

    new_head->node_data = data;
    new_head->next = list->head;
    list->head = new_head;

    /* Special case to update tail when the list was empty. */
    if (list->tail == NULL)
        list->tail = new_head;

    list->length += 1;
    return;
}


void ll_push_tail(List *list, void *data)
{
    Node *new_tail;
    new_tail = malloc(sizeof(Node));
    assert(new_tail != NULL);

    new_tail->node_data = data;
    new_tail->next = NULL;


    /* If list is empty, update head.  If list is NOT empty, update the
     * old tail. */
    if (list->tail == NULL)
        list->head = new_tail;
    else
        list->tail->next = new_tail;

    list->tail = new_tail;

    list->length += 1;
    return;
}


void* ll_pop(List *list)
{
    void *data;
    Node *old_head;

    if (list->head == NULL)
        return NULL;

    old_head = list->head;
    list->head = list->head->next;

    data = old_head->node_data;
    free(old_head);

    /* Special case to update tail when the list becomes empty. */
    if (list->tail == old_head)
        list->tail = NULL;

    list->length -= 1;
    return data;
}


uint16_t ll_length(List *list)
{
    return list->length;
}


List *ll_copy(List *list)
{
    List *list_copy;
    Node *current;
    void *data_copy;

    list_copy = ll_create(list->data_compare, list->data_copy,
            list->data_free);

    for (current = list->head; current != NULL; current = current->next)
    {
        data_copy = list->data_copy(current->node_data);
        ll_push_tail(list_copy, data_copy);
    }

    return list_copy;
}


void ll_free(List *list)
{
    void *data;

    while (list->head != NULL)
    {
        data = ll_pop(list);
        list->data_free(data);
    }
    free(list);

    return;
}


List* merge_sorted(List *list_a, List *list_b)
{
    List *merged_list;
    merged_list = ll_create(list_a->data_compare, list_a->data_copy,
            list_a->data_free);

    /* Walk through head_a and head_b constructing a new list. */
    while (list_a->head != NULL || list_b->head != NULL)
    {
        if (list_a->head == NULL)
        {
            /* Pop from b when a is empty. */
            ll_push_tail(merged_list, ll_pop(list_b));
        }

        else if (list_b->head == NULL)
        {
            /* Pop from a when b is empty. */
            ll_push_tail(merged_list, ll_pop(list_a));
        }

        else if (merged_list->data_compare(list_a->head->node_data,
                    list_b->head->node_data) <= 0)
        {
            /* Pop from a when a's head is less than b's */
            ll_push_tail(merged_list, ll_pop(list_a));
        }

        else
        {
            /* Else b's head is less than a's, so pop from b. */
            ll_push_tail(merged_list, ll_pop(list_b));
        }
    }

    assert(list_a->head == NULL);
    ll_free(list_a);

    assert(list_b->head == NULL);
    ll_free(list_b);

    return merged_list;
}


void ll_sort(List *list)
{
    List *merged_list;
    List *split_a;
    List *split_b;

    /* Base case for merge sort where the list is one or zero elements
     * long.  Simply return the list. */
    if (list->head == NULL || list->head->next == NULL)
        return;

    /* TODO: Check to see if the list is already sorted and return it
     * unmodified if it is.  This is only an order N operation, so
     * really not that expensive.  It has the potential to save a lot of
     * time when working with pre-ordered lists.
     */

    /* Divide list into two halves. */
    split_a = ll_create(list->data_compare, list->data_copy,
            list->data_free);
    split_b = ll_create(list->data_compare, list->data_copy,
            list->data_free);

    /* Split into two lists. */
    while (list->head != NULL)
    {
        ll_push(split_a, ll_pop(list));
        if(list->head != NULL)
            ll_push(split_b, ll_pop(list));
    }
    assert(list->head == NULL);

    /* Sort each of the split lists. */
    ll_sort(split_a);
    ll_sort(split_b);

    /* Merge sorted lists back together. */
    merged_list = merge_sorted(split_a, split_b);

    /* Move the linked list data from merged list into the body of the
     * original list. */
    list->head = merged_list->head;
    list->tail = merged_list->tail;
    list->length = merged_list->length;
    merged_list->head = NULL;
    merged_list->tail = NULL;
    free(merged_list);
}


/* Traverse two lists comparing pairwise elements.  The first list with
 * a smaller key element or, should all elements be equal, the shorter
 * list is considered "less than" in the list ordering. */
int16_t ll_list_compare(void *list_blob_a, void *list_blob_b)
{
    List *list_a;
    List *list_b;
    Node *node_a;
    Node *node_b;

    /* Cast to correct data type. */
    list_a = (List *)list_blob_a;
    list_b = (List *)list_blob_b;

    /* Compare pairwise elements of the key lists. */
    node_a = list_a->head;
    node_b = list_b->head;
    while (node_a != NULL && node_b != NULL)
    {
        int16_t cmp;

        cmp = list_a->data_compare(node_a->node_data, node_b->node_data);
        if (cmp != 0) return cmp;

        node_a = node_a->next;
        node_b = node_b->next;
    }

    return ll_length(list_a) - ll_length(list_b);
}




void* ll_search(List *list, void* data)
{
    Node *current;
    for (current = list->head; current != NULL; current = current->next)
    {
        if (list->data_compare(data, current->node_data) == 0)
            return current->node_data;
    }
    return NULL;
}


void* ll_get_nth(List *list, uint16_t index)
{
    Node *current;

    /* Traverse the list to the requested element.  Return NULL if the
     * end of the list is reached. */
    current = list->head;
    for(; index > 0; index--)
    {
        if (current == NULL) return NULL;
        current = current->next;
    }

    return current->node_data;
}


void* ll_remove(List *list, void* data)
{
    Node *matched_node;
    Node *index;
    void *match;

    /* Initialize matched node to NULL assuming no match. */
    matched_node = NULL;

    /* Basic case for empty list. */
    if (list->head == NULL)
        return NULL;


    if (list->data_compare(data, list->head->node_data) == 0)
    {
        /* Special case for removing the first item in the linked list. */
        matched_node = list->head;
        list->head = matched_node->next;
    }
    else
    {
        /* Walk through list always looking for a match one step ahead. */
        for (index = list->head; index->next != NULL; index = index->next)
        {
            if (list->data_compare(data, index->next->node_data) == 0)
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
        match = matched_node->node_data;
        free(matched_node);
        list->length -= 1;
        return match;
    }

    return NULL;
}


uint8_t ll_is_subset_of(List *list, List *subset)
{
    Node *subset_index;

    assert(list->data_compare == subset->data_compare);

    /* Verify that each element of subset is somewhere within list. */
    for (subset_index = subset->head; subset_index != NULL;
            subset_index = subset_index->next)
    {
        if (ll_search(list, subset_index->node_data) == NULL)
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
