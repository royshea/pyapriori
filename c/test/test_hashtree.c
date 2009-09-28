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

    /* XOR hashes of individual keys together to generate a single hash
     * for a list of keys.
     *
     * TODO: XOR is not a great way to combine hashes, but it works for
     * now.
     */

    hash = 0;
    for (i=0; i<ll_length(key_list); i++)
    {
        data = ll_get_nth(key_list, i);
        hash ^= uint16_hash(data);
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
    uint16_t *key;
    uint16_t *count;

    tree = NULL;
    tree = tree_create(5, uint16_hash, uint16_list_hash,
            uint16_compare, uint16_copy, uint16_free,
            uint16_compare, uint16_copy, uint16_free);
    assert_true(tree != NULL);

    /* Insert the key 0 1 2 */
    key_list = ll_create(uint16_compare, uint16_copy, uint16_free);
    key = malloc(sizeof(uint16_t));
    *key = 0;
    ll_push_tail(key_list, key);
    key = malloc(sizeof(uint16_t));
    *key = 1;
    ll_push_tail(key_list, key);
    key = malloc(sizeof(uint16_t));
    *key = 2;
    ll_push_tail(key_list, key);
    count = malloc(sizeof(uint16_t));
    *count = 1;
    tree_insert(tree, key_list, count);

    /* Insert the key 1 2 3 */
    key_list = ll_create(uint16_compare, uint16_copy, uint16_free);
    key = malloc(sizeof(uint16_t));
    *key = 1;
    ll_push_tail(key_list, key);
    key = malloc(sizeof(uint16_t));
    *key = 2;
    ll_push_tail(key_list, key);
    key = malloc(sizeof(uint16_t));
    *key = 3;
    ll_push_tail(key_list, key);
    count = malloc(sizeof(uint16_t));
    *count = 1;
    tree_insert(tree, key_list, count);

    /* Insert the key 0 1 3 */
    key_list = ll_create(uint16_compare, uint16_copy, uint16_free);
    key = malloc(sizeof(uint16_t));
    *key = 0;
    ll_push_tail(key_list, key);
    key = malloc(sizeof(uint16_t));
    *key = 1;
    ll_push_tail(key_list, key);
    key = malloc(sizeof(uint16_t));
    *key = 3;
    ll_push_tail(key_list, key);
    count = malloc(sizeof(uint16_t));
    *count = 1;
    tree_insert(tree, key_list, count);

    /* Insert the duplicate key 0 1 3 */
    key_list = ll_create(uint16_compare, uint16_copy, uint16_free);
    key = malloc(sizeof(uint16_t));
    *key = 0;
    ll_push_tail(key_list, key);
    key = malloc(sizeof(uint16_t));
    *key = 1;
    ll_push_tail(key_list, key);
    key = malloc(sizeof(uint16_t));
    *key = 3;
    ll_push_tail(key_list, key);
    count = malloc(sizeof(uint16_t));
    *count = 1;
    expect_assert_failure(tree_insert(tree, key_list, count));
    ll_free(key_list);
    uint16_free(count);

    /* Clean up. */
    tree_free(tree);

    /*
    uint16_t *sub_key;
    Node **table;
    Entry *hash_one_entry;
    Node *hash_one_keys;
    Node *stored_key_list;
    */

    /*
    tree_insert(tree, key_list);
    assert_true(tree->root_node->table->count == 1);

    table = tree->root_node->table->table;
    assert_true(table[0] == NULL);
    assert_true(table[1] != NULL);
    assert_true(table[1]->next == NULL);
    assert_true(table[2] == NULL);
    assert_true(table[3] == NULL);

    hash_one_keys = table[1];
    hash_one_entry = hash_one_keys->node_data;
    stored_key_list = hash_one_entry->entry_data;
    assert_true(*(uint16_t *)stored_key_list->node_data == 1);
    assert_true(stored_key_list->next != NULL);
    assert_true(*((uint16_t*)stored_key_list->next->node_data) == 0);

    key_list = NULL;
    sub_key = malloc(sizeof(uint16_t));
    *sub_key = 0;
    ll_push(&key_list, sub_key);
    sub_key = malloc(sizeof(uint16_t));
    *sub_key = 2;
    ll_push(&key_list, sub_key);
    tree_insert(tree, key_list);

    key_list = NULL;
    sub_key = malloc(sizeof(uint16_t));
    *sub_key = 0;
    ll_push(&key_list, sub_key);
    sub_key = malloc(sizeof(uint16_t));
    *sub_key = 3;
    ll_push(&key_list, sub_key);
    tree_insert(tree, key_list);
    */

    /*
    key_list = NULL;
    sub_key = malloc(sizeof(uint16_t));
    *sub_key = 1;
    ll_push(&key_list, sub_key);
    sub_key = malloc(sizeof(uint16_t));
    *sub_key = 3;
    ll_push(&key_list, sub_key);
    tree_insert(tree, key_list);
    */

}


void test_tree_search(void **state)
{}


void test_tree_free(void **state)
{}


int main(int argc, char* argv[]) {
    const UnitTest tests[] = {
        unit_test(test_tree_create),
        unit_test(test_tree_insert),
        unit_test(test_tree_search),
        unit_test(test_tree_free),
    };
    return run_tests(tests);
}

