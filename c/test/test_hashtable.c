#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <google/cmockery.h>

#include "hashtable.h"

#include "unit_testing.h"

#define LIMIT 20


/* Utility functions used to construct a hash table using integer keys
 * and data. */

uint16_t hash_int(uint16_t key)
{
    return key;
}


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


/* Tests. */

void test_ht_create(void **state)
{
    Hashtable *ht;

    expect_assert_failure(ht_create( -1, hash_int, copy_int, free_int));

    ht = ht_create(0, hash_int, copy_int, free_int);
    ht_free(ht);
}


void test_ht_insert(void **state)
{
    Hashtable *ht;
    int *data;
    int i;

    ht = ht_create(0, hash_int, copy_int, free_int);

    for (i=0; i<LIMIT; i++)
    {
        data = malloc(sizeof(int));
        *data = i;
        ht_insert(ht, i, data);
    }

    for (i=0; i<LIMIT; i++)
    {
        data = malloc(sizeof(int));
        *data = i;
        expect_assert_failure(ht_insert(ht, i, data));
        free(data);
    }

    ht_free(ht);
}


void test_ht_search(void **state)
{
    Hashtable *ht;
    int *data;
    int i;

    ht = ht_create(0, hash_int, copy_int, free_int);

    for (i=0; i<LIMIT; i++)
    {
        data = malloc(sizeof(int));
        *data = i;
        ht_insert(ht, i, data);
    }

    for (i=0; i<LIMIT; i++)
    {
        data = ht_search(ht, i);
        assert_false(data == NULL);
        assert_int_equal(*data, i);
    }
    data = ht_search(ht, LIMIT + 1);
    assert_true(data == NULL);

    ht_free(ht);
}


void test_ht_remove(void **state)
{
    Hashtable *ht;
    int *data;
    int i;

    ht = ht_create(0, hash_int, copy_int, free_int);

    for (i=0; i<LIMIT; i++)
    {
        data = malloc(sizeof(int));
        *data = i;
        ht_insert(ht, i, data);
    }

    for (i=0; i<LIMIT; i++)
    {
        data = ht_remove(ht, i);
        assert_false(data == NULL);
        assert_int_equal(*data, i);
        free(data);
        data = ht_remove(ht, i);
        assert_true(data == NULL);
    }

    ht_free(ht);
}


void test_ht_free(void **state)
{
    Hashtable *ht;
    int *data;
    int i;

    ht = ht_create(0, hash_int, copy_int, free_int);

    for (i=0; i<LIMIT; i++)
    {
        data = malloc(sizeof(int));
        *data = i;
        ht_insert(ht, i, data);
    }

    ht_free(ht);
}


int main(int argc, char* argv[]) {
    const UnitTest tests[] = {
        unit_test(test_ht_create),
        unit_test(test_ht_insert),
        unit_test(test_ht_search),
        unit_test(test_ht_remove),
        unit_test(test_ht_free),
    };
    return run_tests(tests);
}
