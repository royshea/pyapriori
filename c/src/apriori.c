#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "linked_list.h"
#include "uint16_list.h"
#include "apriori.h"

#if UNIT_TESTING
#include "../test/unit_testing.h"
#endif /* UNIT_TESTING */

/* Generate the set of size 1 transactions with minium support greater
 * than the specified support ratio.
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
    size_one = ll_create(uint16_compare, uint16_copy, uint16_free);

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
            ll_push(size_one, unique_val);
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
