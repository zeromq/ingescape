SOURCES += \
    $$PWD/../src/definition.c \
    $$PWD/../src/mapping.c \
    $$PWD/../src/network.c \
    $$PWD/../src/tests.c \
    $$PWD/../src/model.c \
    $$PWD/../src/parser.c \
    $$PWD/../src/kvsimple.c

HEADERS += \
    $$PWD/../src/include/definition.h \
    $$PWD/../src/include/mapping.h \
    $$PWD/../src/include/model.h \
    $$PWD/../src/include/network.h \
    $$PWD/../src/include/tests.h \
    $$PWD/../src/include/uthash/uthash.h \
    $$PWD/../src/include/parser.h \
    $$PWD/../src/include/kvsimple.h \

INCLUDEPATH += $$PWD/../src/include \
#               $$PWD/uthash \

#add librairies zyre, czmq, zmq + yajl and configurate only for VS 2015 x86 (32 bits)
win32:{

    QMAKE_CXXFLAGS += -std=c11

    INCLUDEPATH += $$PWD/getopt \

    HEADERS += $$PWD/getopt/getopt_win32.h
    SOURCES += $$PWD/getopt/getopt_win32.c

    CONFIG(debug, debug|release){
        #configuration DEBUG
        DESTDIR = $$OUT_PWD/debug
        libzyre_path = $$PWD/zyre/bin/Win32/Debug/v140/dynamic
        libyajl_path = $$PWD/yajl/lloyd-yajl-2.1.0/MSVS/VS2013/x86/DebugDLL
    }else {
        #configuration RELEASE
        DESTDIR = $$OUT_PWD/release
        libzyre_path = $$PWD/zyre/bin/Win32/Release/v140/dynamic
        libyajl_path = $$PWD/yajl/lloyd-yajl-2.1.0/MSVS/VS2013/x86/ReleaseDLL
    }

    # Ajouter la librairie : attention -> ajouter l'etape make install a la compilation
    #Add the make step 'install' to copy the dll files to the output folder
    install_libs.files += $$libzyre_path/*.dll \
                            $$libyajl_path/*.dll
    install_libs.path += $$DESTDIR
    INSTALLS += install_libs

    #Add librairies
    LIBS += -L$$libzyre_path -llibzmq \
            -L$$libzyre_path -llibczmq \
            -L$$libzyre_path -llibzyre \
            -L$$libyajl_path -lyajl

    #to get the Ip address into the network.c
    LIBS += -L$$C:/Windows/System32 -lwsock32
    LIBS += -L$$C:/Windows/System32 -lIPHLPAPI
    LIBS += -L$$C:/Windows/System32 -lws2_32
}

macx:{

    libzyre_path = $$PWD/zyre/bin/Macos
    libyajl_path = $$PWD/yajl/lloyd-yajl-2.1.0/Macos/lib

    #Add librairies
    LIBS += -L$$libzyre_path -lzmq -lczmq -lzyre \
            -L$$libyajl_path -lyajl

    #Intallation librairie
    DESTDIR = /usr/local/lib

    install_libs.files += $$libzyre_path/*.dylib \
                          $$libyajl_path/*.dylib
    install_libs.path += $$DESTDIR

    INSTALLS += install_libs
}

unix:{

    libzyre_path = $$PWD/zyre/bin/Linux
    libyajl_path = $$PWD/yajl/lloyd-yajl-2.1.0/Linux/lib

    #Add librairies
    LIBS += -L$$libzyre_path -lzmq -lczmq -lzyre \
            -L$$libyajl_path -lyajl

    #Intallation librairie
    #DESTDIR = /usr/local/lib

    #install_libs.files += $$libzyre_path/*.so \
    #                        $$libyajl_path/*.so
    #install_libs.path += $$DESTDIR

    #INSTALLS += install_libs
}

#--------- COMMON ---------#

##Add headers from dependencies
INCLUDEPATH += $$PWD/libzmq/include \
               $$PWD/czmq/include \
               $$PWD/zyre/include \
               $$PWD/yajl/lloyd-yajl-2.1.0/include

DEPENDPATH += $$PWD/libzmq/include \
              $$PWD/czmq/include \
              $$PWD/zyre/include \
              $$PWD/yajl/lloyd-yajl-2.1.0/include


