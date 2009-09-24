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
    current_ht->free_key(((Entry *)entry)->key);
    current_ht->free_data(((Entry *)entry)->data);
    free(entry);
    return;
}


Hashtable* ht_create(uint16_t size_hint,
        uint16_t(*hash_function)(void*),
        int16_t(*compare)(void*, void *),
        void*(*deep_copy)(void*),
        void(*free_data)(void*),
        void(*free_key)(void*)
        )
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
    ht->compare = compare;
    ht->deep_copy = deep_copy;
    ht->free_data = free_data;
    ht->free_key = free_key;

    ht->table = malloc(sizeof(Node*) * ht->size);
    assert(ht->table != NULL);
    for (i=0; i<ht->size; i++)
    {
        ht->table[i] = NULL;
    }

    return ht;
}


void ht_insert(Hashtable *ht, void *key, void* data)
{
    Entry *e;
    uint16_t index;

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


void* ht_search(Hashtable *ht, void *key)
{
    uint16_t index;
    index = ht->hash_function(key) % ht->size;
    return NULL;
}


void* ht_remove(Hashtable *ht, void *key);


void ht_free(Hashtable *ht)
{
    uint8_t i;

    current_ht = ht;
    for (i=0; i<ht->size; i++)
    {
        ll_free(&(ht->table[i]), free_entry);
    }
    current_ht = NULL;

    free(ht->table);
    free(ht);
}
