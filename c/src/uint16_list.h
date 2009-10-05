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


/* Trivial hash function for uint16_t data that simply returns the value. */
uint16_t uint16_hash(void *key);


/* Compare two lists of uint16_t values. This can be used to sort lists
 * of uint16_t lists.*/
int16_t uint16_list_compare(void *data_a, void* data_b);


/* Copy a list of uint16_t values. */
void *uint16_list_copy(void *data);


/* Free a list of uint16_t values. */
void uint16_list_free(void *data);


/* Hash a list of uint16_t values. */
uint16_t uint16_list_hash(void *key);


/* Utility for making lists of uint16_t values. */
List *uint16_list_create(uint16_t length, ...);


/* Print out a list of uint16_t data. */
void uint16_list_print(List *list);

/* Print out a list of uint16_t lists. */
void uint16_list_list_print(List *list);


#endif /*UINT16_LIST_*/
