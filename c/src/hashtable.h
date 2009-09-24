#ifndef HASHTABLE_
#define HASHTABLE_

#include <stdint.h>

typedef struct _ht_table Hashtable;

Hashtable* ht_create(uint16_t size_hint,
        uint16_t(*hash_function)(uint16_t),
        void*(*deep_copy)(void*),
        void(*free_data)(void*));

void ht_insert(Hashtable *table, uint16_t key, void* data);

void* ht_search(Hashtable *table, uint16_t key);

uint16_t ht_num_entries(Hashtable *table);

void* ht_remove(Hashtable *table, uint16_t key);

void ht_free(Hashtable *table);

#endif /* HASHTABLE_ */
