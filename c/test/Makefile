# Author: Roy Shea (royshea@gmail.com)
# Date:  9/21/09


####
# General compiler configuration 
####
CC = gcc
CFLAGS = -g -Wall -pedantic -I../src
LIBS = -L../src -lapriori -lcmockery

####
# Target specific default rulse
####
#all: test_linked_list test_hashtable test_hashtree
all: test_linked_list

test_linked_list: test_linked_list.o
	$(CC) $(CFLAGS) $< -o $@ $(LIBS)

test_hashtable: test_hashtable.o
	$(CC) $(CFLAGS) $< -o $@ $(LIBS)

test_hashtree: test_hashtree.o
	$(CC) $(CFLAGS) $< -o $@ $(LIBS)


####
# Generic build rules for constructing a library
####
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@


####
# Clean up our mess...
####
clean:
	rm -f *.o
