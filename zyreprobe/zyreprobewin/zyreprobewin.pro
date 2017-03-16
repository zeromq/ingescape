TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

include ('C:\ZYRE\zyre.pri')

SOURCES += main.c \
    zregex.c

HEADERS += \
    zregex.h

