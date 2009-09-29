#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>

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
