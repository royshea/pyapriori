# Author: Roy Shea (royshea@gmail.com)
# Date:  9/21/09

####
# Project specific configuration
####
SOURCES = linked_list.c
LIB = libapriori.a

OBJECTS = $(SOURCES:.c=.o)

####
# General compiler configuration 
####
CC = gcc
AR = ar
CFLAGS = -g -Wall -pedantic

####
# Target specific default rulse
####
all: clean $(LIB)


####
# Generic build rules for constructing a library
####
$(LIB): $(OBJECTS)
	$(AR) rcs $(LIB) $(OBJECTS)

.c.o:
	$(CC) $(TARGETFLAGS) $(CFLAGS) -c $< -o $@

####
# Clean up our mess...
####
clean:
	rm -f *.o
