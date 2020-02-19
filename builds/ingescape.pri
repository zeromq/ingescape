#####################################################################
#
# Generic (multi-platform) rules
#
#####################################################################

DEFINES += INGESCAPE_FROM_PRI

SOURCES += \
    $$PWD/../src/definition.c \
    $$PWD/../src/mapping.c \
    $$PWD/../src/model.c \
    $$PWD/../src/network.c \
    $$PWD/../src/parser.c \
    $$PWD/../src/admin.c \
    $$PWD/../src/bus.c \
    $$PWD/../src/call.c \
    $$PWD/../src/license.c \
    $$PWD/../src/monitor.c \
    $$PWD/../src/json.c \
    $$PWD/../src/performance.c \
    $$PWD/../src/global.c \
    $$PWD/../src/agent.c \
    $$PWD/../dependencies/yajl/src/yajl_alloc.c \
    $$PWD/../dependencies/yajl/src/yajl_buf.c \
    $$PWD/../dependencies/yajl/src/yajl_encode.c \
    $$PWD/../dependencies/yajl/src/yajl_gen.c \
    $$PWD/../dependencies/yajl/src/yajl_lex.c \
    $$PWD/../dependencies/yajl/src/yajl_parser.c \
    $$PWD/../dependencies/yajl/src/yajl_tree.c \
    $$PWD/../dependencies/yajl/src/yajl_version.c \
    $$PWD/../dependencies/yajl/src/yajl.c

HEADERS += \
    $$PWD/../src/include/uthash/uthash.h \
    $$PWD/../src/include/ingescape.h \
    $$PWD/../src/include/ingescape_advanced.h \
    $$PWD/../src/include/ingescape_private.h \
    $$PWD/../src/include/ingescape_agent.h \
    $$PWD/../dependencies/yajl/src/yajl_alloc.h\
    $$PWD/../dependencies/yajl/src/yajl_buf.h \
    $$PWD/../dependencies/yajl/src/yajl_bytestack.h \
    $$PWD/../dependencies/yajl/src/yajl_encode.h \
    $$PWD/../dependencies/yajl/src/yajl_lex.h \
    $$PWD/../dependencies/yajl/src/yajl_parser.h

INCLUDEPATH += $$PWD/../src/include \
               $$PWD/../dependencies/yajl/src/api


#####################################################################
#
# Dependencies
#
#####################################################################

# Include libsodium library
!include(../../libsodium/builds/qt/libsodium.pri) {
    error(Could not load libsodium.pri)
}

# Include ZeroMQ library
!include(../../libzmq/builds/qt/libzmq.pri) {
    error(Could not load libzmq.pri)
}

# Include C ZeroMQ library
!include(../../czmq/builds/qt/czmq.pri) {
    error(Could not load czmq.pri)
}

# Include zyre library
!include(../../zyre/builds/qt/zyre.pri) {
    error(Could not load zyre.pri)
}


#####################################################################
#
# OS specific rules
#
#####################################################################


#add librairies zyre, czmq, zmq and configurate only for VS 2015 x86 (32 bits)
win32:{
    message("Scope is win32...")

    CONFIG(debug, debug|release){
        #configuration DEBUG
        DESTDIR = $$OUT_PWD/debug
    }else {
        #configuration RELEASE
        DESTDIR = $$OUT_PWD/release
    }

    #UNIX functions
    SOURCES += $$PWD/../dependencies/windows/unix/unixfunctions.c

    HEADERS += $$PWD/../dependencies/windows/unix/unixfunctions.h

    #To get the Ip address into the network.c
    LIBS += -L$$C:/Windows/System32 -lwsock32 -lIPHLPAPI -lws2_32

    INCLUDEPATH += $$PWD/../dependencies/windows/unix
}



#
# Mac and iOS
#
mac:{
    message("Scope is macos...")

    #
    # Option 1: generic version (relative paths)
    # - pros: add /usr/local/lib to the list of search directories of our application
    #         It can help it to find extra librairies
    #
    # - cons: may create a conflict with brew installs (libjpeg, libpng, libtiff)

    #
    # Option 2: specific version (absolute paths)
    # - pros: avoid to add /usr/local/lib to the list of search directories of our application
    #         It prevents conflict with brew installs (libjpeg, libpng, libtiff)
    #
    # - cons: all required librairies must be linked explictly
    #         AND does not work if libraries are installed in another directory
    LIBS += -framework CoreFoundation
}

#
# Unix except macOS and iOS
#
unix:!mac {
    message("Scope is unix...")

    #TODO


    ############ Android ###########
    android {

        message("Compilation android scope...")

        # Add librairies
        libs_path = $$PWD/../dependencies/android/libs-armeabi-v7a
    }
}
