#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <google/cmockery.h>

#include "hashtable.h"

#include "unit_testing.h"

#define LIMIT 5


/* Utility functions used to construct a hash table using integer keys
 * and data. */

uint16_t hash_int(void* key)
{
    return *(int *)key;
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


int16_t compare_int(void* int_a, void* int_b)
{
    return *(int16_t*)int_a - *(int16_t*)int_b;
}


/* Tests. */

void test_ht_create(void **state)
{
    Hashtable *ht;

    expect_assert_failure(ht_create( -1, hash_int, compare_int, copy_int,
                free_int, free_int));

    ht = ht_create(0, hash_int, compare_int, copy_int, free_int, free_int);
    ht_free(ht);
}


void test_ht_insert(void **state)
{
}


void test_ht_search(void **state)
{
}


void test_ht_remove(void **state)
{
}


void test_ht_free(void **state)
{
    Hashtable *ht;
    int *data;
    int *key;
    int i;

    ht = ht_create(0, hash_int, compare_int, copy_int, free_int, free_int);

    for (i=0; i<LIMIT; i++)
    {
        data = malloc(sizeof(int));
        *data = i;
        key = malloc(sizeof(int));
        *key = i;
        ht_insert(ht, key, data);
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
