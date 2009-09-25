#include <stdlib.h>
#include <assert.h>

#include "hashtree_private.h"

#ifdef UNIT_TESTING
#include "../test/unit_testing.h"
#endif /* UNIT_TESTING */

static void free_tree_node(void* tree_node)
{
    ht_free(((TreeNode *)tree_node)->table);
    free(tree_node);
    return;
}




Hashtree* tree_create(uint16_t threshold,
        uint16_t(*hash_function)(uint16_t),
        void*(*deep_copy)(void*),
        void(*free_data)(void*))
{
    Hashtree *tree;
    tree = malloc(sizeof(Hashtree));
    assert(tree != 0);

    tree->threshold = threshold;
    tree->hash_function = hash_function;
    tree->deep_copy = deep_copy;
    tree->free_data = free_data;

    /* Initialize the root node. */
    tree->root = malloc(sizeof(TreeNode));
    assert(tree->root != NULL);
    tree->root->key = ROOT_KEY;
    tree->root->depth = 0;
    tree->root->type = LEAF;
    tree->root->parent = NULL;

    /* TODO: Evaluate what a good table size should be based off of a
     * requested threshold.  This will probably depend on if the
     * hashtree is located on a PC or embedded device. */
    tree->root->table = ht_create(threshold, hash_function, deep_copy, free_data);

    return tree;
}


static TreeNode* find_leaf(TreeNode *node, Node *key_list)
{
    return node;
}


static inline uint16_t get_nth_key(Node* key_list, uint8_t index)
{
    uint16_t *key;

    key = (uint16_t *)ll_get_nth(key_list, index);
    return *key;
}


static void expand_node(TreeNode **node)
{}

void tree_insert(Hashtree *tree, Node *key_list, void* data)
{
    TreeNode *leaf;

    leaf = find_leaf(tree->root, key_list);
    ht_insert(leaf->table, get_nth_key(key_list, leaf->depth), data);
    if (ht_num_entries(leaf->table) > tree->threshold)
        expand_node(&leaf);
}

void* tree_search(Hashtree *tree, Node *key_list);

void tree_free(Hashtree *tree)
{
    ht_free(tree->root->table);
    free(tree->root);
    free(tree);
}
