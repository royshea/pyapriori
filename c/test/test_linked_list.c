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

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <google/cmockery.h>

#include "linked_list.h"
#include "linked_list_private.h"

#include "uint16_list.h"

#include "unit_testing.h"

#define LIMIT 5


/* Utility for testing that a list is sorted.  Frees the list in the
 * process. */
void test_sorted_then_free(List *list)
{
    uint16_t *new;
    uint16_t *old;

    new = ll_pop(list);
    while(new != NULL)
    {
        old = new;
        new = ll_pop(list);
        assert_true(new == NULL || *new >= *old);
        free(old);
    }
    ll_free(list);
}


/* Linked list testing. */
void test_ll_free(void **state)
{
    int i;
    uint16_t *data;
    List *list;

    list = ll_create(uint16_compare, uint16_copy, uint16_free);
    ll_free(list);

    list = ll_create(uint16_compare, uint16_copy, uint16_free);
    for (i=0; i<LIMIT; i++)
    {
        data = malloc(sizeof(int16_t));
        *data = i;
        ll_push(list, data);
    }
    ll_free(list);
}


void test_ll_pop(void **state)
{
    int i;
    uint16_t *data;
    List *list;

    list = ll_create(uint16_compare, uint16_copy, uint16_free);
    assert_true(ll_pop(list) == NULL);

    for (i=0; i<LIMIT; i++)
    {
        data = malloc(sizeof(int16_t));
        *data = i;
        ll_push(list, data);
    }

    for (i=LIMIT-1; i>=0; i--)
    {
        data = ll_pop(list);
        assert_int_equal(*data, i);
        free(data);
    }

    assert_true(ll_pop(list) == NULL);
    ll_free(list);
}


void test_ll_push(void **state)
{
    int i;
    uint16_t *data;
    List *list;

    list = ll_create(uint16_compare, uint16_copy, uint16_free);

    for (i=0; i<LIMIT; i++)
    {
        data = malloc(sizeof(int16_t));
        *data = i;
        ll_push(list, data);
        assert_int_equal(*(uint16_t *)list->head->node_data, i);
    }

    ll_free(list);
}


void test_ll_push_tail(void **state)
{
    int i;
    uint16_t *data;
    List *list;

    list = ll_create(uint16_compare, uint16_copy, uint16_free);

    for (i=0; i<LIMIT; i++)
    {
        data = malloc(sizeof(int16_t));
        *data = i;
        ll_push_tail(list, data);
        assert_int_equal(*(uint16_t *)list->head->node_data, 0);
    }

    ll_free(list);
}


void test_ll_length(void **state)
{
    int i;
    uint16_t *data;
    List *list;

    list = ll_create(uint16_compare, uint16_copy, uint16_free);

    for (i=0; i<LIMIT; i++)
    {
        assert_int_equal(ll_length(list), i);
        data = malloc(sizeof(int16_t));
        *data = i;
        ll_push(list, data);
        assert_int_equal(ll_length(list), i+1);
    }

    ll_free(list);
}


void test_ll_copy(void **state)
{
    int i;
    uint16_t *data;
    List *list;
    List *copy;

    list = ll_create(uint16_compare, uint16_copy, uint16_free);

    for (i=0; i<LIMIT; i++)
    {
        data = malloc(sizeof(uint16_t));
        *data = i;
        ll_push(list, data);
    }

    copy = ll_copy(list);

    for (i=LIMIT-1; i>=0; i--)
    {
        uint16_t *data_a;
        uint16_t *data_b;

        data_a = ll_pop(list);
        data_b = ll_pop(copy);

        assert_int_equal(*data_a, *data_b);

        free(data_a);
        free(data_b);
    }

    assert_true(ll_pop(list) == NULL);
    assert_true(ll_pop(copy) == NULL);

    ll_free(list);
    ll_free(copy);
}


void test_merge_sorted(void **state)
{
    List *list_a;
    List *list_b;
    List *merged;

    list_a = uint16_list_create(3, 0, 2, 4);
    list_b = uint16_list_create(2, 1, 3);
    merged = merge_sorted(list_a, list_b);
    test_sorted_then_free(merged);
}


void test_ll_sort(void **state)
{
    List *list;

    /* Empty list. */
    list = uint16_list_create(0);
    ll_sort(list);
    test_sorted_then_free(list);

    /* In order list */
    list = uint16_list_create(5, 0, 1, 2, 3, 4);
    ll_sort(list);
    test_sorted_then_free(list);

    /* Revese order list */
    list = uint16_list_create(5, 4, 3, 2, 1, 0);
    ll_sort(list);
    test_sorted_then_free(list);

    /* Mixed order list */
    list = uint16_list_create(5, 3, 4, 1, 2, 0);
    ll_sort(list);
    test_sorted_then_free(list);

    /* Repeated elements */
    list = uint16_list_create(10, 3, 4, 1, 2, 0, 0, 2, 1, 4, 3);
    ll_sort(list);
    test_sorted_then_free(list);


}


void test_ll_compare(void **state)
{
    List *list_a;
    List *list_b;
    List *list_c;

    list_a = uint16_list_create(5, 0, 1, 2, 3, 5);
    list_b = uint16_list_create(4, 0, 1, 2, 3);
    list_c = uint16_list_create(5, 0, 1, 2, 1, 4);

    assert_true(ll_list_compare((void *)list_a, (void *)list_a) == 0);
    assert_true(ll_list_compare((void *)list_a, (void *)list_b) > 0);
    assert_true(ll_list_compare((void *)list_a, (void *)list_c) > 0);
    assert_true(ll_list_compare((void *)list_b, (void *)list_c) > 0);

    ll_free(list_a);
    ll_free(list_b);
    ll_free(list_c);
}


void test_ll_search(void **state)
{
    int i;
    uint16_t *data;
    List *list;

    list = ll_create(uint16_compare, uint16_copy, uint16_free);

    for (i=0; i<LIMIT; i++)
    {
        data = malloc(sizeof(int16_t));
        *data = i;
        ll_push(list, data);
    }

    for (i=0; i<LIMIT; i++)
    {
        data = ll_search(list, &i);
        assert_true(data != NULL);
        assert_int_equal(*data, i);
    }
    i = LIMIT + 1;
    data = ll_search(list, &i);
    assert_true(data == NULL);

    ll_free(list);
}


void test_ll_get_nth(void **state)
{
    int i;
    uint16_t *data;
    List *list;

    list = ll_create(uint16_compare, uint16_copy, uint16_free);

    for (i=0; i<LIMIT; i++)
    {
        data = malloc(sizeof(int16_t));
        *data = i;
        ll_push(list, data);
    }

    for (i=0; i<LIMIT; i++)
    {
        data = ll_get_nth(list, i);
        assert_true(data != NULL);
        assert_int_equal(*data, LIMIT - i - 1);
    }
    i = LIMIT + 1;
    data = ll_get_nth(list, i);
    assert_true(data == NULL);

    ll_free(list);
}


void test_ll_remove(void **state)
{
    int i;
    uint16_t *data;
    List *list;

    list = ll_create(uint16_compare, uint16_copy, uint16_free);

    for (i=0; i<LIMIT; i++)
    {
        data = malloc(sizeof(int16_t));
        *data = i;
        ll_push(list, data);
    }

    for (i=0; i<LIMIT; i++)
    {
        data = ll_remove(list, &i);
        assert_true(data != NULL);
        assert_int_equal(*data, i);
        free(data);
        data = ll_remove(list, &i);
        assert_true(data == NULL);
    }

    ll_free(list);
}


void test_ll_is_subset_of(void **state)
{
    int i;
    uint16_t *data;
    List *list;
    List *subset;

    list = ll_create(uint16_compare, uint16_copy, uint16_free);

    for (i=0; i<LIMIT; i++)
    {
        data = malloc(sizeof(int16_t));
        *data = i;
        ll_push(list, data);
    }


    subset = ll_create(uint16_compare, uint16_copy, uint16_free);

    assert_true(ll_is_subset_of(list, subset));

    data = malloc(sizeof(int16_t));
    *data = 4;
    ll_push(subset, data);
    assert_true(ll_is_subset_of(list, subset));

    data = malloc(sizeof(int16_t));
    *data = 2;
    ll_push(subset, data);
    assert_true(ll_is_subset_of(list, subset));

    data = malloc(sizeof(int16_t));
    *data = 6;
    ll_push(subset, data);
    assert_false(ll_is_subset_of(list, subset));

    data = malloc(sizeof(int16_t));
    *data = 0;
    ll_push(subset, data);
    assert_false(ll_is_subset_of(list, subset));

    data = ll_pop(subset);
    free(data);
    data = ll_pop(subset);
    free(data);

    data = malloc(sizeof(int16_t));
    *data = 0;
    ll_push(subset, data);
    assert_true(ll_is_subset_of(list, subset));

    data = malloc(sizeof(int16_t));
    *data = 4;
    ll_push(subset, data);
    assert_true(ll_is_subset_of(list, subset));

    ll_free(subset);
    ll_free(list);
}


int main(int argc, char* argv[]) {
    const UnitTest tests[] = {
        unit_test(test_ll_free),
        unit_test(test_ll_pop),
        unit_test(test_ll_push),
        unit_test(test_ll_push_tail),
        unit_test(test_ll_length),
        unit_test(test_ll_copy),
        unit_test(test_merge_sorted),
        unit_test(test_ll_sort),
        unit_test(test_ll_compare),
        unit_test(test_ll_search),
        unit_test(test_ll_get_nth),
        unit_test(test_ll_remove),
        unit_test(test_ll_is_subset_of),
    };
    return run_tests(tests);
}
