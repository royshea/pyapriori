#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <google/cmockery.h>

#include "linked_list.h"

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

/* Redefine assert for use with cmockery when unit testing. */

#define assert(expression) \
        mock_assert((int)(expression), #expression, __FILE__, __LINE__);

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
    Node *head;
    head = NULL;

    ll_free(&head, free_int);
    assert_true(head == NULL);

    for (i=0; i<LIMIT; i++)
    {
        int *data;
        data = malloc(sizeof(int));
        *data = i;
        ll_push(&head, data);
    }

    ll_free(&head, free_int);
    assert_true(head == NULL);
}


void test_ll_pop(void **state)
{
    int i;
    Node *head;

    head = NULL;
    expect_assert_failure(ll_pop(&head));

    for (i=0; i<LIMIT; i++)
    {
        int *data;
        data = malloc(sizeof(int));
        *data = i;
        ll_push(&head, data);
    }

    for (i=LIMIT-1; i>=0; i--)
    {
        int *data;
        data = ll_pop(&head);
        assert_int_equal(*data, i);
        free(data);
    }

    expect_assert_failure(ll_pop(&head));
}


void test_ll_push(void **state)
{
    int i;
    Node *head;

    head = NULL;

    for (i=0; i<LIMIT; i++)
    {
        int *data;
        data = malloc(sizeof(int));
        *data = i;
        ll_push(&head, data);
        assert_true(head != NULL);
        assert_true(head->data == data);
    }

    ll_free(&head, free_int);
}


void test_ll_length(void **state)
{
    int i;
    Node *head;

    head = NULL;

    for (i=0; i<LIMIT; i++)
    {
        int *data;
        assert_int_equal(ll_length(head), i);
        data = malloc(sizeof(int));
        *data = i;
        ll_push(&head, data);
        assert_int_equal(ll_length(head), i+1);
    }

    ll_free(&head, free_int);
}


void test_ll_copy(void **state)
{
    int i;
    Node *head;
    Node *copy;

    head = NULL;

    for (i=0; i<LIMIT; i++)
    {
        int *data;
        data = malloc(sizeof(int));
        *data = i;
        ll_push(&head, data);
    }

    copy = ll_copy(head, copy_int);

    for (i=LIMIT-1; i>=0; i--)
    {
        int *data_a;
        int *data_b;

        data_a = ll_pop(&head);
        data_b = ll_pop(&copy);

        assert_int_equal(*data_a, *data_b);

        free(data_a);
        free(data_b);
    }

    expect_assert_failure(ll_pop(&head));
    expect_assert_failure(ll_pop(&copy));
}


void test_ll_sort(void **state)
{
    Node *head;
    int *data;

    int* old;
    int* new;

    head = NULL;

    ll_sort(&head, compare_int);
    assert_true(head == NULL);

    /* In order list */
    data = malloc(sizeof(int));
    *data = 0;
    ll_push(&head, data);
    data = malloc(sizeof(int));
    *data = 1;
    ll_push(&head, data);
    data = malloc(sizeof(int));
    *data = 2;
    ll_push(&head, data);
    data = malloc(sizeof(int));
    *data = 3;
    ll_push(&head, data);
    data = malloc(sizeof(int));
    *data = 4;
    ll_push(&head, data);
    ll_sort(&head, compare_int);
    old = ll_pop(&head);
    while(head != NULL)
    {
        new = ll_pop(&head);
        assert_true(*new >= *old);
        free(old);
        old = new;
    }
    free(old);

    /* Revese order list */
    data = malloc(sizeof(int));
    *data = 4;
    ll_push(&head, data);
    data = malloc(sizeof(int));
    *data = 3;
    ll_push(&head, data);
    data = malloc(sizeof(int));
    *data = 2;
    ll_push(&head, data);
    data = malloc(sizeof(int));
    *data = 1;
    ll_push(&head, data);
    data = malloc(sizeof(int));
    *data = 0;
    ll_push(&head, data);
    ll_sort(&head, compare_int);
    old = ll_pop(&head);
    while(head != NULL)
    {
        new = ll_pop(&head);
        assert_true(*new >= *old);
        free(old);
        old = new;
    }
    free(old);

    /* Mixed order list */
    data = malloc(sizeof(int));
    *data = 3;
    ll_push(&head, data);
    data = malloc(sizeof(int));
    *data = 4;
    ll_push(&head, data);
    data = malloc(sizeof(int));
    *data = 1;
    ll_push(&head, data);
    data = malloc(sizeof(int));
    *data = 2;
    ll_push(&head, data);
    data = malloc(sizeof(int));
    *data = 0;
    ll_push(&head, data);
    ll_sort(&head, compare_int);
    old = ll_pop(&head);
    while(head != NULL)
    {
        new = ll_pop(&head);
        assert_true(*new >= *old);
        free(old);
        old = new;
    }
    free(old);
}


void test_ll_is_subset_of(void **state)
{
    int i;
    int *data;
    Node *head;
    Node *subset;

    head = NULL;
    for (i=0; i<5; i++)
    {
        data = malloc(sizeof(int));
        *data = i;
        ll_push(&head, data);
    }

    subset = NULL;
    assert_true(ll_is_subset_of(subset, head, compare_int));

    data = malloc(sizeof(int));
    *data = 4;
    ll_push(&subset, data);
    assert_true(ll_is_subset_of(subset, head, compare_int));

    data = malloc(sizeof(int));
    *data = 2;
    ll_push(&subset, data);
    assert_true(ll_is_subset_of(subset, head, compare_int));

    data = malloc(sizeof(int));
    *data = 6;
    ll_push(&subset, data);
    assert_false(ll_is_subset_of(subset, head, compare_int));

    data = malloc(sizeof(int));
    *data = 0;
    ll_push(&subset, data);
    assert_false(ll_is_subset_of(subset, head, compare_int));

    data = ll_pop(&subset);
    free(data);
    data = ll_pop(&subset);
    free(data);

    data = malloc(sizeof(int));
    *data = 0;
    ll_push(&subset, data);
    assert_true(ll_is_subset_of(subset, head, compare_int));

    data = malloc(sizeof(int));
    *data = 4;
    ll_push(&subset, data);
    assert_true(ll_is_subset_of(subset, head, compare_int));

    ll_free(&subset, free_int);
    ll_free(&head, free_int);
}


int main(int argc, char* argv[]) {
    const UnitTest tests[] = {
        unit_test(test_ll_free),
        unit_test(test_ll_pop),
        unit_test(test_ll_push),
        unit_test(test_ll_length),
        unit_test(test_ll_copy),
        unit_test(test_ll_sort),
        unit_test(test_ll_is_subset_of),
    };
    return run_tests(tests);
}
