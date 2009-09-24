# Author: Roy Shea (royshea@gmail.com)
# Date:  9/22/09

####
# Project specific configuration
####
SOURCES = linked_list.c hashtable.c
LIB = libapriori.a

OBJECTS = $(SOURCES:.c=.o)


####
# General compiler configuration 
####
CC = gcc
AR = ar
CFLAGS = -g -Wall -pedantic


####
# Target specific default rules
####
all: $(LIB)


####
# Generic build rules for constructing a library
####
$(LIB): $(OBJECTS)
	$(AR) rcs $(LIB) $(OBJECTS)

.c.o:
	$(CC) $(DEFS) $(CFLAGS) -c $< -o $@


####
# Clean up our mess...
####
clean:
	rm -f *.o

####
# Extra dependencies
####
linked_list.o: linked_list_private.h

hashtable.o: linked_list.c hashtable_private.h
