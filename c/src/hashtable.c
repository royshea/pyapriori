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

#include <stdlib.h>
#include <assert.h>

#include "hashtable_private.h"

#ifdef UNIT_TESTING
#include "../test/unit_testing.h"
#endif /* UNIT_TESTING */


/* Check to see if the key for entry_a matches key.  This type of match
 * is specific to hash tables where elements are looked up based on the
 * key and the actual stored data is ignored.
 */
static int16_t compare_entry(void* entry_a, void* entry_b)
{
    Entry *a;
    Entry *b;
    int16_t(*key_compare)(void *,void *);

    /* Type and unpack important data. */
    a = (Entry *)entry_a;
    b = (Entry *)entry_b;
    key_compare = a->parent_table->key_compare;

    /* Compare on keys from the two entries. */
    return key_compare(a->key, b->key);
}


static void* copy_entry(void* entry)
{
    Entry *e;
    Entry *copy;
    void*(*key_copy)(void*);
    void*(*data_copy)(void*);

    /* Type and unpack important data. */
    e = (Entry *)entry;
    key_copy = e->parent_table->key_copy;
    data_copy = e->parent_table->data_copy;

    /* Free up the fields and entry. */
    copy = malloc(sizeof(Entry));
    copy->key = key_copy(e->key);
    copy->entry_data = data_copy(e->entry_data);
    copy->parent_table = e->parent_table;

    return copy;
}


static void free_entry(void* entry)
{
    Entry *e;
    void(*key_free)(void*);
    void(*data_free)(void*);

    /* Type and unpack important data. */
    e = (Entry *)entry;
    key_free = e->parent_table->key_free;
    data_free = e->parent_table->data_free;

    /* Free up the fields and entry. */
    key_free(e->key);
    data_free(e->entry_data);
    free(e);

    return;
}


Hashtable* ht_create(uint16_t size_hint,
        uint16_t(*hash_function)(void *),
        int16_t(*key_compare)(void *,void *),
        void*(*key_copy)(void*),
        void(*key_free)(void*),
        int16_t(*data_compare)(void *,void *),
        void*(*data_copy)(void*),
        void(*data_free)(void*))

{
    Hashtable *ht;
    uint8_t i;

    /* Find first prime table size larger than that requested. */
    for (i=0; i<prime_count; i++)
    {
        if (prime_table_size[i] >= size_hint)
            break;
    }
    assert(i < prime_count);

    /* Create hash table and initialize fields. */
    ht = malloc(sizeof(Hashtable));
    assert(ht != NULL);

    ht->size = prime_table_size[i];
    ht->count = 0;
    ht->hash_function = hash_function;
    ht->key_compare = key_compare;
    ht->key_copy = key_copy;
    ht->key_free = key_free;
    ht->data_compare = data_compare;
    ht->data_copy = data_copy;
    ht->data_free = data_free;

    /* Initialze buckets that will hold data. Each element in a bucket
     * is an Entry storing the key and data. */
    ht->buckets = malloc(sizeof(List*) * ht->size);
    assert(ht->buckets != NULL);
    for (i=0; i<ht->size; i++)
        ht->buckets[i] = ll_create(compare_entry, copy_entry, free_entry);

    return ht;
}


void ht_insert(Hashtable *ht, void *key, void *data)
{
    Entry *e;
    uint16_t index;

    /* Forbid duplicate keys. */
    assert(ht_search(ht, key) == NULL);

    /* Create entry to insert into table. */
    e = malloc(sizeof(Entry));
    assert(e != NULL);
    e->key = key;
    e->entry_data = data;
    e->parent_table = ht;

    /* Insert entry into the table at correct index. */
    index = ht->hash_function(key) % ht->size;
    ll_push(ht->buckets[index], e);
    ht->count += 1;
}


void* ht_search(Hashtable *ht, void *key)
{
    uint16_t index;
    Entry tmp_entry;
    Entry *match_entry;

    /* Create a dumy entry with correct key to match against. */
    tmp_entry.key = key;
    tmp_entry.entry_data = NULL;
    tmp_entry.parent_table = ht;

    /* Look for matching entry in the table. */
    index = ht->hash_function(key) % ht->size;
    match_entry = ll_search(ht->buckets[index], &tmp_entry);

    /* Return pointer to data if found. */
    if (match_entry == NULL)
        return NULL;
    return match_entry->entry_data;
}


uint16_t ht_num_entries(Hashtable *ht)
{
    return ht->count;
}


void* ht_remove(Hashtable *ht, void *key)
{
    uint16_t index;
    Entry tmp_entry;
    Entry *match_entry;
    void *data;

    /* Create a dumy entry with correct key to match against. */
    tmp_entry.key = key;
    tmp_entry.entry_data = NULL;
    tmp_entry.parent_table = ht;

    /* Look for matching entry in the table. */
    index = ht->hash_function(key) % ht->size;
    match_entry = ll_remove(ht->buckets[index], &tmp_entry);

    /* Return pointer to data if found. */
    if (match_entry == NULL)
        return NULL;

    /* Remove the entry from the table. */
    data = match_entry->entry_data;
    ht->key_free(match_entry->key);
    free(match_entry);
    ht->count -= 1;
    return data;
}


void ht_free(Hashtable *ht)
{
    uint16_t i;

    for (i=0; i<ht->size; i++)
        ll_free(ht->buckets[i]);

    free(ht->buckets);
    free(ht);
}
