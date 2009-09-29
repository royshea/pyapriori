#include <stdio.h>
#include <assert.h>

#include "linked_list.h"
#include "uint16_list.h"
#include "apriori.h"

#if UNIT_TESTING
#include "../test/unit_testing.h"
#endif /* UNIT_TESTING */


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
        }

        /* Add sorted sublist to the set of transactions. */
        ll_sort(sublist);
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
