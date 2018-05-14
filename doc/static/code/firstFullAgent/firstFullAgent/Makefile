INCLOCAL = /usr/local/include/
INCDIRS   = -I${INCLOCAL}

LIBDIRLOCAL = /usr/local/lib
DYNLIBS = -lingescape

CC = gcc
CFLAGS = -W -Wall -g $(INCDIRS) -std=gnu99
LDFLAGS = -L$(LIBDIRLOCAL) $(DYNLIBS)


EXECUTABLE = firstFullAgent
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
	@install $(EXECUTABLE) /usr/local/bin/
