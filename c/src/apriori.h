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
        uint16_t min_support_count);


/* Convert a list of data into a distinct "transaction" lists.  Each
 * transaction is a copy of the list contained within a fixed width
 * sliding window passing over the input list. */
List *make_transactions_fixed_width(List *stream, uint8_t width);


/* Read in uint16_t data from a white space separated data file. */
List *read_uint16_list(char *file_name);


/* Implementation of the Apriori algorithm. */
List *apriori(char *file_name, uint8_t transaction_width, float
        support_ratio);

#endif /*APRIORI_*/
