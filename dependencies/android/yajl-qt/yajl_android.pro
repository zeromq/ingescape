TEMPLATE = lib

#by default .so
#CONFIG+= dll

#create .a
#CONFIG += staticlib

#create .la
CONFIG += static

TARGET = yajl

yajl_path = $$PWD/../../yajl

INCLUDEPATH += $$PWD/../../windows/headers/ \

HEADERS += \
    $${yajl_path}/src/yajl_alloc.h \
    $${yajl_path}/src/yajl_buf.h \
    $${yajl_path}/src/yajl_bytestack.h \
    $${yajl_path}/src/yajl_encode.h \
    $${yajl_path}/src/yajl_lex.h \
    $${yajl_path}/src/yajl_parser.h \

SOURCES += \
    $${yajl_path}/src/yajl.c \
    $${yajl_path}/src/yajl_alloc.c \
    $${yajl_path}/src/yajl_buf.c \
    $${yajl_path}/src/yajl_encode.c \
    $${yajl_path}/src/yajl_gen.c \
    $${yajl_path}/src/yajl_lex.c \
    $${yajl_path}/src/yajl_parser.c \
    $${yajl_path}/src/yajl_tree.c \
    $${yajl_path}/src/yajl_version.c




QMAKE_CFLAGS += -std=c11 \
