TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

TARGET = zyreprobe

SOURCES += \
    ./zyreprobe/main.c \
    ./zyreprobe/zregex.c

HEADERS += \
    ./zyreprobe/zregex.h \


unix: {
    message(UNIX specific rules)
    LIBS += /usr/local/lib/libczmq.a /usr/local/lib/libzmq.a /usr/local/lib/libsodium.a /usr/local/lib/libzyre.a
    INCLUDEPATH += /usr/local/include
    DEPENDPATH += /usr/local/include
}


win32 {
    message("Win32 specific rules")

    QMAKE_CXXFLAGS += -std=c11

    CONFIG(debug, debug|release){
        #configuration DEBUG
        DESTDIR_WIN = $$OUT_PWD/debug
        libs_path = $$PWD/../dependencies/windows/libs/win32/Debug
    }else {
        #configuration RELEASE
        DESTDIR_WIN = $$OUT_PWD/release
        libs_path = $$PWD/../dependencies/windows/libs/win32/Release
    }

    #librariesToCopy.files += $$libs_path/libzyre.dll

    #UNIX functions
    SOURCES += $$PWD/../dependencies/windows/unix/unixfunctions.c \
                $$PWD/../dependencies/windows/unix/getopt.h

    HEADERS += $$PWD/../dependencies/windows/unix/unixfunctions.h \
                $$PWD/../dependencies/windows/unix/getopt.h \
                $$PWD/../dependencies/windows/pcre/headers/pcre.h\
                $$PWD/../dependencies/windows/pcre/headers/pcreposix.h\
                $$PWD/../dependencies/windows/pcre/headers/regex.h

    INCLUDEPATH += $$PWD/../dependencies/windows/unix \
                   $$PWD/../dependencies/windows/headers \
                   $$PWD/../dependencies/windows/zyre_suite \
                   $$PWD/../dependencies/windows/pcre/headers/ \

    #Add librairies
    LIBS += -L$$libs_path -llibzyre -llibczmq -lyajl

    #To get the Ip address into the network.c
    LIBS += -L$$C:/Windows/System32 -lwsock32 -lIPHLPAPI -lws2_32

    #Add pcre librairies
    LIBS += -L$$PWD/../dependencies/windows/pcre/libs -lpcre -lpcreposix

    ##Add headers from dependencies
    zyre_include_path = $$PWD/../dependencies/windows/headers/zyre_suite
    yajl_include_path = $$PWD/../dependencies/windows/headers/

    INCLUDEPATH += $$zyre_include_path \
                   $$yajl_include_path \

    DEPENDPATH += $$zyre_include_path \
                  $$yajl_include_path \

    # Copy libraries in our application directory
    EXTRA_BINFILES += \
        $$libs_path/libzmq.dll \
        $$libs_path/libzyre.dll \
        $$libs_path/libczmq.dll \
        $$libs_path/yajl.dll \
        $$PWD/../dependencies/windows/pcre/libs/pcre3.dll \
        $$PWD/../dependencies/windows/pcre/libs/pcreposix3.dll

    EXTRA_BINFILES_WIN = $${EXTRA_BINFILES}
    EXTRA_BINFILES_WIN ~= s,/,\\,g

    DESTDIR_WIN ~= s,/,\\,g

    for(FILE,EXTRA_BINFILES_WIN){
        QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$shell_path($${FILE})) $$quote($$shell_path($${DESTDIR_WIN})) $$escape_expand(\n\t)
    }

}

#--------- COMMON ---------#


