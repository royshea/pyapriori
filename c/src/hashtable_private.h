#ifndef HASH_TABLE_PRIVATE_
#define HASH_TABLE_PRIVATE_

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


typedef struct _ht_entry
{
    void *key;
    void *data;
} Entry;


struct _ht_table
{
    /* Basic table statistics. */
    uint16_t size;
    uint16_t count;

    /* Type specific functions used by table. */
    uint16_t (*hash_function) (void*);
    int16_t (*compare) (void*, void*);
    void* (*deep_copy) (void*);
    void (*free_data) (void*);
    void (*free_key) (void*);

    /* Array of linked lists making up the table data. */
    Node **table;

};

#endif /* HASH_TABLE_PRIVATE_ */

