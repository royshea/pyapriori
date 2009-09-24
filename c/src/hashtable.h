#ifndef HASH_TABLE_
#define HASH_TABLE_

#include <stdint.h>

typedef struct _ht_table Hashtable;

Hashtable* ht_create(uint16_t size_hint,
        uint16_t(*hash_function)(void*),
        int16_t(*compare)(void*, void *),
        void*(*deep_copy)(void*),
        void(*free_data)(void*),
        void(*free_key)(void*));

void ht_insert(Hashtable *table, void *key, void* data);

void* ht_search(Hashtable *table, void *key);

void* ht_remove(Hashtable *table, void *key);

void ht_free(Hashtable *table);

#endif /* HASH_TABLE_ */
