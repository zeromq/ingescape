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
        libzmq_path = $$PWD/libzmq/bin/Win32/Debug/v140/dynamic
        libczmq_path = $$PWD/czmq/bin/Win32/Debug/v140/dynamic
        libzyre_path = $$PWD/zyre/bin/Win32/Debug/v140/dynamic
        libyajl_path = $$PWD/yajl/lloyd-yajl-2.1.0/MSVS/VS2013/x86/DebugDLL
    }else {
        #configuration RELEASE
        DESTDIR = $$OUT_PWD/release
        libzmq_path = $$PWD/libzmq/bin/Win32/Release/v140/dynamic
        libczmq_path = $$PWD/czmq/bin/Win32/Release/v140/dynamic
        libzyre_path = $$PWD/zyre/bin/Win32/Release/v140/dynamic
        libyajl_path = $$PWD/yajl/lloyd-yajl-2.1.0/MSVS/VS2013/x86/ReleaseDLL
    }

    # Ajouter la librairie : attention -> ajouter l'etape make install a la compilation
    #Add the make step 'install' to copy the dll files to the output folder
    install_libs.files += $$libzmq_path/*.dll \
                           $$libczmq_path/*.dll \
                            $$libzyre_path/*.dll \
                            $$libyajl_path/*.dll
    install_libs.path += $$DESTDIR
    INSTALLS += install_libs

    #Add librairies
    LIBS += -L$$libzmq_path -llibzmq \
            -L$$libczmq_path -llibczmq \
            -L$$libzyre_path -llibzyre \
            -L$$libyajl_path -lyajl

    #to get the Ip address into the network.c
    LIBS += -L$$C:/Windows/System32 -lwsock32
    LIBS += -L$$C:/Windows/System32 -lIPHLPAPI
    LIBS += -L$$C:/Windows/System32 -lws2_32

    #Add common files to the output folder to run test easily
#    common_files.files += $$PWD/common_files/*.json
#    common_files.path += $$DESTDIR
#    INSTALLS += common_files
}

#unix :{
#    #CONFIG -= console

#    SOURCES -= getopt_win32.c
#    HEADERS -= getopt_win32.h

#    #QMAKE_CFLAGS += -std=c99
#    QMAKE_CFLAGS += -std=gnu99
#    QMAKE_CFLAGS += -Werror-implicit-function-declaration

#    libzmq_path = $$PWD/zyre_ref/libczmq/lib/
#    libczmq_path = $$PWD/zyre_ref/czmq/lib/
#    libzyre_path = $$PWD/zyre_ref/zyre/lib/
#    libyajl_path = $$PWD/lloyd-yajl-2.1.0/build/yajl-2.1.0/lib

#    #Add librairies
#    LIBS += -L$$libzmq_path -lzmq \
#            -L$$libczmq_path -lczmq \
#            -L$$libzyre_path -lzyre \
#            -L$$libyajl_path -lyajl


#    #Add common files to the output folder to run test easily on UNIX the destdir need to be specified, on windows taken from project configuration (TOFIX)
#    common_files.files += $$PWD/common_files/*.json
#    common_files.path += $$PWD/../build-agent0_crossplateform-Desktop-Debug
#    INSTALLS += common_files
#}

#macx : {
#    LIBS += -L$$PWD/../../../../../../usr/local/lib/ -lczmq.3
#    LIBS += -L$$PWD/../../../../../../usr/local/lib/ -lzmq.5
#    LIBS += -L$$PWD/../../../../../../usr/local/lib/ -lzyre.1
#    LIBS += -L$$PWD/../../../../../../usr/local/Cellar/libsodium/1.0.10/lib/ -lsodium.18

#    INCLUDEPATH += $$PWD/../../../../../../usr/local/Cellar/libsodium/1.0.10/include
#    DEPENDPATH += $$PWD/../../../../../../usr/local/Cellar/libsodium/1.0.10/include
#}

#--------- COMMON ---------#

##Add headers from dependencies
INCLUDEPATH += $$PWD/libzmq/include \
               $$PWD/czmq/include \
               $$PWD/zyre/include \
               $$PWD/yajl/lloyd-yajl-2.1.0/build/yajl-2.1.0/include

DEPENDPATH += $$PWD/libzmq/include \
              $$PWD/czmq/include \
              $$PWD/zyre/include \
              $$PWD/yajl/lloyd-yajl-2.1.0/build/yajl-2.1.0/include


