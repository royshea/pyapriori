#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <google/cmockery.h>
#include <stdio.h>

#include "apriori.h"
#include "uint16_list.h"

#include "unit_testing.h"

char tmp_file_name[] = "test_apriori_data.tmp";

void test_read_uint16_list(void **state)
{
    FILE *fid;
    List *list;
    int i;

    /* Basic test. */
    fid = fopen(tmp_file_name, "w");
    fprintf(fid, "0 1 2 3");
    fclose(fid);
    list = read_uint16_list(tmp_file_name);
    assert_int_equal(ll_length(list), 4);
    ll_sort(list);
    for (i=0; i<4; i++)
    {
        uint16_t *data;
        data = ll_get_nth(list, i);
        assert_int_equal(*data, i);
    }
    ll_free(list);

    /* More complex data file formatting. */
    fid = fopen(tmp_file_name, "w");
    fprintf(fid, "   0\t 1\n2 3  ");
    fclose(fid);
    list = read_uint16_list(tmp_file_name);
    assert_int_equal(ll_length(list), 4);
    ll_sort(list);
    for (i=0; i<4; i++)
    {
        uint16_t *data;
        data = ll_get_nth(list, i);
        assert_int_equal(*data, i);
    }
    ll_free(list);

    remove(tmp_file_name);
}


void test_make_transactions_fixed_width(void **state)
{
    List *stream;
    List *transactions;
    List *sublist;
    List *tmp;

    stream = uint16_list_create(10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9);

    /* Verify failure of creating size 0 transactions. */
    expect_assert_failure(make_transactions_fixed_width(stream, 0));

    /* Create size 3 transaction. */
    transactions = make_transactions_fixed_width(stream, 3);
    assert_int_equal(ll_length(transactions), 8);

    ll_sort(transactions);

    tmp = ll_get_nth(transactions, 0);
    ll_sort(tmp);
    sublist = uint16_list_create(3, 0, 1, 2);
    assert_int_equal(ll_list_compare(sublist, tmp), 0);
    ll_free(sublist);

    tmp = ll_get_nth(transactions, 7);
    ll_sort(tmp);
    sublist = uint16_list_create(3, 7, 8, 9);
    assert_int_equal(ll_list_compare(sublist, tmp), 0);
    ll_free(sublist);

    ll_free(transactions);
    ll_free(stream);
}


void test_generate_frequent_size_one(void **main)
{
    List *stream;
    List *transactions;
    List *size_one;

    /* Create a stream of transactions with the following properties:
     * 0 -> 1x
     * 1 -> 2x
     * 2 -> 3x
     * 3 -> 4x
     * 4 -> 5x
     * 9 -> a lot
     * The value 9 is used to seperate the numbers 1-4 so that each only
     * occures in its own isolated window of size 3.
     */
    stream = uint16_list_create(17 * 3,
            9, 9, 9,
            9, 0, 9,
            9, 1, 9,
            9, 1, 9,
            9, 2, 9,
            9, 2, 9,
            9, 2, 9,
            9, 3, 9,
            9, 3, 9,
            9, 3, 9,
            9, 3, 9,
            9, 4, 9,
            9, 4, 9,
            9, 4, 9,
            9, 4, 9,
            9, 4, 9,
            9, 9, 9);

    /* Create size 3 transaction. */
    transactions = make_transactions_fixed_width(stream, 3);
    assert_int_equal(ll_length(transactions), 49);

    expect_assert_failure(generate_frequent_size_one(stream,
                transactions, 0));

    size_one = generate_frequent_size_one(stream, transactions, .02);
    assert_int_equal(ll_length(size_one), 6);
    ll_sort(size_one);
    assert_int_equal(*(uint16_t *)ll_get_nth(size_one, 0), 0);
    ll_free(size_one);

    size_one = generate_frequent_size_one(stream, transactions, .04);
    assert_int_equal(ll_length(size_one), 5);
    ll_sort(size_one);
    assert_int_equal(*(uint16_t *)ll_get_nth(size_one, 0), 1);
    ll_free(size_one);

    size_one = generate_frequent_size_one(stream, transactions, .06);
    assert_int_equal(ll_length(size_one), 4);
    ll_sort(size_one);
    assert_int_equal(*(uint16_t *)ll_get_nth(size_one, 0), 2);
    ll_free(size_one);

    size_one = generate_frequent_size_one(stream, transactions, .08);
    assert_int_equal(ll_length(size_one), 3);
    ll_sort(size_one);
    assert_int_equal(*(uint16_t *)ll_get_nth(size_one, 0), 3);
    ll_free(size_one);

    size_one = generate_frequent_size_one(stream, transactions, .10);
    assert_int_equal(ll_length(size_one), 2);
    ll_sort(size_one);
    assert_int_equal(*(uint16_t *)ll_get_nth(size_one, 0), 4);
    ll_free(size_one);

    size_one = generate_frequent_size_one(stream, transactions, .12);
    assert_int_equal(ll_length(size_one), 1);
    ll_sort(size_one);
    assert_int_equal(*(uint16_t *)ll_get_nth(size_one, 0), 9);
    ll_free(size_one);

    ll_free(transactions);
    ll_free(stream);
}

int main(int argc, char* argv[]) {
    const UnitTest tests[] = {
        unit_test(test_read_uint16_list),
        unit_test(test_make_transactions_fixed_width),
        unit_test(test_generate_frequent_size_one),
    };
    return run_tests(tests);
}
