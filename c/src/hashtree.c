#include <stdlib.h>
#include <assert.h>

#include "hashtree_private.h"

/* NOTE: See other NOTEs within this program on advice about removing
 * these includes. */
#include "hashtable_private.h"

#ifdef UNIT_TESTING
#include "../test/unit_testing.h"
#endif /* UNIT_TESTING */


/* Generic function used to copy key lists.
 *
 * TODO: This function has not been tested.
 */
static void *key_list_copy(void *data)
{
    return ll_copy((List *)data);
}


/* Generic function used to free key lists. */
static void key_list_free(void *data)
{
    ll_free((List *)data);
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
        *parent_node, List *key)
{
    TreeNode *tn;
    void *key_copy;

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
        key_copy = ht->key_copy(key);
        tn->key = key_copy;
    }

    /* Create a hashtable to store key_list / data pairs. */
    tn->table = ht_create(ht->threshold, ht->hash_key_list,
            ll_list_compare, key_list_copy, key_list_free,
            ht->data_compare, ht->data_copy, ht->data_free);

    return tn;
}


/* Basic constructor for the hash tree. */
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
    /* Stop when a leaf node is found. */
    if (node->type == LEAF)
        return node;

    /* Descend through BODY nodes tree until a LEAF node is found. */
    while (key_list)
    {
        void *key;
        TreeNode *next_node;

        /* Descend one step deeper into the tree.
         *
         * Currently in a BODY node.  Therefore it is keyed using the
         * individual key_list element, key, rather than the entire
         * key_list. */
        key = ll_get_nth(key_list, node->depth);
        next_node = ht_search(node->table, key);

        if (next_node == NULL)
        {
            void *key_copy;

            /* If branch has not been explored then create a new leaf. */
            next_node = create_tree_leaf_node(node->parent_tree, node, key);
            key_copy = node->parent_tree->key_copy(key);
            ht_insert(node->table, key_copy, next_node);
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


static void expand_node(TreeNode *node);
static void insert_to_leaf(TreeNode *node, List *key_list, void *data)
{
    TreeNode *leaf;
    void *old_data;

    /* Locate the leaf that should house key_list. */
    leaf = find_leaf(node, key_list);

    /* Check if key_list is already present in the hash tree. */
    old_data = ht_search(leaf->table, key_list);
    assert(old_data == NULL);

    /* Insert data into the table if key_list is not already
     * present. */
    ht_insert(leaf->table, key_list, data);
    if (ht_num_entries(leaf->table) > leaf->parent_tree->threshold)
        expand_node(leaf);
}


/* Function used to free tree body nodes. This function is used by
 * expand node to define the free function for BODY nodes. */
static void tree_node_free(void *data)
{
    TreeNode *node;
    Hashtree *ht;

    node = (TreeNode *)data;
    ht = node->parent_tree;
    ht->key_free(node->key);
    ht_free(node->table);
    free(node);
    return;
}


/* Expand a leaf node into a body node. */
static void expand_node(TreeNode *node)
{
    Hashtable *old_table;
    Hashtree *ht;
    uint16_t i;

    ht = node->parent_tree;

    node->type = BODY;

    /* Create a new hash table that maps a single key value down to
     * a lower level of hash tree nodes. */
    old_table = node->table;

    /* Create a hashtable to store other TreeNodes.
     *
     * TODO: Setting the body hash table size hint to threshold.
     * Threshold is really being used to control how bushy the leaf
     * nodes are.  Size of body nodes can be separate from this. */
    node->table = ht_create(ht->threshold, ht->hash_key,
            ht->key_compare, ht->key_copy, ht->key_free,
            NULL, NULL, tree_node_free);

    /* NOTE: Currently requires internal access to the hash table.  It
     * would be good to create an iterator for the hash table to avoid
     * this violation of the data type abstraction.
     */
    for (i=0; i<old_table->size; i++)
    {
        List *bucket;
        Entry *e;

        bucket = old_table->buckets[i];
        if (bucket == NULL)
            continue;

        /* Rehash each entry. */
        for (e = (Entry *)ll_pop(bucket); e != NULL;
                e = (Entry *)ll_pop(bucket))
        {
            insert_to_leaf(node, e->key, e->entry_data);
            free(e);
        }
    }

    ht_free(old_table);
}


/* Insert data into the tree using key_list. */
void tree_insert(Hashtree *tree, List *key_list, void *data)
{
    insert_to_leaf(tree->root_node, key_list, data);
}


/* Search the tree for data referenced by key_list. */
void* tree_search(Hashtree *tree, List *key_list)
{
    TreeNode *leaf;

    /* Locate the leaf that should house key_list and then look up the
     * key_list in the leaf. */
    leaf = find_leaf(tree->root_node, key_list);
    assert(leaf != NULL);
    return ht_search(leaf->table, key_list);
}


/* Free the hash tree. */
void tree_free(Hashtree *tree)
{
    ht_free(tree->root_node->table);
    free(tree->root_node);
    ll_free(tree->key_list_type);
    free(tree);
}
