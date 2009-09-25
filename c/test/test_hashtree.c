#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <google/cmockery.h>

#include "hashtree.h"
#include "hashtree_private.h"
#include "hashtable_private.h"
#include "linked_list_private.h"
#include "linked_list.h"

#include "unit_testing.h"


/* Utility functions used to construct a hash tree using integer keys
 * and data. */

uint16_t hash_int(uint16_t key)
{
    return key;
}


void* copy_int(void* int_data)
{
    int *data_copy;
    data_copy = malloc(sizeof(int));
    *data_copy = *(int *)int_data;
    return data_copy;
}


void* copy_key(void* key_list)
{
    Node *key_copy;
    key_copy = ll_copy(key_list, copy_int);
    return key_copy;
}

void free_int(void* int_data)
{
    free(int_data);
    return;
}

void free_key(void* key_list)
{
    Node *n;
    n = key_list;
    ll_free(&n, free_int);
}

/* Tests! */

void test_tree_create(void **state)
{
    Hashtree *tree;

    tree = NULL;
    tree = tree_create(5, hash_int, copy_key, free_key);
    assert_true(tree != NULL);
    assert_true(tree->root_node != NULL);
    assert_true(tree->root_node->key == (uint16_t)-1);
    assert_true(tree->root_node->depth == 0);
    assert_true(tree->root_node->type == LEAF);
    assert_true(tree->root_node->parent == NULL);
    assert_true(tree->root_node->table != NULL);

    tree_free(tree);
}


void test_tree_insert(void **state)
{
    Hashtree *tree;
    Node *key_list;
    uint16_t *sub_key;

    Node **table;
    Entry *hash_one_entry;
    Node *hash_one_keys;
    Node *stored_key_list;

    tree = NULL;
    tree = tree_create(5, hash_int, copy_key, free_key);
    assert_true(tree != NULL);

    key_list = NULL;

    sub_key = malloc(sizeof(uint16_t));
    *sub_key = 0;
    ll_push(&key_list, sub_key);

    sub_key = malloc(sizeof(uint16_t));
    *sub_key = 1;
    ll_push(&key_list, sub_key);

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

    tree_free(tree);
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

