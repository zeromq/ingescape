SOURCES += \
    $$PWD/../src/mtic_definition.c \
    $$PWD/../src/mtic_model.c \
    $$PWD/../src/mtic_network.c \
    $$PWD/../src/mtic_mapping.c \
    $$PWD/../src/mtic_parser.c \
    $$PWD/../src/mtic_tests.c

HEADERS += \
    $$PWD/../src/include/mtic_tests.h \
    $$PWD/../src/include/mtic_network.h \
    $$PWD/../src/include/mtic_parser.h \
    $$PWD/../src/include/mtic_model.h \
    $$PWD/../src/include/mtic_mapping.h \
    $$PWD/../src/include/mtic_definition.h \
    $$PWD/../src/include/mtic_mastic.h \
    $$PWD/../src/include/uthash/uthash.h

INCLUDEPATH += $$PWD/../src/include

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

    #Destination repository for our librairy
    DESTDIR = /usr/local/lib

    #Copy includes
    install_headers.files += $$PWD/../src/include/*.h \
                             $$PWD/../src/include/uthash
    install_headers.path += /usr/local/include/mastic

    #Copy libraries
    install_libs.files += $$libzyre_path/*.dylib \
                          $$libyajl_path/*.dylib
    install_libs.path += $$DESTDIR

    #Add installation options
    INSTALLS += install_libs \
                install_headers
}

unix:{

    libzyre_path = $$PWD/zyre/bin/Linux
    libyajl_path = $$PWD/yajl/lloyd-yajl-2.1.0/Linux/lib

    #Add librairies
    LIBS += -L$$libzyre_path -lzmq -lczmq -lzyre \
            -L$$libyajl_path -lyajl

    #Intallation librairie: USE MAKE FILE in InstallLib.mk in builds directory
    #sudo make -f InstallLib.mk
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


