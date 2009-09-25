#ifndef HASHTREE_
#define HASHTREE_

#include <stdint.h>
#include "linked_list.h"

/* NOTE: This data structure is not a hash tree in the sense of a tiger
 * tree, where a node is the hash of its children.  This data structure
 * is a tree where interior nodes represent parts of a key made up of
 * inidvidual elements.  Each level of the tree reflects hashing on the
 * next such element to select the child node.
 */

typedef struct _tree_tree Hashtree;

Hashtree* tree_create(uint16_t threshold,
        uint16_t(*hash_function)(uint16_t),
        void*(*deep_copy)(void*),
        void(*free_data)(void*));

void tree_insert(Hashtree *tree, Node *key);

void* tree_search(Hashtree *tree, Node *key);

void tree_free(Hashtree *tree);

#endif /* HASHTREE_ */
