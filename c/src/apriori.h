#ifndef APRIORI_
#define APRIORI_

#include "linked_list.h"
#include "hashtree.h"

/* Construct a hash tree from a List of key lists. */
Hashtree *build_hashtree(List *list);

/* Given a list of size n transactions that are frequent, generate the
 * set of candidate lists that are of size n+1.  This generation is
 * based on the algorithm put forward in section 2,1,1 of agrawal94fast.
 */
List *generate_candidate_sets(List *prior_sets);


/* Generate frequent sets of size 1. */
List *generate_frequent_size_one(List *stream, List *transactions,
        float support_ratio);


/* Convert a list of data into a distinct "transaction" lists.  Each
 * transaction is a copy of the list contained within a fixed width
 * sliding window passing over the input list. */
List *make_transactions_fixed_width(List *stream, uint8_t width);


/* Read in uint16_t data from a white space seperated data file. */
List *read_uint16_list(char *file_name);


#endif /*APRIORI_*/
