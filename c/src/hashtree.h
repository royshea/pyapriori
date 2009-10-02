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
        uint16_t(*hash_key)(void *),
        uint16_t(*hash_key_list)(void *),
        int16_t(*key_compare)(void *,void *),
        void*(*key_copy)(void *),
        void(*key_free)(void *),
        int16_t(*data_compare)(void *,void *),
        void*(*data_copy)(void *),
        void(*data_free)(void *));

void tree_insert(Hashtree *tree, List *key_list, void *data);

void* tree_search(Hashtree *tree, List *key_list);

void tree_free(Hashtree *tree);

void tree_mark_subsets(Hashtree *tree, List *key_list);

List *tree_extract_frequent(Hashtree *tree, uint16_t min_count);

List *tree_fingerprint(Hashtree *tree);

void tree_print_uint16(Hashtree *tree);

#endif /* HASHTREE_ */
