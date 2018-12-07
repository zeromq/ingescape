lib_LTLIBRARIES = libingescape.la

libingescape_la_includedir = $(includedir)/ingescape
libingescape_la_include_HEADERS = include/ingescape.h include/ingescape_advanced.h

libingescape_la_LDFLAGS = \
    -version-info @LTVER@ \
    $(LIBTOOL_EXTRA_LDFLAGS)

libingescape_la_CFLAGS = -Iinclude/

libingescape_la_SOURCES = \
	definition.c \
	mapping.c \
	model.c \
	network.c \
	parser.c \
	admin.c \
	bus.c \
	token.c

if INGESCAPE_FOR_WINDOWS
libingescape_la_CFLAGS += -I../dependencies/windows/unix/ -DINGESCAPE=1 -Wl,--output-def,libingescape.def
libingescape_la_SOURCES += ../dependencies/windows/unix/unixfunctions.c
endif
