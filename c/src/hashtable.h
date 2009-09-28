#ifndef HASHTABLE_
#define HASHTABLE_

#include <stdint.h>

typedef struct _ht_table Hashtable;

Hashtable* ht_create(uint16_t size_hint,
        uint16_t(*hash_function)(void *),
        int16_t(*key_compare)(void *,void *),
        void*(*key_copy)(void *),
        void(*key_free)(void *),
        int16_t(*data_compare)(void *,void *),
        void*(*data_copy)(void *),
        void(*data_free)(void *));

void ht_insert(Hashtable *table, void *key, void *data);

void* ht_search(Hashtable *table, void *key);

uint16_t ht_num_entries(Hashtable *table);

void* ht_remove(Hashtable *table, void *key);

void ht_free(Hashtable *table);

#endif /* HASHTABLE_ */
