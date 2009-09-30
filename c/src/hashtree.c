#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

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
#if 0
static void *key_list_copy(void *data)
{
    return ll_copy((List *)data);
}


/* Generic function used to free key lists. */
static void key_list_free(void *data)
{
    ll_free((List *)data);
}
#endif

/* Sort based on the keys in a key data tuple. */
static int16_t key_data_compare(void *key_data_a, void *key_data_b)
{
    KeyData *kd_a;
    KeyData *kd_b;
    kd_a = (KeyData *)key_data_a;
    kd_b = (KeyData *)key_data_b;
    return ll_list_compare(kd_a->key, kd_b->key);
}


/* Create a copy of a KeyData tuple. */
static void *key_data_copy(void *key_data)
{
    KeyData *kd;
    KeyData *copy;

    kd = (KeyData *)key_data;
    copy = malloc(sizeof(KeyData));
    copy->key = kd->parent_tree->key_copy(kd->key);
    copy->data = kd->parent_tree->data_copy(kd->data);
    copy->parent_tree = kd->parent_tree;

    return copy;
}


/* Free both key and data in a key data tuple. */
static void key_data_free(void *key_data)
{
    KeyData *kd;
    kd = (KeyData *)kd;
    ll_free(kd->key);
    kd->parent_tree->data_free(kd->data);
    free(kd);
    return;
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
    tn->body_table = NULL;

    /* Create a list to store key_list / data pairs. */
    tn->leaf_list = ll_create(key_data_compare, key_data_copy,
            key_data_free);

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
        next_node = ht_search(node->body_table, key);

        if (next_node == NULL)
        {
            void *key_copy;

            /* If branch has not been explored then create a new leaf. */
            next_node = create_tree_leaf_node(node->parent_tree, node, key);
            key_copy = node->parent_tree->key_copy(key);
            ht_insert(node->body_table, key_copy, next_node);
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
    KeyData *kd;

    /* Locate the leaf that should house key_list. */
    leaf = find_leaf(node, key_list);

    /* Check if key_list is already present in the hash tree. */
    old_data = ll_search(leaf->leaf_list, key_list);
    assert(old_data == NULL);

    /* Insert data into the table if key_list is not already
     * present. */
    kd = malloc(sizeof(KeyData));
    kd->key = key_list;
    kd->data = data;
    ll_push(leaf->leaf_list, kd);
    if (ll_length(leaf->leaf_list) > leaf->parent_tree->threshold)
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
    if (node->body_table)
        ht_free(node->body_table);
    if (node->leaf_list)
        ll_free(node->leaf_list);
    free(node);
    return;
}


/* Expand a leaf node into a body node. */
static void expand_node(TreeNode *node)
{
    Hashtree *ht;
    KeyData *kd;

    ht = node->parent_tree;

    node->type = BODY;

    /* Create a hashtable to store other TreeNodes.
     *
     * TODO: Setting the body hash table size hint to threshold.
     * Threshold is really being used to control how bushy the leaf
     * nodes are.  Size of body nodes can be separate from this. */
    node->body_table = ht_create(ht->threshold, ht->hash_key,
            ht->key_compare, ht->key_copy, ht->key_free,
            NULL, NULL, tree_node_free);

    /* NOTE: Currently requires internal access to the hash table.  It
     * would be good to create an iterator for the hash table to avoid
     * this violation of the data type abstraction.
     */
    for (kd = ll_pop(node->leaf_list); kd != NULL;
            kd = ll_pop(node->leaf_list))
    {
        /* Rehash each entry. */
        insert_to_leaf(node, kd->key, kd->data);
        free(kd);
    }

    ll_free(node->leaf_list);
    node->leaf_list = NULL;
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
    return ht_search(leaf->body_table, key_list);
}


/* Free the hash tree. */
void tree_free(Hashtree *tree)
{
    ht_free(tree->root_node->body_table);
    free(tree->root_node);
    ll_free(tree->key_list_type);
    free(tree);
}


static void print_indent(uint8_t depth)
{
    for(; depth>0; depth--)
        printf("  ");
    return;
}

static void print_leaf_node(TreeNode *node)
{
    uint16_t i;
    uint16_t j;

    print_indent(node->depth);
    printf(".\n");
    for (i=0; i<ll_length(node->leaf_list); i++)
    {

        KeyData *kd;
        kd = ll_get_nth(node->leaf_list, i);
        print_indent(node->depth + 1);
        for (j = 0; j < ll_length(kd->key); j++)
        {
            uint16_t *key;
            key = ll_get_nth(kd->key, j);
            printf("%d ", *key);
        }
        printf("\n");
    }
}


static void print_node(TreeNode *node)
{
    uint16_t i;
    uint16_t j;

    if (node->type == LEAF)
        print_leaf_node(node);
    else
    {
        for (i=0; i<node->body_table->size; i++)
        {
            List *bucket;
            bucket = node->body_table->buckets[i];

            for (j = 0; j < ll_length(bucket); j++)
            {
                Entry *e;
                uint16_t *key;

                e = (Entry *)ll_get_nth(bucket, j);
                key = e->key;

                print_indent(node->depth + 1);
                printf("%d\n", *key);
                print_leaf_node((e->entry_data));
            }
        }
    }
}


#if 0
static void mark_subsets_helper(TreeNode *node, List *key_list,
        uint8_t key_index)
{
    if (node->type == LEAF)
    {
        List *candidates;
        void *key;

        /* Mark leaf sets are contained within the key.
         *
         * TODO: Leaf nodes actually contain one additional level of
         * hashing.  That requires the extra level of processing in this
         * function.  An alternate is to revise the hash_tree
         * implementation to have LEAF nodes contain a linked list,
         * rather than a hash table. */
        key = ll_get_nth(key_list, key_index);
        candidates = ht_search(node->body_table, key);
        if (next_node == NULL)
            return;
        else
        {
            List *candidate;
            uint8_t i;
            for (i = 0; i < ll_len(candidates); i++)
            {
                candidate = ll_get_nth(candidates);
                if (ll_subset(key_list, candidate) == TRUE)
                {

                }
            }
        }
    }
    else
    {
        uint8_t i;

        /* Recursivly mark_subsets for subsequent key values. */
        for (i = key_index; i < ll_length(key_list); i++)
        {
            void *key;
            TreeNode *next_node;

            /* Descend one step deeper into the tree. */
            key = ll_get_nth(key_list, i);
            next_node = ht_search(node->body_table, key);

            if (next_node == NULL)
                return;
            else
                mark_subsets_helper(next_node, key_list, i);
        }
    }
    return;
}

/* Return true if list is a subset of one of the lists contained within
 * the hashtree tree.   See agrawal94fast section 2.1.2 for details.
 *
 * NOTE: Assumes that all lists were sorted before insertion into the
 * hashtree. */
uint8_t mark_subsets(Hashtree *tree, List *key)
{
    return is_subset_helper(tree->root_node, key, 0);
}
#endif

/* Print the contents of the hash tree. */
void tree_print(Hashtree *tree)
{
    printf("ROOT\n");
    print_node(tree->root_node);
    printf("\n\n");
}
