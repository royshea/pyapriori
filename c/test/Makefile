# Author: Roy Shea (royshea@gmail.com)
# Date:  9/21/09


####
# Project specific configuration
####
SOURCES = test_linked_list.c
OBJECTS = $(SOURCES:.c=.o)

####
# General compiler configuration 
####
CC = gcc
CFLAGS = -g -Wall -pedantic -I../src


####
# Target specific default rulse
####
all: test_linked_list

test_linked_list: $(OBJECTS)
	$(CC) $(CFLAGS) $< -o $@ -L../src -lapriori -lcmockery


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
