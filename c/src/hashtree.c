#include <stdlib.h>
#include <assert.h>

#include "hashtree_private.h"
#include "linked_list_private.h"

#ifdef UNIT_TESTING
#include "../test/unit_testing.h"
#endif /* UNIT_TESTING */


static inline void *get_nth_key(List* key_list, uint8_t index)
{
    return ll_get_nth(key_list, index);
}


void key_list_free(void *data)
{
    List *list;
    list = (List *)data;
    ll_free(list);
}

int16_t key_list_compare(void *data_a, void *data_b)
{
    uint16_t i;
    List *list_a;
    List *list_b;
    uint16_t length_a;
    uint16_t length_b;
    void *key_a;
    void *key_b;
    int16_t cmp;

    list_a = (List *)data_a;
    list_b = (List *)data_b;

    length_a = ll_length(list_a);
    length_b = ll_length(list_b);

    for (i=0; i<length_a && i<length_b; i++)
    {
        key_a = ll_get_nth(list_a, i);
        key_b = ll_get_nth(list_b, i);
        cmp = list_a->compare(key_a, key_b);
        if (cmp != 0) return cmp;
    }

    return length_a - length_b;
}


void *key_list_copy(void *data)
{
    List *list;
    return ll_copy(list);
}

/* Create a tree leaf node.
 *
 * Elements in the table of a leaf node are keyed using the entire key
 * list.  This allows for differentiation of elements with common key
 * prefixes.  This is in contrast to the key used to get to the current
 * table was a single key_list element.
 *
 * TODO: Evaluate what a good table size should be based off of a
 * requested threshold.  This will probably depend on if the hashtree is
 * located on a PC or embedded device.
 */
static TreeNode* create_tree_leaf_node(Hashtree *ht, TreeNode
        *parent_node, List *key_list)
{
    TreeNode *tn;

    tn = malloc(sizeof(TreeNode));
    assert(tn != NULL);

    tn->parent_tree = ht;
    tn->type = LEAF;

    tn->parent_node = parent_node;
    if (parent_node == NULL)
    {
        /* Special case for tree root. */
        tn->depth = 0;
        tn->key = NULL;
    }
    else
    {
        /* Non-root leaf nodes. */
        tn->depth = parent_node->depth + 1;
        tn->key = get_nth_key(key_list, tn->depth);
    }

    tn->table = ht_create(ht->threshold, ht->hash_key_list,
            key_list_compare, key_list_copy, key_list_free,
            ht->data_compare, ht->data_copy, ht->data_free);

    return tn;
}


Hashtree* tree_create(uint16_t threshold,
        uint16_t(*hash_key)(void *),
        uint16_t(*hash_key_list)(void *),
        int16_t(*key_compare)(void *,void *),
        void*(*key_copy)(void *),
        void(*key_free)(void *),
        int16_t(*data_compare)(void *,void *),
        void*(*data_copy)(void *),
        void(*data_free)(void *))
{
    Hashtree *tree;
    tree = malloc(sizeof(Hashtree));
    assert(tree != 0);

    tree->threshold = threshold;

    tree->hash_key = hash_key;
    tree->hash_key_list = hash_key_list;
    tree->key_compare = key_compare;
    tree->key_copy = key_copy;
    tree->key_free = key_free;
    tree->data_compare = data_compare;
    tree->data_copy = data_copy;
    tree->data_free = data_free;

    tree->key_list_type = ll_create(key_compare, key_copy, key_free);
    tree->root_node = create_tree_leaf_node(tree, NULL, NULL);

    return tree;
}


/* Traverse a key_list looking for the first leaf node. */
static TreeNode* find_leaf(TreeNode *node, List *key_list)
{
    void *key;
    TreeNode *next_node;

    /* Stop when a leaf node is found. */
    if (node->type == LEAF)
        return node;

    /* Descend through BODY nodes tree until a LEAF node is found. */
    while (key_list)
    {

        /* Descend one step deeper into the tree.
         *
         * Currently in a BODY node.  Therefore it is keyed using the
         * individual key_list element, key, rather than the entire
         * key_list. */
        key = get_nth_key(key_list, node->depth);
        next_node = ht_search(node->table, key);

        if (next_node == NULL)
        {
            /* If branch has not been explored then create a new leaf. */
            next_node = create_tree_leaf_node(node->parent_tree, node, key);
            ht_insert(node->table, key, next_node);
            return next_node;
        }
        else if (next_node->type == LEAF)
        {
            /* If the branch leads to a leaf, return the leaf. */
            return next_node;
        }
        /* Else, descend another level down in the tree. */
        node = next_node;

    }

    /* Assume that key is longer than the depth of the tree. */
    assert(FALSE);
    return NULL;
}


static void expand_node(TreeNode **node)
{
    assert(FALSE);
}


void tree_insert(Hashtree *tree, List *key_list, void *data)
{
    TreeNode *leaf;
    void *old_data;

    leaf = find_leaf(tree->root_node, key_list);

    old_data = ht_search(leaf->table, key_list);

    if (old_data == NULL)
    {
        ht_insert(leaf->table, key_list, data);
        if (ht_num_entries(leaf->table) > tree->threshold)
            expand_node(&leaf);
    }
    else
    {
        /* TODO: This should be revised to update the data stored in the
         * hash tree.  For the apriori application, this update should
         * simply increment old_data. */
        assert(FALSE);
    }
}


void* tree_search(Hashtree *tree, List *key_list)
{
    assert(FALSE);
    return NULL;
}


void tree_free(Hashtree *tree)
{
    ht_free(tree->root_node->table);
    free(tree->root_node);
    ll_free(tree->key_list_type);
    free(tree);
}
