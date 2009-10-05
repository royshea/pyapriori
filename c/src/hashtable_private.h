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

#ifndef HASHTABLE_PRIVATE_
#define HASHTABLE_PRIVATE_

#include "hashtable.h"
#include "linked_list.h"


/* Prime recomendations from Aaron Krowne
 * http://planetmath.org/encyclopedia/GoodHashTablePrimes.html */
static const uint16_t prime_table_size[] =
{
    5, 11, 23, 53, 97, 193, 389, 769, 1543,
    3079, 6151, 12289, 24593, 49157, 65521
};

static const uint8_t prime_count =
        sizeof(prime_table_size) / sizeof(prime_table_size[0]);

struct _ht_table
{
    /* Basic table statistics. */
    uint16_t size;
    uint16_t count;

    /* Type specific functions used by table. */
    uint16_t (*hash_function) (void *);
    int16_t(*key_compare)(void *,void *);
    void*(*key_copy)(void *);
    void(*key_free)(void *);
    int16_t(*data_compare)(void *,void *);
    void*(*data_copy)(void *);
    void(*data_free)(void *);

    /* Array of linked lists storing table data. */
    List** buckets;

};

typedef struct _ht_entry
{
    void *key;
    void *entry_data;
    struct _ht_table *parent_table;
} Entry;



#endif /* HASHTABLE_PRIVATE_ */
