#include <stdlib.h>
#include <assert.h>

#include "hashtable_private.h"

#ifdef UNIT_TESTING
#include "../test/unit_testing.h"
#endif /* UNIT_TESTING */

static Hashtable *current_ht = NULL;


/* WARNING: Must set current_ht before calling this function.  This is
 * done to provide a generic free for use with the data and key stored
 * within an entry. */
static void free_entry(void* entry)
{
    current_ht->free_data(((Entry *)entry)->data);
    free(entry);
    return;
}


Hashtable* ht_create(uint16_t size_hint,
        uint16_t(*hash_function)(uint16_t),
        void*(*deep_copy)(void*),
        void(*free_data)(void*))
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
    ht->deep_copy = deep_copy;
    ht->free_data = free_data;

    ht->table = malloc(sizeof(Node*) * ht->size);
    assert(ht->table != NULL);
    for (i=0; i<ht->size; i++)
    {
        ht->table[i] = NULL;
    }

    return ht;
}


void ht_insert(Hashtable *ht, uint16_t key, void* data)
{
    Entry *e;
    uint16_t index;

    /* Forbid duplicate keys. */
    assert(ht_search(ht, key) == NULL);

    /* Create entry to insert into table. */
    e = malloc(sizeof(Entry));
    assert(e != NULL);
    e->key = key;
    e->data = data;

    /* Insert entry into the table at correct index. */
    index = ht->hash_function(key) % ht->size;
    ll_push(&(ht->table[index]), e);
    ht->count += 1;
}


static int16_t compare_entry_key(void* entry_a, void* entry_b)
{
    return ((Entry *)entry_a)->key - ((Entry *)entry_b)->key;
}


void* ht_search(Hashtable *ht, uint16_t key)
{
    uint16_t index;
    Entry tmp_entry;
    Entry *match;

    /* Create a dumy entry with correct key to match against. */
    tmp_entry.key = key;

    /* Look for matching entry in the table. */
    index = ht->hash_function(key) % ht->size;
    match = ll_search(ht->table[index], &tmp_entry, compare_entry_key);

    /* Return pointer to data if found. */
    if (match == NULL)
        return NULL;
    return match->data;
}


uint16_t ht_num_entries(Hashtable *ht)
{
    return ht->count;
}


void* ht_remove(Hashtable *ht, uint16_t key)
{
    uint16_t index;
    Entry tmp_entry;
    Entry *match;
    void *data;

    /* Create a dumy entry with correct key to match against. */
    tmp_entry.key = key;

    /* Look for matching entry in the table. */
    index = ht->hash_function(key) % ht->size;
    match = ll_remove(&(ht->table[index]), &tmp_entry, compare_entry_key);

    /* Return pointer to data if found. */
    if (match == NULL)
        return NULL;
    ht->count -= 1;
    data = match->data;
    free(match);
    return data;
}


void ht_free(Hashtable *ht)
{
    Hashtable *prior;
    uint8_t i;

    /* WARNING: Must wrap use of free_entry wuth the setting of
     * current_ht.  See free_entry for more details.
     */
    prior = current_ht;
    current_ht = ht;
    for (i=0; i<ht->size; i++)
    {
        ll_free(&(ht->table[i]), free_entry);
    }
    current_ht = prior;

    free(ht->table);
    free(ht);
}
