#ifndef UINT16_LIST_
#define UINT16_LIST_

#include <stdint.h>
#include <stdarg.h>
#include "linked_list.h"

/* These functions work with pointers.  The copy and free routines
 * allocate and deallocate data. */

/* Compare two uint16_t values. */
int16_t uint16_compare(void *int_a, void *int_b);

/* Create a copy of a uint16_t value. */
void* uint16_copy(void* int_data);

/* Free a uint16_t value. */
void uint16_free(void* int_data);

/* Compare two lists of uint16_t values. This can be used to sort lists
 * of uint16_t lists.*/
int16_t uint16_list_compare(void *data_a, void* data_b);

/* Copy a list of uint16_t values. */
void *uint16_list_copy(void *data);

/* Free a list of uint16_t values. */
void uint16_list_free(void *data);

/* Utility for making lists of uint16_t values. */
List *uint16_list_create(uint16_t length, ...);

#endif /*UINT16_LIST_*/
