/*
 * "Copyright (c) 2009 The Regents of the University  of California.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written
 * agreement is hereby granted, provided that the above copyright
 * notice, the following two paragraphs and the author appear in all
 * copies of this software.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
 * FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN
 * IF THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
 * CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT,
 * UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
 *
 * Author: Roy Shea (royshea@gmail.com)
 * Date 10/2009
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>

#include "linked_list.h"
#include "hashtree.h"

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


/* Construct a hash tree from a List of key lists. */
Hashtree *build_hashtree(List *list)
{
    Hashtree *tree;
    uint16_t i;

    tree = tree_create(5, uint16_hash, uint16_list_hash,
            uint16_compare, uint16_copy, uint16_free,
            uint16_compare, uint16_copy, uint16_free);

    for (i = 0; i < ll_length(list); i++)
    {
        List *key;
        uint16_t *count;

        key = ll_copy(ll_get_nth(list, i));
        count = malloc(sizeof(count));
        assert(count != NULL);
        *count = 0;
        ll_sort(key);
        tree_insert(tree, key, count);
    }

    return tree;
}


/* Return TRUE if the first n-1 elements of two length n lists match. */
static uint8_t match_prefix(List *list_a, List *list_b)
{
    uint16_t i;

    assert(ll_length(list_a) == ll_length(list_b));

    /* Compare the n-1 elements of each set. */
    for (i = 0; i < ll_length(list_a)-1; i++)
    {
        if (*(uint16_t *)ll_get_nth(list_a, i) !=
                *(uint16_t *)ll_get_nth(list_b, i))
            return FALSE;
    }

    return TRUE;
}


/* Return true if there exists a set c_less_one that is a subset of
 * candidate missing exactly one element and that is not a member of
 * any of the prior_sets. Note that candidate is an uint16_t list and
 * that prior_sets is a list of uint16_t lists. */
static uint8_t prune(List *candidate, List *prior_sets)
{
    uint16_t i;

    for (i=0; i<ll_length(candidate); i++)
    {
        List *c_less_one;
        uint16_t *data;

        /* Create c_less_one by removing one element. */
        c_less_one = ll_copy(candidate);
        data = ll_get_nth(candidate, i);
        data = ll_remove(c_less_one, data);
        assert(data != NULL);
        free(data);
        assert(ll_length(candidate) - ll_length(c_less_one) == 1);

        /* Check if c_less_one is in prior_sets. */
        if (ll_search(prior_sets, c_less_one) == NULL)
        {
            ll_free(c_less_one);
            return TRUE;
        }

        ll_free(c_less_one);
    }

    return FALSE;
}


/* Given a list of size n transactions that are frequent, generate the
 * set of candidate lists that are of size n+1.  This generation is
 * based on the algorithm put forward in section 2,1,1 of agrawal94fast.
 */
List *generate_candidate_sets(List *prior_sets)
{
    List *candidates;
    List *pruned_candidates;
    List *candidate;
    uint16_t i;
    uint16_t j;

    candidates = ll_create(uint16_list_compare, uint16_list_copy,
            uint16_list_free);
    pruned_candidates = ll_create(uint16_list_compare, uint16_list_copy,
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
            List *set_b;
            set_b = ll_get_nth(prior_sets, j);

            /* If all but the last element match, then create a
             * candidate set. */
            if (match_prefix(set_a, set_b) == TRUE)
            {
                uint16_t *data;

                candidate = ll_copy(set_a);
                data = malloc(sizeof(uint16_t));
                *data = *(uint16_t *)ll_get_nth(set_b, ll_length(set_b) - 1);

                /* NOTE: This construction keeps all elements sorted. */
                ll_push_tail(candidate, data);
                ll_push_tail(candidates, candidate);
            }
            else
                break;
        }
    }

    /* Continue by pruning from candidates any set candidate where
     * c_less_one is a subset of candidate missing exactly one element
     * and c_less_one is not a member of prior_sets. */
    for (candidate = ll_pop(candidates); candidate != NULL;
            candidate = ll_pop(candidates))
    {
        if (prune(candidate, prior_sets) == TRUE)
            ll_free(candidate);
        else
            ll_push(pruned_candidates, candidate);
    }
    assert(ll_length(candidates) == 0);
    ll_free(candidates);

    return pruned_candidates;
}


/* Generate the set of size 1 transactions with minium support greater
 * than the specified support ratio.  The members of the resulting list
 * are each stored as a list of length one.  This results in the same
 * structure being used here as later when sets are of length greater
 * than one.
 *
 * TODO: Ample opportunity for optimization. */
List *generate_frequent_size_one(List *stream, List *transactions,
        uint16_t min_support_count)
{
    List *unique_elements;
    List *size_one;
    List *tmp_copy;
    uint16_t *stream_item;
    uint16_t *unique_val;

    assert(min_support_count > 0);

    unique_elements = ll_create(uint16_compare, uint16_copy, uint16_free);
    size_one = ll_create(uint16_list_compare, uint16_list_copy,
            uint16_list_free);

    /* Iterate through a stream of uint16_t* and generates a list of
     * unique values. */
    tmp_copy = ll_copy(stream);
    for (stream_item = ll_pop(tmp_copy); stream_item != NULL;
            stream_item = ll_pop(tmp_copy))
    {

        if (ll_search(unique_elements, stream_item) == NULL)
            ll_push(unique_elements, stream_item);
        else
            free(stream_item);
    }
    ll_free(tmp_copy);


    /* Count the number of times each unique element occures within a
     * transaction.  If this is greater than or equal to the
     * min_support_count, then add the unique element to the size_one
     * list. */
    for (unique_val = ll_pop(unique_elements); unique_val != NULL;
            unique_val = ll_pop(unique_elements))
    {
        uint16_t support_count;
        List *transaction;

        /* Count the number of transactions involving unique_val. */
        support_count = 0;
        tmp_copy = ll_copy(transactions);
        for (transaction = ll_pop(tmp_copy); transaction != NULL;
                transaction = ll_pop(tmp_copy))
        {
            if (ll_search(transaction, unique_val) != NULL)
                support_count += 1;
            ll_free(transaction);
        }
        ll_free(tmp_copy);

        /* Store unique_val in size_one if it has enough support. */
        if (support_count >= min_support_count)
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

            /* Apriori works with sets of data.  So repeated information
             * is eleminated from the set. */
            if (ll_search(sublist, data) == NULL)
                ll_push(sublist, data);
            else
                free(data);
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

        /* If something has gone wrong, clean up (for testing) and abort. */
        if (read_count != 1)
        {
            free(data);
            ll_free(list);
            assert(read_count == 1);
        }

        ll_push(list, data);
    }
    fclose(fid);

    return list;
}


List *apriori(char *file_name, uint8_t transaction_width,
        float support_ratio)
{
    List *trace;
    List *transactions;
    List *size_n_frequent;
    List *frequent;
    uint16_t min_support_count;

    /* Read in the data. */
    trace = read_uint16_list(file_name);

    /* Use a sliding window to generate small "transactions" from the
     * data stream. */
    transactions = make_transactions_fixed_width(trace, transaction_width);
    min_support_count = support_ratio * ll_length(transactions);

    /* Generate the size one frequent sets and store a copy in frequent. */
    size_n_frequent = generate_frequent_size_one(trace, transactions,
            min_support_count);

    frequent = ll_copy(size_n_frequent);
    ll_free(trace);

    /* Iteratively generate the size n+1 frequent sets.  Store all
     * frequent sets in frequent.  Iterate until no larger sets are
     * generated. */
    while (ll_length(size_n_frequent) != 0)
    {
        List *candidates;
        List *tmp_copy;
        Hashtree *candidate_tree;
        List *transaction;
        uint16_t i;

        /* Size n+1 candidate sets. */
        candidates = generate_candidate_sets(size_n_frequent);
        ll_free(size_n_frequent);

        /* Put into a hash tree. */
        candidate_tree = build_hashtree(candidates);
        ll_free(candidates);

        /* Count frequencies and extract frequent candidate sets. */
        for (i = 0; i < ll_length(transactions); i++)
        {
            transaction = ll_get_nth(transactions, i);
            tree_mark_subsets(candidate_tree, transaction);
        }

        size_n_frequent = tree_extract_frequent(candidate_tree,
                min_support_count);
        tree_free(candidate_tree);

        /* Copy frequent candidate sets into frequent. */
        tmp_copy = ll_copy(size_n_frequent);
        while (ll_length(tmp_copy) > 0)
            ll_push(frequent, ll_pop(tmp_copy));
        ll_free(tmp_copy);
    }

    ll_free(size_n_frequent);
    ll_free(transactions);

    return frequent;
}
