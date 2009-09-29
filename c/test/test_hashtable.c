#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <google/cmockery.h>

#include "hashtable.h"
#include "uint16_list.h"

#include "unit_testing.h"

#define LIMIT 20


/* Tests. */

void test_ht_create(void **state)
{
    Hashtable *ht;

    expect_assert_failure(ht_create(-1, uint16_hash,
            uint16_compare, uint16_copy, uint16_free,
            uint16_compare, uint16_copy, uint16_free));

    ht = ht_create(0, uint16_hash,
            uint16_compare, uint16_copy, uint16_free,
            uint16_compare, uint16_copy, uint16_free);
    ht_free(ht);
}


void test_ht_insert(void **state)
{
    Hashtable *ht;
    int i;

    ht = ht_create(0, uint16_hash,
            uint16_compare, uint16_copy, uint16_free,
            uint16_compare, uint16_copy, uint16_free);

    for (i=0; i<LIMIT; i++)
    {
        uint16_t *key;
        uint16_t *data;

        data = malloc(sizeof(uint16_t));
        *data = i;
        key = malloc(sizeof(uint16_t));
        *key = i;
        ht_insert(ht, key, data);
    }

    for (i=0; i<LIMIT; i++)
    {
        uint16_t *key;
        uint16_t *data;

        data = malloc(sizeof(uint16_t));
        *data = i;
        key = malloc(sizeof(uint16_t));
        *key = i;
        expect_assert_failure(ht_insert(ht, key, data));
        free(data);
        free(key);
    }

    ht_free(ht);
}


void test_ht_search(void **state)
{
    Hashtable *ht;
    int i;

    ht = ht_create(0, uint16_hash,
            uint16_compare, uint16_copy, uint16_free,
            uint16_compare, uint16_copy, uint16_free);

    for (i=0; i<LIMIT; i++)
    {
        uint16_t *key;
        uint16_t *data;

        data = malloc(sizeof(uint16_t));
        *data = i;
        key = malloc(sizeof(uint16_t));
        *key = i;
        ht_insert(ht, key, data);
    }

    for (i=0; i<LIMIT; i++)
    {
        uint16_t *data;

        data = ht_search(ht, &i);
        assert_true(data != NULL);
        assert_int_equal(*data, i);
    }
    i = LIMIT + 1;
    assert_true(ht_search(ht, &i) == NULL);

    ht_free(ht);
}


void test_ht_num_entries(void **state)
{
    Hashtable *ht;
    int i;

    ht = ht_create(0, uint16_hash,
            uint16_compare, uint16_copy, uint16_free,
            uint16_compare, uint16_copy, uint16_free);

    for (i=0; i<LIMIT; i++)
    {
        uint16_t *data;
        uint16_t *key;

        assert_int_equal(ht_num_entries(ht), i);
        data = malloc(sizeof(uint16_t));
        *data = i;
        key = malloc(sizeof(uint16_t));
        *key = i;
        ht_insert(ht, key, data);
    }
    assert_int_equal(ht_num_entries(ht), i);

    ht_free(ht);
}


void test_ht_remove(void **state)
{
    Hashtable *ht;
    int i;

    ht = ht_create(0, uint16_hash,
            uint16_compare, uint16_copy, uint16_free,
            uint16_compare, uint16_copy, uint16_free);

    for (i=0; i<LIMIT; i++)
    {
        uint16_t *data;
        uint16_t *key;

        data = malloc(sizeof(uint16_t));
        *data = i;
        key = malloc(sizeof(uint16_t));
        *key = i;
        ht_insert(ht, key, data);
    }

    for (i=0; i<LIMIT; i++)
    {
        uint16_t *data;

        data = ht_remove(ht, &i);
        assert_true(data != NULL);
        assert_int_equal(*data, i);
        free(data);
        data = ht_remove(ht, &i);
        assert_true(data == NULL);
    }

    ht_free(ht);
}


void test_ht_free(void **state)
{
    Hashtable *ht;
    int i;

    ht = ht_create(0, uint16_hash,
            uint16_compare, uint16_copy, uint16_free,
            uint16_compare, uint16_copy, uint16_free);

    for (i=0; i<LIMIT; i++)
    {
        uint16_t *data;
        uint16_t *key;

        data = malloc(sizeof(uint16_t));
        *data = i;
        key = malloc(sizeof(uint16_t));
        *key = i;
        ht_insert(ht, key, data);
    }
    ht_free(ht);

    ht = ht_create(0, uint16_hash,
            uint16_compare, uint16_copy, uint16_free,
            uint16_compare, uint16_copy, uint16_free);

    ht_free(ht);
}


int main(int argc, char* argv[]) {
    const UnitTest tests[] = {
        unit_test(test_ht_create),
        unit_test(test_ht_insert),
        unit_test(test_ht_search),
        unit_test(test_ht_num_entries),
        unit_test(test_ht_remove),
        unit_test(test_ht_free),
    };
    return run_tests(tests);
}
