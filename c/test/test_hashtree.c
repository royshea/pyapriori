#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <google/cmockery.h>

#include "hashtree.h"
#include "hashtree_private.h"
#include "linked_list.h"

#include "unit_testing.h"


/* Utility functions used to construct a hash table using integer keys
 * and data. */

uint16_t uint16_hash(void *key)
{
    return *(uint16_t *)key;
}


uint16_t uint16_list_hash(void *key)
{
    List* key_list;
    uint16_t hash;
    void *data;
    uint16_t i;

    key_list = (List *)key;

    /* XOR and shift hashes of individual keys together to generate a
     * single hash for a list of keys.
     *
     * TODO: XOR is not a great way to combine hashes, but it works for
     * now.
     */

    hash = 0;
    for (i=0; i<ll_length(key_list); i++)
    {
        data = ll_get_nth(key_list, i);
        hash ^= uint16_hash(data);
        hash <<= 1;
    }

    return hash;
}



void uint16_free(void* int_data)
{
    free(int_data);
    return;
}


void* uint16_copy(void* int_data)
{
    uint16_t *data;
    data = malloc(sizeof(uint16_t));
    *data = *(uint16_t *)int_data;
    return data;
}


int16_t uint16_compare(void *int_a, void *int_b)
{
    return *(uint16_t *)int_a - *(uint16_t *)int_b;
}


/* Create a linked list storing three uint16_t values. */
List *make_key_list(int length, ...)
{
    List *list;
    va_list ap;
    int i;
    uint16_t *data;

    list = ll_create(uint16_compare, uint16_copy, uint16_free);
    va_start(ap, length);
    for (i=0; i<length; i++)
    {
        data = malloc(sizeof(uint16_t));
        *data = (uint16_t)va_arg(ap, int);
        ll_push_tail(list, data);
    }
    va_end(ap);
    return list;
}


/* Allocate and initialize an uint16_t. */
uint16_t *make_count(uint16_t start_val)
{
    uint16_t *count;
    count = malloc(sizeof(uint16_t));
    *count = start_val;
    return count;
}

/* Tests! */

void test_tree_create(void **state)
{
    Hashtree *tree;

    tree = NULL;
    tree = tree_create(5, uint16_hash, uint16_list_hash,
            uint16_compare, uint16_copy, uint16_free,
            uint16_compare, uint16_copy, uint16_free);
    assert_true(tree != NULL);

    /* Verify that the root node is created properly. */
    assert_true(tree->root_node != NULL);
    assert_true(tree->root_node->parent_tree == tree);
    assert_true(tree->root_node->type == LEAF);
    assert_true(tree->root_node->parent_node == NULL);
    assert_true(tree->root_node->depth == 0);
    assert_true(tree->root_node->key == NULL);
    assert_true(tree->root_node->table != NULL);

    tree_free(tree);
}


void test_tree_insert(void **state)
{
    Hashtree *tree;
    List *key_list;
    uint16_t *tmp_count;

    tree = tree_create(5, uint16_hash, uint16_list_hash,
            uint16_compare, uint16_copy, uint16_free,
            uint16_compare, uint16_copy, uint16_free);

    key_list = make_key_list(3, 0, 1, 2);
    tree_insert(tree, key_list, make_count(1));

    key_list = make_key_list(3, 1, 2, 3);
    tree_insert(tree, key_list, make_count(1));

    key_list = make_key_list(3, 0, 1, 3);
    tree_insert(tree, key_list, make_count(1));

    /* Attempt to insert a duplicate key. */
    key_list = make_key_list(3, 0, 1, 3);
    tmp_count = make_count(1);
    expect_assert_failure(tree_insert(tree, key_list, tmp_count));
    uint16_free(tmp_count);
    ll_free(key_list);

    key_list = make_key_list(3, 1, 1, 3);
    tree_insert(tree, key_list, make_count(1));

    key_list = make_key_list(3, 1, 1, 1);
    tree_insert(tree, key_list, make_count(1));

    /* Assuming a node threshold of 5, this insertion cases the root
     * node to be expanded. */
    key_list = make_key_list(3, 3, 1, 1);
    tree_insert(tree, key_list, make_count(1));

    /* Clean up. */
    tree_free(tree);
}


void test_tree_search(void **state)
{
    Hashtree *tree;
    List *key_list;
    List *query_list;
    uint16_t *data;

    tree = NULL;
    tree = tree_create(5, uint16_hash, uint16_list_hash,
            uint16_compare, uint16_copy, uint16_free,
            uint16_compare, uint16_copy, uint16_free);

    /* Populate the hash tree. */
    key_list = make_key_list(3, 0, 1, 2);
    tree_insert(tree, key_list, make_count(1));

    key_list = make_key_list(3, 1, 2, 3);
    tree_insert(tree, key_list, make_count(1));

    key_list = make_key_list(3, 0, 1, 3);
    tree_insert(tree, key_list, make_count(1));

    key_list = make_key_list(3, 1, 1, 3);
    tree_insert(tree, key_list, make_count(1));

    key_list = make_key_list(3, 1, 1, 1);
    tree_insert(tree, key_list, make_count(1));

    key_list = make_key_list(3, 3, 1, 1);
    tree_insert(tree, key_list, make_count(1));


    /* Search for inserted values. */
    query_list = make_key_list(3, 0, 1, 2);
    data = tree_search(tree, query_list);
    assert_true(data != NULL);
    assert_int_equal(*data, 1);
    ll_free(query_list);

    query_list = make_key_list(3, 1, 2, 3);
    assert_true(tree_search(tree, query_list) != NULL);
    ll_free(query_list);

    query_list = make_key_list(3, 0, 1, 3);
    assert_true(tree_search(tree, query_list) != NULL);
    ll_free(query_list);

    query_list = make_key_list(3, 1, 1, 3);
    assert_true(tree_search(tree, query_list) != NULL);
    ll_free(query_list);

    query_list = make_key_list(3, 1, 1, 1);
    assert_true(tree_search(tree, query_list) != NULL);
    ll_free(query_list);

    query_list = make_key_list(3, 9, 9, 9);
    assert_true(tree_search(tree, query_list) == NULL);
    ll_free(query_list);

    /* Clean up. */
    tree_free(tree);
}


void test_tree_free(void **state)
{
    Hashtree *tree;
    List *key_list;

    /* Populate tree. */
    tree = tree_create(5, uint16_hash, uint16_list_hash,
            uint16_compare, uint16_copy, uint16_free,
            uint16_compare, uint16_copy, uint16_free);
    key_list = make_key_list(3, 0, 1, 2);
    tree_insert(tree, key_list, make_count(1));
    key_list = make_key_list(3, 1, 2, 3);
    tree_insert(tree, key_list, make_count(1));
    key_list = make_key_list(3, 0, 1, 3);
    tree_insert(tree, key_list, make_count(1));
    key_list = make_key_list(3, 1, 1, 3);
    tree_insert(tree, key_list, make_count(1));
    key_list = make_key_list(3, 1, 1, 1);
    tree_insert(tree, key_list, make_count(1));
    key_list = make_key_list(3, 3, 1, 1);
    tree_insert(tree, key_list, make_count(1));

    /* Free the tree. */
    tree_free(tree);
}


void test_tree_print(void **state)
{
    Hashtree *tree;
    List *key_list;

    tree = tree_create(5, uint16_hash, uint16_list_hash,
            uint16_compare, uint16_copy, uint16_free,
            uint16_compare, uint16_copy, uint16_free);
    tree_print(tree);

    key_list = make_key_list(3, 0, 1, 2);
    tree_insert(tree, key_list, make_count(1));
    tree_print(tree);

    key_list = make_key_list(3, 1, 2, 3);
    tree_insert(tree, key_list, make_count(1));
    tree_print(tree);

    key_list = make_key_list(3, 0, 1, 3);
    tree_insert(tree, key_list, make_count(1));
    tree_print(tree);

    key_list = make_key_list(3, 1, 1, 3);
    tree_insert(tree, key_list, make_count(1));
    tree_print(tree);

    key_list = make_key_list(3, 1, 1, 1);
    tree_insert(tree, key_list, make_count(1));
    tree_print(tree);

    /* Assuming a node threshold of 5, this insertion cases the root
     * node to be expanded. */
    key_list = make_key_list(3, 3, 1, 1);
    tree_insert(tree, key_list, make_count(1));
    tree_print(tree);

    /* Clean up. */
    tree_free(tree);
}


int main(int argc, char* argv[]) {
    const UnitTest tests[] = {
        unit_test(test_tree_create),
        unit_test(test_tree_insert),
        unit_test(test_tree_search),
        unit_test(test_tree_free),
        /*unit_test(test_tree_print),*/
    };
    return run_tests(tests);
}
