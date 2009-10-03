#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#include "linked_list.h"

#if UNIT_TESTING
#include "../test/unit_testing.h"
#endif /* UNIT_TESTING */


/* Core functions used for manipulating uint16_t data. */
int16_t uint16_compare(void *int_a, void *int_b)
{
    return *(uint16_t *)int_a - *(uint16_t *)int_b;
}


void* uint16_copy(void* int_data)
{
    uint16_t *data;
    data = malloc(sizeof(uint16_t));
    *data = *(uint16_t *)int_data;
    return data;
}


void uint16_free(void* int_data)
{
    free(int_data);
    return;
}


uint16_t uint16_hash(void *key)
{
    return *(uint16_t *)key;
}


int16_t uint16_list_compare(void *data_a, void* data_b)
{
    return ll_list_compare(data_a, data_b);
}


void *uint16_list_copy(void *data)
{
    return ll_copy((List *)data);
}


void uint16_list_free(void *data)
{
    ll_free((List *)data);
}


uint16_t uint16_list_hash(void *key)
{
    List* key_list;
    uint16_t hash;
    void *data;
    uint16_t i;

    key_list = (List *)key;

    /* XOR and shift hashes of individual keys together to generate a
     * single hash for a list of keys.
     *
     * TODO: XOR is not a great way to combine hashes, but it works for
     * now.
     */

    hash = 0;
    for (i=0; i<ll_length(key_list); i++)
    {
        data = ll_get_nth(key_list, i);
        hash ^= uint16_hash(data);
        hash <<= 1;
    }

    return hash;
}


/* Utility for making lists. */
List *uint16_list_create(uint16_t length, ...)
{
    List *list;
    va_list ap;

    list = ll_create(uint16_compare, uint16_copy, uint16_free);
    va_start(ap, length);
    for (; length>0; length--)
    {
        uint16_t *data;
        data = malloc(sizeof(uint16_t));
        *data = (uint16_t)va_arg(ap, int);
        ll_push_tail(list, data);
    }
    va_end(ap);
    return list;
}

/* Print out a list of uint16_t data. */
void uint16_list_print(List *list)
{
    uint16_t i;
    for (i=0; i<ll_length(list); i++)
        printf("%d ", *(uint16_t *)ll_get_nth(list, i));
    printf("\n");
}


/* Print out a list of uint16_t lists. */
void uint16_list_list_print(List *list)
{
    uint16_t i;

    for (i=0; i<ll_length(list); i++)
        uint16_list_print(ll_get_nth(list, i));
}
