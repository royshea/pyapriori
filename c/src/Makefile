# Author: Roy Shea (royshea@gmail.com)
# Date:  9/22/09

####
# Project specific configuration
####
SOURCES = linked_list.c hashtable.c hashtree.c apriori.c uint16_list.c
OBJECTS = $(SOURCES:.c=.o)
LIBS = -L. -lapriori
CFLAGS += -g -Wall -pedantic


####
# Compiler configuration 
####
ifeq ($(MAKECMDGOALS),avr)
CC = avr-gcc
AR = avr-ar
LIB = libapriori-avr.a
TARGETFLAGS = -mmcu=atmega128

else ifeq ($(MAKECMDGOALS),msp430)
CC = msp430-gcc
AR = msp430-ar
LIB = libapriori-msp430.a
TARGETFLAGS = -specs=/usr/lib/ncc/tdspecs -mdisable-hwmul -mmcu=msp430x1611

else ifneq ($(MAKECMDGOALS),clean)
CC = gcc
AR = ar
LIB = libapriori.a

endif


####
# Target specific default rules.  Explicit clean is added to keep builds
# of different architectures clean.
####
all: clean $(LIB) use_apriori

msp430: clean $(LIB)

avr: clean $(LIB)


####
# Executable wrapping apriori
####

use_apriori: use_apriori.o
	$(CC) $(CFLAGS) $< -o $@ $(LIBS)



####
# Generic build rules for constructing a library
####
$(LIB): $(OBJECTS)
	$(AR) rcs $(LIB) $(OBJECTS)

.c.o:
	$(CC) $(TARGETFLAGS) $(DEFS) $(CFLAGS) -c $< -o $@


####
# Clean up our mess...
####
clean:
	rm -f *.o

####
# Extra dependencies
####
linked_list.o: linked_list_private.h

hashtable.o: linked_list.o hashtable_private.h

hashtree.o: linked_list.o hashtable.o hashtree_private.h
