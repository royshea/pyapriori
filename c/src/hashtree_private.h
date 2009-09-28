#ifndef HASHTREE_PRIVATE_
#define HASHTREE_PRIVATE_

#include "hashtree.h"
#include "linked_list.h"
#include "hashtable.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

enum NodeType {BODY, LEAF};

struct _tree_tree;

typedef struct _tree_node
{
    /* Link back to the hash tree contaning this node.  This provides
     * access to the function pointers stored by the hash tree. */
    struct _tree_tree *parent_tree;

    /* Data about this particular node. */
    enum NodeType type;
    uint8_t depth;
    void *key;

    /* Tree structure specific information. */
    struct _tree_node *parent_node;
    Hashtable *table;

} TreeNode;


struct _tree_tree
{
    /* Threshold used at interior nodes. */
    uint16_t threshold;

    /* Type specific functions used by table. */
    uint16_t(*hash_key)(void *);
    uint16_t(*hash_key_list)(void *);
    int16_t(*key_compare)(void *,void *);
    void*(*key_copy)(void *);
    void(*key_free)(void *);
    int16_t(*data_compare)(void *,void *);
    void*(*data_copy)(void *);
    void(*data_free)(void *);

    /* Place holder list used to house key constructor / descriptors. */
    List *key_list_type;

    /* Root of the hash tree. */
    TreeNode *root_node;
};

#endif /* HASHTREE_PRIVATE_ */
