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

# all dependencies listed in LDFLAGS shall be installed before running this Makefile

CC = gcc
CFLAGS = -g -I./include -I/usr/local/include/ -fPIC -Wextra -O2 -std=gnu99 -Wall
LDFLAGS = -shared -L/usr/local/lib -lsodium -lzmq -lczmq -lzyre -lyajl
RM = rm -f
ifeq ($(shell uname -s), Darwin)
    TARGET_LIB = libmastic.dylib
else
    TARGET_LIB = libmastic.so
endif
STARGET_LIB = libmastic.a

SRCS = definition.c mapping.c model.c network.c parser.c admin.c bus.c
OBJS = $(SRCS:.c=.o)

.PHONY: all
all: ${TARGET_LIB} ${STARGET_LIB}

$(TARGET_LIB): $(OBJS)
	$(CC) ${LDFLAGS} -o $@ $^

$(STARGET_LIB): $(OBJS)
	$(AR) rcs $@ $^

$(SRCS:.c=.d):%.d:%.c
	$(CC) $(CFLAGS) -MM $< >$@

.PHONY: clean
clean:
	-${RM} ${TARGET_LIB} ${OBJS} $(SRCS:.c=.d)
	-${RM} ${STARGET_LIB} ${OBJS} $(SRCS:.c=.d)

PREFIX = /usr/local

.PHONY: install
install: $(TARGET_LIB) $(STARGET_LIB)
	mkdir -p $(PREFIX)/lib
	mkdir -p $(PREFIX)/include/mastic
	install $(TARGET_LIB) $(DESTDIR)$(PREFIX)/lib/$(TARGET_LIB)
	install $(STARGET_LIB) $(DESTDIR)$(PREFIX)/lib/$(STARGET_LIB)
	install ./include/mastic.h $(DESTDIR)$(PREFIX)/include/mastic/

.PHONY: uninstall
uninstall:
	rm -f $(PREFIX)/lib/$(TARGET_LIB)
	rm -f $(PREFIX)/lib/$(STARGET_LIB)
	rm -Rf $(PREFIX)/include/mastic
