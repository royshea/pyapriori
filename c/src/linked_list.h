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
List *ll_copy(List *list);

/* Free the linked list. */
void ll_free(List *list);

/* Sort a linked list using the comparison function defined by compare. */
void ll_sort(List *list);

/* Compare two linked lists. */
int16_t ll_list_compare(void *list_blob_a, void *list_blob_b);

/* Search for data in a list and, if found, return its reference. */
void* ll_search(List *list, void* data);

/* Compare an element from a linked list Search for data in a list and, if found, return its reference. */
void* ll_search(List *list, void* data);

/* Return reference to the nth element in the list (starting with zero). */
void* ll_get_nth(List *list, uint16_t index);

/* Search for data in a list and, if found, remove from the list and
 * return its reference. */
void* ll_remove(List *list, void* data);

/* Check if subset is contained within the list head. */
uint8_t ll_is_subset_of(List *list, List *subset);

#endif /*LINKED_LIST_*/
