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

typedef struct
{
    List *key;
    void *data;
    struct _tree_tree *parent_tree;
} KeyData;


typedef struct _tree_node
{
    /* Link back to the hash tree containing this node.  This provides
     * access to the function pointers stored by the hash tree. */
    struct _tree_tree *parent_tree;

    /* Data about this particular node. */
    enum NodeType type;
    uint8_t depth;
    void *key;

    /* Tree structure specific information. */
    struct _tree_node *parent_node;

    /* Hashtable used by body nodes. */
    Hashtable *body_table;

    /* List of keys used by leaf nodes. */
    List *leaf_list;

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
