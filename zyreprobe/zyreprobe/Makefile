#
# 
# Copyright (c) 2016 Ingenuity i/o. All rights reserved.
#
# See license terms for the rights and conditions
# defined by copyright holders.
#
#
# Contributors:
#	Stephane Vales <vales@ingenuity.io>
#


INCLOCAL = /usr/local/include/
INCDIRS   = -I${INCLOCAL}

LIBDIRLOCAL = /usr/local/lib
DYNLIBS = -lzyre -lczmq -lzmq -lsodium
STATICLIBS = -static ${LIBDIRLOCAL}/libzyre.a ${LIBDIRLOCAL}/libczmq.a ${LIBDIRLOCAL}/libzmq.a ${LIBDIRLOCAL}/libsodium.a  

CC = gcc
CFLAGS = -W -Wall -g $(INCDIRS) -std=gnu99
LDFLAGS = -L$(LIBDIRLOCAL) $(DYNLIBS)


EXECUTABLE = zyreprobe
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)

 
all : $(EXECUTABLE) 
 
$(EXECUTABLE) : $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)
	
%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $<
	
clean :
	@rm *.o $(EXECUTABLE)

test : $(EXECUTABLE) 
	@./$(EXECUTABLE)

install : $(EXECUTABLE)
	@cp $(EXECUTABLE) /usr/local/bin/