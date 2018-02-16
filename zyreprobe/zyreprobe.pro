TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

TARGET = zyreprobe

SOURCES += \
    ./zyreprobe/main.c \
    ./zyreprobe/zregex.c

HEADERS += \
    ./zyreprobe/zregex.h


unix: {
    message(UNIX specific rules)
    LIBS += -L/usr/local/lib/ -lczmq -lzmq -lsodium -lzyre
    INCLUDEPATH += /usr/local/include
    DEPENDPATH += /usr/local/include
    PRE_TARGETDEPS += /usr/local/lib/libczmq.a /usr/local/lib/libzmq.a /usr/local/lib/libsodium.a /usr/local/lib/libzyre.a
}


win32 {
    message("Win32 specific rules")

    QMAKE_CXXFLAGS += -std=c11

    CONFIG(debug, debug|release){
        #configuration DEBUG
        DESTDIR = $$OUT_PWD/debug
        libs_path = $$PWD/../dependencies/windows/libs/win32/Debug
    }else {
        #configuration RELEASE
        DESTDIR = $$OUT_PWD/release
        libs_path = $$PWD/../dependencies/windows/libs/win32/Release
    }

    #UNIX functions
    SOURCES += $$PWD/../dependencies/windows/unix/unixfunctions.c \

    HEADERS += $$PWD/../dependencies/windows/unix/unixfunctions.h \

    INCLUDEPATH += $$PWD/../dependencies/windows/unix \

    #Add librairies
    LIBS += -L$$libs_path -llibzyre -llibczmq -lyajl

    #To get the Ip address into the network.c
    LIBS += -L$$C:/Windows/System32 -lwsock32 -lIPHLPAPI -lws2_32

}
