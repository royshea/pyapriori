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

/* Linked list testing. */

void test_ll_init(void **state)
{
    struct ll_list *ll;
    ll = ll_init(sizeof(int));
    ll_free(ll);
}


void test_ll_append(void **state)
{

    struct ll_list *ll;
    int data;

    ll = ll_init(sizeof(int));
    data = 1;
    ll_append(ll, &data);
    data = 2;
    ll_append(ll, &data);
    data = 3;
    ll_append(ll, &data);
    data = 4;
    ll_append(ll, &data);
    ll_free(ll);
}


void test_ll_pop(void **state)
{

    struct ll_list *ll;
    int data;
    int *popped_data;

    ll = ll_init(sizeof(int));
    data = 1;
    ll_append(ll, &data);
    data = 2;
    ll_append(ll, &data);
    data = 3;
    ll_append(ll, &data);
    data = 4;
    ll_append(ll, &data);

    popped_data = ll_pop(ll);
    assert_int_equal(*popped_data, 1);
    free(popped_data);

    popped_data = ll_pop(ll);
    assert_int_equal(*popped_data, 2);
    free(popped_data);

    popped_data = ll_pop(ll);
    assert_int_equal(*popped_data, 3);
    free(popped_data);

    popped_data = ll_pop(ll);
    assert_int_equal(*popped_data, 4);
    free(popped_data);

    expect_assert_failure(ll_pop(ll));

    ll_free(ll);
}


void test_ll_length(void **state)
{
    struct ll_list *ll;
    int data;

    ll = ll_init(sizeof(int));
    assert_int_equal(ll_length(ll), 0);

    data = 1;
    ll_append(ll, &data);
    assert_int_equal(ll_length(ll), 1);
    data = 2;

    ll_append(ll, &data);
    assert_int_equal(ll_length(ll), 2);
    data = 3;

    ll_append(ll, &data);
    assert_int_equal(ll_length(ll), 3);
    data = 4;

    ll_append(ll, &data);
    assert_int_equal(ll_length(ll), 4);
    ll_free(ll);

}


void test_ll_copy(void **state)
{
    struct ll_list *ll;
    struct ll_list *copy;
    int data;

    ll = ll_init(sizeof(int));

    copy = ll_copy(ll);
    assert_int_equal(ll_length(copy), 0);
    ll_free(copy);

    data = 1;
    ll_append(ll, &data);
    copy = ll_copy(ll);
    assert_int_equal(ll_length(copy), 1);
    ll_free(copy);

    data = 2;
    ll_append(ll, &data);
    copy = ll_copy(ll);
    assert_int_equal(ll_length(copy), 2);
    ll_free(copy);

    data = 3;
    ll_append(ll, &data);
    copy = ll_copy(ll);
    assert_int_equal(ll_length(copy), 3);
    ll_free(copy);

    data = 4;
    ll_append(ll, &data);
    copy = ll_copy(ll);
    assert_int_equal(ll_length(copy), 4);
    ll_free(copy);

    ll_free(ll);

}


int main(int argc, char* argv[]) {
    const UnitTest tests[] = {
        unit_test(test_ll_init),
        unit_test(test_ll_append),
        unit_test(test_ll_pop),
        unit_test(test_ll_length),
        unit_test(test_ll_copy),
    };
    return run_tests(tests);
}
