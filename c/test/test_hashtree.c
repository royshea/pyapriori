/*
 * "Copyright (c) 2009 The Regents of the University  of California.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written
 * agreement is hereby granted, provided that the above copyright
 * notice, the following two paragraphs and the author appear in all
 * copies of this software.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
 * FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN
 * IF THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
 * CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT,
 * UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
 *
 * Author: Roy Shea (royshea@gmail.com)
 * Date 10/2009
 */

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <google/cmockery.h>

#include "hashtree.h"
#include "hashtree_private.h"
#include "linked_list.h"

#include "uint16_list.h"

#include "unit_testing.h"


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
    assert_true(tree->root_node->body_table == NULL);
    assert_true(tree->root_node->leaf_list != NULL);

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

    key_list = uint16_list_create(3, 0, 1, 2);
    tree_insert(tree, key_list, make_count(1));

    key_list = uint16_list_create(3, 1, 2, 3);
    tree_insert(tree, key_list, make_count(1));

    key_list = uint16_list_create(3, 0, 1, 3);
    tree_insert(tree, key_list, make_count(1));

    /* Attempt to insert a duplicate key. */
    key_list = uint16_list_create(3, 0, 1, 3);
    tmp_count = make_count(1);
    expect_assert_failure(tree_insert(tree, key_list, tmp_count));
    uint16_free(tmp_count);
    ll_free(key_list);

    key_list = uint16_list_create(3, 1, 1, 3);
    tree_insert(tree, key_list, make_count(1));

    key_list = uint16_list_create(3, 1, 1, 1);
    tree_insert(tree, key_list, make_count(1));

    /* Assuming a node threshold of 5, this insertion cases the root
     * node to be expanded. */
    key_list = uint16_list_create(3, 3, 1, 1);
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
    key_list = uint16_list_create(3, 0, 1, 2);
    tree_insert(tree, key_list, make_count(1));

    key_list = uint16_list_create(3, 1, 2, 3);
    tree_insert(tree, key_list, make_count(1));

    key_list = uint16_list_create(3, 0, 1, 3);
    tree_insert(tree, key_list, make_count(1));

    key_list = uint16_list_create(3, 1, 1, 3);
    tree_insert(tree, key_list, make_count(1));

    key_list = uint16_list_create(3, 1, 1, 1);
    tree_insert(tree, key_list, make_count(1));

    key_list = uint16_list_create(3, 3, 1, 1);
    tree_insert(tree, key_list, make_count(1));


    /* Search for inserted values. */
    query_list = uint16_list_create(3, 0, 1, 2);
    data = tree_search(tree, query_list);
    assert_true(data != NULL);
    assert_int_equal(*data, 1);
    ll_free(query_list);

    query_list = uint16_list_create(3, 1, 2, 3);
    assert_true(tree_search(tree, query_list) != NULL);
    ll_free(query_list);

    query_list = uint16_list_create(3, 0, 1, 3);
    assert_true(tree_search(tree, query_list) != NULL);
    ll_free(query_list);

    query_list = uint16_list_create(3, 1, 1, 3);
    assert_true(tree_search(tree, query_list) != NULL);
    ll_free(query_list);

    query_list = uint16_list_create(3, 1, 1, 1);
    assert_true(tree_search(tree, query_list) != NULL);
    ll_free(query_list);

    query_list = uint16_list_create(3, 9, 9, 9);
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
    key_list = uint16_list_create(3, 0, 1, 2);
    tree_insert(tree, key_list, make_count(1));
    key_list = uint16_list_create(3, 1, 2, 3);
    tree_insert(tree, key_list, make_count(1));
    key_list = uint16_list_create(3, 0, 1, 3);
    tree_insert(tree, key_list, make_count(1));
    key_list = uint16_list_create(3, 1, 1, 3);
    tree_insert(tree, key_list, make_count(1));
    key_list = uint16_list_create(3, 1, 1, 1);
    tree_insert(tree, key_list, make_count(1));
    key_list = uint16_list_create(3, 3, 1, 1);
    tree_insert(tree, key_list, make_count(1));

    /* Free the tree. */
    tree_free(tree);
}


void test_tree_print_uint16(void **state)
{
    Hashtree *tree;
    List *key_list;

    tree = tree_create(5, uint16_hash, uint16_list_hash,
            uint16_compare, uint16_copy, uint16_free,
            uint16_compare, uint16_copy, uint16_free);
    tree_print_uint16(tree);

    key_list = uint16_list_create(3, 0, 1, 2);
    tree_insert(tree, key_list, make_count(1));
    tree_print_uint16(tree);

    key_list = uint16_list_create(3, 1, 2, 3);
    tree_insert(tree, key_list, make_count(1));
    tree_print_uint16(tree);

    key_list = uint16_list_create(3, 0, 1, 3);
    tree_insert(tree, key_list, make_count(1));
    tree_print_uint16(tree);

    key_list = uint16_list_create(3, 1, 1, 3);
    tree_insert(tree, key_list, make_count(1));
    tree_print_uint16(tree);

    key_list = uint16_list_create(3, 1, 1, 1);
    tree_insert(tree, key_list, make_count(1));
    tree_print_uint16(tree);

    /* Assuming a node threshold of 5, this insertion cases the root
     * node to be expanded. */
    key_list = uint16_list_create(3, 3, 1, 1);
    tree_insert(tree, key_list, make_count(1));
    tree_print_uint16(tree);

    /* Clean up. */
    tree_free(tree);
}


int main(int argc, char* argv[]) {
    const UnitTest tests[] = {
        unit_test(test_tree_create),
        unit_test(test_tree_insert),
        unit_test(test_tree_search),
        unit_test(test_tree_free),
        /*unit_test(test_tree_print_uint16),*/
    };
    return run_tests(tests);
}
