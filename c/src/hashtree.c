#include <stdlib.h>
#include <assert.h>

#include "hashtree_private.h"

#ifdef UNIT_TESTING
#include "../test/unit_testing.h"
#endif /* UNIT_TESTING */

/* Create a tree node. */
static TreeNode* create_tree_node(Hashtree *ht, TreeNode *parent,
        uint16_t key)
{
    TreeNode *tn;

    tn = malloc(sizeof(TreeNode));
    assert(tn != NULL);

    tn->key = key;
    tn->depth = parent == NULL ? 0 : parent->depth + 1;
    tn->type = LEAF;
    tn->parent = parent;

    /* TODO: Evaluate what a good table size should be based off of a
     * requested threshold.  This will probably depend on if the
     * hashtree is located on a PC or embedded device. */
    tn->table = ht_create(ht->threshold, ht->hash_function,
            ht->deep_copy, ht->free_data);

    return tn;
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
    tree->root_node = create_tree_node(tree, NULL, ROOT_KEY);

    return tree;
}


static inline uint16_t get_nth_key(Node* key_list, uint8_t index)
{
    uint16_t *key;
    key = (uint16_t *)ll_get_nth(key_list, index);
    return *key;
}


static TreeNode* find_leaf(Hashtree *ht, TreeNode *node, Node *key_list)
{
    uint16_t key;
    TreeNode *next_node;

    /* Stop when a leaf node is found. */
    if (node->type == LEAF)
        return node;

    /* Descend tree until a leaf node is found. */
    while (key_list)
    {
        /* Descend one step deeper into the tree. */
        key = get_nth_key(key_list, node->depth);
        next_node = ht_search(node->table, key);


        if (next_node == NULL)
        {
            /* If branch has not been explored then create a new leaf. */
            next_node = create_tree_node(ht, node, key);
            ht_insert(node->table, key, next_node);
            return next_node;
        }
        else if (next_node->type == LEAF)
        {
            /* If the branch leads to a leaf, return the leaf. */
            return next_node;
        }
        /* Else continue through next iteration of the loop. */
        node = next_node;

    }

    /* Assume that key is longer than the depth of the tree. */
    assert(FALSE);
    return NULL;
}


static void expand_node(TreeNode **node)
{}


void tree_insert(Hashtree *tree, Node *key_list)
{
    TreeNode *leaf;

    leaf = find_leaf(tree, tree->root_node, key_list);


    ht_insert(leaf->table, get_nth_key(key_list, leaf->depth), key_list);
    if (ht_num_entries(leaf->table) > tree->threshold)
        expand_node(&leaf);
}


void* tree_search(Hashtree *tree, Node *key_list);


void tree_free(Hashtree *tree)
{
    ht_free(tree->root_node->table);
    free(tree->root_node);
    free(tree);
}
