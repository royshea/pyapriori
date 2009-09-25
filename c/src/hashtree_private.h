#ifndef HASHTREE_PRIVATE_
#define HASHTREE_PRIVATE_

#include "hashtree.h"
#include "linked_list.h"
#include "hashtable.h"

static const uint16_t ROOT_KEY = -1;

enum NodeType {BODY, LEAF};

typedef struct _tree_node
{
    uint16_t key;
    uint8_t depth;
    enum NodeType type;
    struct _tree_node *parent;

    Hashtable *table;

} TreeNode;


struct _tree_tree
{
    /* Threshold used at interior nodes. */
    uint16_t threshold;

    /* Type specific functions used by table. */
    uint16_t (*hash_function) (uint16_t);
    void* (*deep_copy) (void*);
    void (*free_data) (void*);

    /* Root of the hash tree. */
    TreeNode *root;
};

#endif /* HASHTREE_PRIVATE_ */
