#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <google/cmockery.h>

#include "linked_list.h"
#include "linked_list_private.h"
#include "unit_testing.h"

#define LIMIT 5


/* Utility functions used by the testing. */
void free_int(void* int_data)
{
    free(int_data);
    return;
}


void* copy_int(void* int_data)
{
    int *data;
    data = malloc(sizeof(int));
    *data = *(int *)int_data;
    return data;
}


int16_t compare_int(void* int_a, void* int_b)
{
    return *(int16_t*)int_a - *(int16_t*)int_b;
}


/* Linked list testing. */

void test_ll_free(void **state)
{
    int i;
    int *data;
    List *list;

    list = ll_create(compare_int, copy_int, free_int);
    ll_free(list);

    list = ll_create(compare_int, copy_int, free_int);
    for (i=0; i<LIMIT; i++)
    {
        data = malloc(sizeof(int));
        *data = i;
        ll_push(list, data);
    }
    ll_free(list);
}


void test_ll_pop(void **state)
{
    int i;
    int *data;
    List *list;

    list = ll_create(compare_int, copy_int, free_int);
    assert_true(ll_pop(list) == NULL);

    for (i=0; i<LIMIT; i++)
    {
        data = malloc(sizeof(int));
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
    int *data;
    List *list;

    list = ll_create(compare_int, copy_int, free_int);

    for (i=0; i<LIMIT; i++)
    {
        data = malloc(sizeof(int));
        *data = i;
        ll_push(list, data);
        assert_int_equal(*(int *)list->head->node_data, i);
    }

    ll_free(list);
}


void test_ll_push_tail(void **state)
{
    int i;
    int *data;
    List *list;

    list = ll_create(compare_int, copy_int, free_int);

    for (i=0; i<LIMIT; i++)
    {
        data = malloc(sizeof(int));
        *data = i;
        ll_push_tail(list, data);
        assert_int_equal(*(int *)list->head->node_data, 0);
    }

    ll_free(list);
}


void test_ll_length(void **state)
{
    int i;
    int *data;
    List *list;

    list = ll_create(compare_int, copy_int, free_int);

    for (i=0; i<LIMIT; i++)
    {
        assert_int_equal(ll_length(list), i);
        data = malloc(sizeof(int));
        *data = i;
        ll_push(list, data);
        assert_int_equal(ll_length(list), i+1);
    }

    ll_free(list);
}


void test_ll_copy(void **state)
{
    int i;
    int *data;
    List *list;
    List *copy;

    list = ll_create(compare_int, copy_int, free_int);

    for (i=0; i<LIMIT; i++)
    {
        data = malloc(sizeof(int));
        *data = i;
        ll_push(list, data);
    }

    copy = ll_copy(list);

    for (i=LIMIT-1; i>=0; i--)
    {
        int *data_a;
        int *data_b;

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
    int *data;
    int* old;
    int* new;

    List *list_a;
    List *list_b;
    List *merged;

    list_a = ll_create(compare_int, copy_int, free_int);
    data = malloc(sizeof(int));
    *data = 4;
    ll_push(list_a, data);
    data = malloc(sizeof(int));
    *data = 2;
    ll_push(list_a, data);
    data = malloc(sizeof(int));
    *data = 0;
    ll_push(list_a, data);

    list_b = ll_create(compare_int, copy_int, free_int);
    data = malloc(sizeof(int));
    *data = 3;
    ll_push(list_b, data);
    data = malloc(sizeof(int));
    *data = 1;
    ll_push(list_b, data);

    merged = merge_sorted(list_a, list_b);

    new = ll_pop(merged);
    while(new != NULL)
    {
        old = new;
        new = ll_pop(merged);
        assert_true(new == NULL || *new >= *old);
        free(old);
    }

    ll_free(merged);
}


void test_ll_sort(void **state)
{
    int *data;
    int* old;
    int* new;

    List *list;

    list = ll_create(compare_int, copy_int, free_int);

    ll_sort(list);
    assert_true(list->head == NULL);

    /* In order list */
    data = malloc(sizeof(int));
    *data = 0;
    ll_push(list, data);
    data = malloc(sizeof(int));
    *data = 1;
    ll_push(list, data);
    data = malloc(sizeof(int));
    *data = 2;
    ll_push(list, data);
    data = malloc(sizeof(int));
    *data = 3;
    ll_push(list, data);
    data = malloc(sizeof(int));
    *data = 4;
    ll_push(list, data);

    ll_sort(list);

    new = ll_pop(list);
    while(new != NULL)
    {
        old = new;
        new = ll_pop(list);
        assert_true(new == NULL || *new >= *old);
        free(old);
    }

    /* Revese order list */
    data = malloc(sizeof(int));
    *data = 4;
    ll_push(list, data);
    data = malloc(sizeof(int));
    *data = 3;
    ll_push(list, data);
    data = malloc(sizeof(int));
    *data = 2;
    ll_push(list, data);
    data = malloc(sizeof(int));
    *data = 1;
    ll_push(list, data);
    data = malloc(sizeof(int));
    *data = 0;
    ll_push(list, data);

    ll_sort(list);

    new = ll_pop(list);
    while(new != NULL)
    {
        old = new;
        new = ll_pop(list);
        assert_true(new == NULL || *new >= *old);
        free(old);
    }

    /* Mixed order list */
    data = malloc(sizeof(int));
    *data = 3;
    ll_push(list, data);
    data = malloc(sizeof(int));
    *data = 4;
    ll_push(list, data);
    data = malloc(sizeof(int));
    *data = 1;
    ll_push(list, data);
    data = malloc(sizeof(int));
    *data = 2;
    ll_push(list, data);
    data = malloc(sizeof(int));
    *data = 0;
    ll_push(list, data);

    ll_sort(list);

    new = ll_pop(list);
    while(new != NULL)
    {
        old = new;
        new = ll_pop(list);
        assert_true(new == NULL || *new >= *old);
        free(old);
    }

    free(list);
}


void test_ll_search(void **state)
{
    int i;
    int *data;
    List *list;

    list = ll_create(compare_int, copy_int, free_int);

    for (i=0; i<LIMIT; i++)
    {
        data = malloc(sizeof(int));
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
    int *data;
    List *list;

    list = ll_create(compare_int, copy_int, free_int);

    for (i=0; i<LIMIT; i++)
    {
        data = malloc(sizeof(int));
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
    int *data;
    List *list;

    list = ll_create(compare_int, copy_int, free_int);

    for (i=0; i<LIMIT; i++)
    {
        data = malloc(sizeof(int));
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
    int *data;
    List *list;
    List *subset;

    list = ll_create(compare_int, copy_int, free_int);

    for (i=0; i<LIMIT; i++)
    {
        data = malloc(sizeof(int));
        *data = i;
        ll_push(list, data);
    }


    subset = ll_create(compare_int, copy_int, free_int);

    assert_true(ll_is_subset_of(list, subset));

    data = malloc(sizeof(int));
    *data = 4;
    ll_push(subset, data);
    assert_true(ll_is_subset_of(list, subset));

    data = malloc(sizeof(int));
    *data = 2;
    ll_push(subset, data);
    assert_true(ll_is_subset_of(list, subset));

    data = malloc(sizeof(int));
    *data = 6;
    ll_push(subset, data);
    assert_false(ll_is_subset_of(list, subset));

    data = malloc(sizeof(int));
    *data = 0;
    ll_push(subset, data);
    assert_false(ll_is_subset_of(list, subset));

    data = ll_pop(subset);
    free(data);
    data = ll_pop(subset);
    free(data);

    data = malloc(sizeof(int));
    *data = 0;
    ll_push(subset, data);
    assert_true(ll_is_subset_of(list, subset));

    data = malloc(sizeof(int));
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
        unit_test(test_ll_search),
        unit_test(test_ll_get_nth),
        unit_test(test_ll_remove),
        unit_test(test_ll_is_subset_of),
    };
    return run_tests(tests);
}
