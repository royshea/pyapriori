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

#ifndef HASHTREE_
#define HASHTREE_

#include <stdint.h>
#include "linked_list.h"

/* NOTE: This data structure is not a hash tree in the sense of a tiger
 * tree, where a node is the hash of its children.  This data structure
 * is a tree where interior nodes represent parts of a key made up of
 * individual elements.  Each level of the tree reflects hashing on the
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
