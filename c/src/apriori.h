#ifndef APRIORI_
#define APRIORI_

#include "linked_list.h"

/* Generate frequent sets of size 1. */
List *generate_frequent_size_one();

/* Convert a list of data into a distinct "transaction" lists.  Each
 * transaction is a copy of the list contained within a fixed width
 * sliding window passing over the input list. */
List *make_transactions_fixed_width(List *stream, uint8_t width);

/* Read in uint16_t data from a white space seperated data file. */
List *read_uint16_list(char *file_name);

#endif /*APRIORI_*/
