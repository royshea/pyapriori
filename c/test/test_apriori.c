#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <google/cmockery.h>
#include <stdio.h>

#include "uint16_list.h"

#include "hashtree.h"
#include "hashtable.h"

#include "hashtree_private.h"
#include "apriori.h"

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
    remove(tmp_file_name);
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
    remove(tmp_file_name);
    assert_int_equal(ll_length(list), 4);
    ll_sort(list);
    for (i=0; i<4; i++)
    {
        uint16_t *data;
        data = ll_get_nth(list, i);
        assert_int_equal(*data, i);
    }
    ll_free(list);

    /* Testing types of invalid data.
     *
     * NOTE: Current implementation fails to recognize out of range
     * positive or negative integers.
     */
    fid = fopen(tmp_file_name, "w");
    fprintf(fid, "0 3.14 2 3");
    fclose(fid);
    expect_assert_failure(read_uint16_list(tmp_file_name));
    remove(tmp_file_name);

    fid = fopen(tmp_file_name, "w");
    fprintf(fid, "0 a 2 3");
    fclose(fid);
    expect_assert_failure(read_uint16_list(tmp_file_name));
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


void test_generate_frequent_size_one(void **state)
{
    List *stream;
    List *transactions;
    List *size_one;
    List *singleton;

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

    size_one = generate_frequent_size_one(stream, transactions, .025 * 3);
    assert_int_equal(ll_length(size_one), 6);
    ll_sort(size_one);
    singleton = (List *)ll_get_nth(size_one, 0);
    assert_int_equal(*(uint16_t *)ll_get_nth(singleton, 0), 0);
    ll_free(size_one);

    size_one = generate_frequent_size_one(stream, transactions, .045 * 3);
    assert_int_equal(ll_length(size_one), 5);
    ll_sort(size_one);
    singleton = (List *)ll_get_nth(size_one, 0);
    assert_int_equal(*(uint16_t *)ll_get_nth(singleton, 0), 1);
    ll_free(size_one);

    size_one = generate_frequent_size_one(stream, transactions, .065 * 3);
    assert_int_equal(ll_length(size_one), 4);
    ll_sort(size_one);
    singleton = (List *)ll_get_nth(size_one, 0);
    assert_int_equal(*(uint16_t *)ll_get_nth(singleton, 0), 2);
    ll_free(size_one);

    size_one = generate_frequent_size_one(stream, transactions, .085 * 3);
    assert_int_equal(ll_length(size_one), 3);
    ll_sort(size_one);
    singleton = (List *)ll_get_nth(size_one, 0);
    assert_int_equal(*(uint16_t *)ll_get_nth(singleton, 0), 3);
    ll_free(size_one);

    size_one = generate_frequent_size_one(stream, transactions, .105 * 3);
    assert_int_equal(ll_length(size_one), 2);
    ll_sort(size_one);
    singleton = (List *)ll_get_nth(size_one, 0);
    assert_int_equal(*(uint16_t *)ll_get_nth(singleton, 0), 4);
    ll_free(size_one);

    size_one = generate_frequent_size_one(stream, transactions, .125 * 3);
    assert_int_equal(ll_length(size_one), 1);
    ll_sort(size_one);
    singleton = (List *)ll_get_nth(size_one, 0);
    assert_int_equal(*(uint16_t *)ll_get_nth(singleton, 0), 9);
    ll_free(size_one);

    ll_free(transactions);
    ll_free(stream);
}


void test_generate_candidate_sets_a(void **state)
{
    List *candidates;
    List *candidate;
    List *tmp;

    /* Create a seed of size one sets. */
    candidates = ll_create(uint16_list_compare, uint16_list_copy,
            uint16_list_free);
    ll_push_tail(candidates, uint16_list_create(1, 3));
    ll_push_tail(candidates, uint16_list_create(1, 4));
    ll_push_tail(candidates, uint16_list_create(1, 9));

    /* Calculate candidate sets.  From the singletons 3, 4, and 9 we
     * expect the tuples (3,4), (3,9), and (4,9). */
    tmp = generate_candidate_sets(candidates);
    ll_free(candidates);
    candidates = tmp;
    ll_sort(candidates);
    assert_int_equal(ll_length(candidates), 3);

    /* Verify the the expected sets were generated. */
    tmp = uint16_list_create(2, 3, 4);
    candidate = (List *)ll_get_nth(candidates, 0);
    assert_int_equal(ll_list_compare(tmp, candidate), 0);
    ll_free(tmp);

    tmp = uint16_list_create(2, 3, 9);
    candidate = (List *)ll_get_nth(candidates, 1);
    assert_int_equal(ll_list_compare(tmp, candidate), 0);
    ll_free(tmp);

    tmp = uint16_list_create(2, 4, 9);
    candidate = (List *)ll_get_nth(candidates, 2);
    assert_int_equal(ll_list_compare(tmp, candidate), 0);
    ll_free(tmp);

    /* Calculate candidate sets.  From the tuples (3,4), (3,9), and
     * (4,9) we expect the single set (3,4,9). */
    tmp = generate_candidate_sets(candidates);
    ll_free(candidates);
    candidates = tmp;
    ll_sort(candidates);
    assert_int_equal(ll_length(candidates), 1);

    /* Verify the the expected sets were generated. */
    tmp = uint16_list_create(3, 3, 4, 9);
    candidate = (List *)ll_get_nth(candidates, 0);
    assert_int_equal(ll_list_compare(tmp, candidate), 0);
    ll_free(tmp);

    /* Calculate candidate sets.  From the single set (3,4,9), we expect
     * to get nothing. */
    tmp = generate_candidate_sets(candidates);
    ll_free(candidates);
    candidates = tmp;
    ll_sort(candidates);
    assert_int_equal(ll_length(candidates), 0);

    /* Clean up. */
    ll_free(candidates);
}


void test_generate_candidate_sets_b(void **state)
{
    List *candidates;
    List *candidate;
    List *tmp;

    /* Create a seed of size three sets. */
    candidates = ll_create(uint16_list_compare, uint16_list_copy,
            uint16_list_free);
    ll_push_tail(candidates, uint16_list_create(3, 1, 2, 3));
    ll_push_tail(candidates, uint16_list_create(3, 1, 2, 4));
    ll_push_tail(candidates, uint16_list_create(3, 1, 3, 4));
    ll_push_tail(candidates, uint16_list_create(3, 1, 3, 5));
    ll_push_tail(candidates, uint16_list_create(3, 2, 3, 4));

    /* Calculate candidate sets.  Expeting (1,2,3,4).  The set (1,3,4,5)
     * should be pruned since the subset of it (1,4,5) and others are
     * not in the candidates. */
    tmp = generate_candidate_sets(candidates);
    ll_free(candidates);
    candidates = tmp;
    ll_sort(candidates);
    assert_int_equal(ll_length(candidates), 1);

    /* Verify the the expected sets were generated. */
    tmp = uint16_list_create(4, 1, 2, 3, 4);
    candidate = (List *)ll_get_nth(candidates, 0);
    assert_int_equal(ll_list_compare(tmp, candidate), 0);
    ll_free(tmp);

    /* Clean up. */
    ll_free(candidates);
}


void test_build_hashtree(void **state)
{
    List *keys;
    Hashtree *tree;

    /* Create a collection of keys. */
    keys = ll_create(uint16_list_compare, uint16_list_copy,
            uint16_list_free);
    ll_push_tail(keys, uint16_list_create(3, 1, 2, 3));
    ll_push_tail(keys, uint16_list_create(3, 1, 2, 4));
    ll_push_tail(keys, uint16_list_create(3, 1, 3, 4));
    ll_push_tail(keys, uint16_list_create(3, 1, 3, 5));
    ll_push_tail(keys, uint16_list_create(3, 2, 3, 4));
    ll_push_tail(keys, uint16_list_create(3, 2, 4, 5));

    /* Encode keys in a hash table. */
    tree = build_hashtree(keys);

    assert(tree->root_node->type == BODY);
    assert_int_equal(ht_num_entries(tree->root_node->body_table), 2);

    tree_free(tree);
    ll_free(keys);
}


void test_tree_mark_subsets(void **state)
{
    List *keys;
    List *transaction;
    List *fingerprint;
    List *expected;
    uint16_t i;
    Hashtree *tree;

    /* Create a collection of keys. */
    keys = ll_create(uint16_list_compare, uint16_list_copy,
            uint16_list_free);
    ll_push_tail(keys, uint16_list_create(3, 1, 2, 3));
    ll_push_tail(keys, uint16_list_create(3, 1, 2, 4));
    ll_push_tail(keys, uint16_list_create(3, 1, 3, 4));
    ll_push_tail(keys, uint16_list_create(3, 1, 3, 5));
    ll_push_tail(keys, uint16_list_create(3, 2, 3, 4));
    ll_push_tail(keys, uint16_list_create(3, 2, 4, 5));

    /* Encode keys in a hash table. */
    tree = build_hashtree(keys);

    /* Mark keys that are the subset of transaction. */
    transaction = uint16_list_create(5, 1, 2, 3, 5, 6);
    tree_mark_subsets(tree, transaction);

    /* Verify marking.  */
    expected = uint16_list_create(6, 0, 0, 1, 0, 0, 1);
    fingerprint = tree_fingerprint(tree);
    assert_int_equal(ll_length(expected), ll_length(fingerprint));
    for (i = 0; i < ll_length(expected); i++)
    {
        uint16_t expected_count;
        uint16_t fingerprint_count;

        expected_count = *(uint16_t *)ll_get_nth(expected, i);
        fingerprint_count = *(uint16_t *)ll_get_nth(fingerprint, i);
        assert_int_equal(expected_count, fingerprint_count);
    }

    ll_free(fingerprint);
    ll_free(expected);
    ll_free(transaction);
    tree_free(tree);
    ll_free(keys);
}


void test_tree_extract_frequent(void **state)
{
    List *keys;
    List *transaction;
    List *frequent;
    List *key;
    Hashtree *tree;

    /* Create a collection of keys. */
    keys = ll_create(uint16_list_compare, uint16_list_copy,
            uint16_list_free);
    ll_push_tail(keys, uint16_list_create(3, 1, 2, 3));
    ll_push_tail(keys, uint16_list_create(3, 1, 2, 4));
    ll_push_tail(keys, uint16_list_create(3, 1, 3, 4));
    ll_push_tail(keys, uint16_list_create(3, 1, 3, 5));
    ll_push_tail(keys, uint16_list_create(3, 2, 3, 4));
    ll_push_tail(keys, uint16_list_create(3, 2, 4, 5));

    /* Encode keys in a hash table. */
    tree = build_hashtree(keys);

    /* Mark keys that are the subset of transaction. */
    transaction = uint16_list_create(5, 1, 2, 3, 5, 6);
    tree_mark_subsets(tree, transaction);

    /* Extract frequent keys. */
    frequent = tree_extract_frequent(tree, 1);
    assert_int_equal(ll_length(frequent), 2);

    key = uint16_list_create(3, 1, 2, 3);
    assert_true(ll_search(frequent, key) != NULL);
    ll_free(key);

    key = uint16_list_create(3, 1, 3, 5);
    assert_true(ll_search(frequent, key) != NULL);
    ll_free(key);

    ll_free(frequent);
    ll_free(transaction);
    tree_free(tree);
    ll_free(keys);
}


/* The full monty. */
void test_apriori(void **state)
{
    FILE *fid;
    List *frequent;
    float ratio;

    /* Calculate frequent sets for an input file. */
    fid = fopen(tmp_file_name, "w");
    fprintf(fid, "1 1 2 3 5 8 1 3");
    fclose(fid);

    ratio = 1.0;
    frequent = apriori(tmp_file_name, 4, ratio);
    printf("Printing list for ratio %1.02f...\n", ratio);
    uint16_list_list_print(frequent);
    printf("Printing done...\n");
    assert(ll_length(frequent) == 1);
    ll_free(frequent);

    /*
    ratio = 0.8;
    frequent = apriori(tmp_file_name, 4, ratio);
    printf("Printing list for ratio %1.02f...\n", ratio);
    uint16_list_list_print(frequent);
    printf("Printing done...\n");
    assert(ll_length(frequent) == 5);
    ll_free(frequent);

    ratio = 0.6;
    frequent = apriori(tmp_file_name, 4, ratio);
    printf("Printing list for ratio %1.02f...\n", ratio);
    uint16_list_list_print(frequent);
    printf("Printing done...\n");
    assert(ll_length(frequent) == 14);
    ll_free(frequent);

    ratio = 0.4;
    frequent = apriori(tmp_file_name, 4, ratio);
    printf("Printing list for ratio %1.02f...\n", ratio);
    uint16_list_list_print(frequent);
    printf("Printing done...\n");
    assert(ll_length(frequent) == 20);
    ll_free(frequent);

    ratio = 0.2;
    frequent = apriori(tmp_file_name, 4, ratio);
    printf("Printing list for ratio %1.02f...\n", ratio);
    uint16_list_list_print(frequent);
    printf("Printing done...\n");
    assert(ll_length(frequent) == 32);
    ll_free(frequent);
    */

    remove(tmp_file_name);
}


int main(int argc, char* argv[]) {
    const UnitTest tests[] = {
        unit_test(test_read_uint16_list),
        unit_test(test_make_transactions_fixed_width),
        unit_test(test_generate_frequent_size_one),
        unit_test(test_generate_candidate_sets_a),
        unit_test(test_generate_candidate_sets_b),
        unit_test(test_build_hashtree),
        unit_test(test_tree_mark_subsets),
        unit_test(test_tree_extract_frequent),
        unit_test(test_apriori),
    };
    return run_tests(tests);
}
