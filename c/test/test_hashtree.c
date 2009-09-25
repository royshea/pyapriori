#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <google/cmockery.h>

#include "hashtree.h"
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
    Node **n;
    n = (Node **)key_list;
    ll_free(n, free_int);
}

/* Tests! */

void test_tree_create(void **state)
{
    Hashtree *tree;

    tree = NULL;
    tree = tree_create(5, hash_int, copy_key, free_key);
    assert_true(tree != NULL);

    tree_free(tree);
}


void test_tree_insert(void **state)
{
    Hashtree *tree;
    Node *key_list;
    Node *data;
    uint16_t *sub_key;

    tree = NULL;
    tree = tree_create(5, hash_int, copy_key, free_key);
    assert_true(tree != NULL);

    key_list = NULL;
    sub_key = malloc(sizeof(uint16_t));
    *sub_key = 0;
    ll_push(&key_list, sub_key);

    data = ll_copy(key_list, copy_int);
    tree_insert(tree, key_list, data);

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

