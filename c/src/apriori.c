#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "linked_list.h"
#include "uint16_list.h"
#include "apriori.h"

#if UNIT_TESTING
#include "../test/unit_testing.h"
#endif /* UNIT_TESTING */

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/* Given a list of size n transactions that are frequent, generate the
 * set of candidate lists that are of size n+1.  This generation is
 * based on the algorithm put forward in section 2,1,1 of agrawal94fast.
 */
List *generate_candidate_sets(List *prior_sets)
{
    List *candidates;
    uint16_t i;
    uint16_t j;
    uint16_t k;

    candidates = ll_create(uint16_list_compare, uint16_list_copy,
            uint16_list_free);

    /* Ensure that everything is sorted. */
    for (i=0; i<ll_length(prior_sets); i++)
        ll_sort(ll_get_nth(prior_sets, i));
    ll_sort(prior_sets);


    /* Examine pairs of sets from prior_sets.  For those sets that only
     * differ in the last element, create a new set using the union of
     * the two sets.  By ordering the sets, we can abort the operation
     * and move the outer index as soon as two sets are found that
     * differ by more than the last element.
     */
    for (i=0; i<ll_length(prior_sets); i++)
    {
        List *set_a;
        set_a = ll_get_nth(prior_sets, i);

        for (j=i+1; j<ll_length(prior_sets); j++)
        {
            uint8_t prefix_match;
            List *set_b;
            set_b = ll_get_nth(prior_sets, j);

            /* Compare the n-1 elements of each set. */
            prefix_match = TRUE;
            for (k=0; k<ll_length(set_a)-1; k++)
            {
                if (*(uint16_t *)ll_get_nth(set_a, k) !=
                        *(uint16_t *)ll_get_nth(set_b, k))
                {
                    prefix_match = FALSE;
                    break;
                }
            }

            /* If all but the last element match, then create a
             * candidate set. */
            if (prefix_match == TRUE)
            {
                List * candidate;
                uint16_t *data;

                candidate = ll_copy(set_a);
                data = malloc(sizeof(uint16_t));
                *data = *(uint16_t *)ll_get_nth(set_b, ll_length(set_b) - 1);
                ll_push_tail(candidate, data);
                ll_push_tail(candidates, candidate);
            }
            else
                break;
        }
    }
    return candidates;
}

/* Generate the set of size 1 transactions with minium support greater
 * than the specified support ratio.  The members of the resulting list
 * are each stored as a list of length one.  This results in the same
 * structure being used here as later when sets are of length greater
 * than one.
 *
 * TODO: Ample opportunity for optimization. */
List *generate_frequent_size_one(List *stream, List *transactions,
        float support_ratio)
{
    uint16_t min_support;
    List *unique_elements;
    List *size_one;
    uint16_t stream_index;
    uint16_t *unique_val;

    min_support = support_ratio * ll_length(stream);
    assert(min_support > 0);

    unique_elements = ll_create(uint16_compare, uint16_copy, uint16_free);
    size_one = ll_create(uint16_list_compare, uint16_list_copy,
            uint16_list_free);

    /* Iterate through stream generate a list of unique values. */
    for (stream_index=0; stream_index<ll_length(stream); stream_index++)
    {
        uint16_t *tmp;

        tmp = ll_get_nth(stream, stream_index);
        if (ll_search(unique_elements, tmp) == NULL)
        {
            uint16_t *data;
            data = malloc(sizeof(uint16_t));
            *data = *tmp;
            ll_push(unique_elements, data);
        }
    }

    /* Count the number of times each unique element occures within
     * stream.  If this is greater than or equal to the min_support,
     * then add the unique element to the size_one list. */
    for (unique_val = ll_pop(unique_elements); unique_val != NULL;
            unique_val = ll_pop(unique_elements))
    {
        uint16_t support_count;
        uint16_t stream_index;

        /* Count the number of transactions involving unique_val. */
        support_count = 0;
        for (stream_index=0; stream_index<ll_length(stream);
                stream_index++)
        {
            if (*unique_val == *(uint16_t *)ll_get_nth(stream, stream_index))
                support_count += 1;

        }

        /* Store unique_val in size_one if it has enough support. */
        if (support_count >= min_support)
        {
            List *singleton;
            singleton = ll_create(uint16_compare, uint16_copy, uint16_free);
            ll_push(singleton, unique_val);
            ll_push(size_one, singleton);
        }
        else
            free(unique_val);
    }

    assert(ll_length(unique_elements) == 0);
    ll_free(unique_elements);

    return size_one;
}


/* Convert a list of data into a distinct "transaction" lists.  Each
 * transaction is a copy of the list contained within a fixed width
 * sliding window passing over the input list. */
List *make_transactions_fixed_width(List *stream, uint8_t width)
{
    List *transaction_list;
    int stream_index;

    assert(width > 0);

    /* Create a list where each member is a list of uint16_t. */
    transaction_list = ll_create(uint16_list_compare, uint16_list_copy,
            uint16_list_free);

    /* Slide window with specified width across the stream.  Copy data
     * within window into a sublist that is placed on transaction_list. */
    for (stream_index=0; stream_index<ll_length(stream) - (width - 1);
            stream_index++)
    {
        int window_index;
        List *sublist;

        /* Create a sublist for each window. */
        sublist = ll_create(uint16_compare, uint16_copy, uint16_free);
        for (window_index=0; window_index<width; window_index++)
        {
            uint16_t *data;
            data = malloc(sizeof(uint16_t));
            *data = *(uint16_t *)ll_get_nth(stream, stream_index + window_index);
            ll_push(sublist, data);
        }

        /* Add sublist to the set of transactions. */
        ll_push(transaction_list, sublist);
    }

    return transaction_list;
}


/* Read in uint16_t data from a white space seperated data file. */
List *read_uint16_list(char *file_name)
{
    FILE *fid;
    List *list;

    list = ll_create(uint16_compare, uint16_copy, uint16_free);

    fid = fopen(file_name, "r");
    assert(fid != NULL);
    while(!feof(fid))
    {
        int read_count;
        uint16_t *data;

        data = malloc(sizeof(uint16_t));
        assert(data != NULL);
        read_count = fscanf(fid, "%hu", data);
        if (read_count == EOF)
        {
            free(data);
            break;
        }
        ll_push(list, data);
    }
    fclose(fid);

    return list;
}
