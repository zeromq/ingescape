#####################################################################
#
# Generic (multi-platform) rules
#
#####################################################################


SOURCES += \
    $$PWD/../src/definition.c \
    $$PWD/../src/mapping.c \
    $$PWD/../src/model.c \
    $$PWD/../src/network.c \
    $$PWD/../src/parser.c \
    $$PWD/../src/admin.c

HEADERS += \
    $$PWD/../src/include/uthash/uthash.h \
    $$PWD/../src/include/mastic.h \
    $$PWD/../src/include/mastic_private.h \

INCLUDEPATH += $$PWD/../src/include




#####################################################################
#
# OS specific rules
#
#####################################################################


#add librairies zyre, czmq, zmq + yajl and configurate only for VS 2015 x86 (32 bits)
win32:{
    message("Compilation win32 scope ...")

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

    #Add librairies
    LIBS += -L$$libs_path -llibzyre -llibczmq -lyajl

    #To get the Ip address into the network.c
    LIBS += -L$$C:/Windows/System32 -lwsock32 -lIPHLPAPI -lws2_32

    ##Add headers from dependencies
    zyre_include_path = $$PWD/../dependencies/windows/headers/zyre_suite
    yajl_include_path = $$PWD/../dependencies/windows/headers/

    INCLUDEPATH += $$zyre_include_path \
                   $$yajl_include_path \
                   $$PWD/../dependencies/windows/unix \

    DEPENDPATH += $$zyre_include_path \
                  $$yajl_include_path \

    #include the pri to copy files to C:\
    include ("$$PWD/../dependencies/windows/common/pri/mastic-job-copy.pri")
}



#
# Mac and iOS
#
mac:{
    message("Compilation macOs and iOS scope ...")

    # NB: use ios { } for ios sub-rules


    #------------------------------------------------
    #
    # Include paths
    #
    #------------------------------------------------

    # zyre, zmq, czmq, sodium, yajl
    INCLUDEPATH += /usr/local/include


    #------------------------------------------------
    #
    # List of libraries to be linked into the project
    #
    # i.e. zyre, zmq, czmq, sodium, yajl
    #
    #------------------------------------------------

    #
    # Option 1: generic version (relative paths)
    # - pros: add /usr/local/lib to the list of search directories of our application
    #         It can help it to find extra librairies
    #
    # - cons: may create a conflict with brew installs (libjpeg, libpng, libtiff)

    #LIBS += -L/usr/local/lib -lzmq -lczmq -lzyre -lyajl


    #
    # Option 2: specific version (absolute paths)
    # - pros: avoid to add /usr/local/lib to the list of search directories of our application
    #         It prevents conflict with brew installs (libjpeg, libpng, libtiff)
    #
    # - cons: all required librairies must be linked explictly
    #         AND does not work if libraries are installed in another directory
    LIBS += /usr/local/lib/libzmq.dylib
    LIBS += /usr/local/lib/libczmq.dylib
    LIBS += /usr/local/lib/libzyre.dylib
    LIBS += /usr/local/lib/libyajl.dylib
}



#
# Unix except macOS and iOS
#
unix:!mac {
    raspberry_compilation {
        ############ Raspberry ###########
    message("Compilation raspberry scope ...")

    QMAKE_CFLAGS_DEBUG = \
        -std=gnu99

    QMAKE_CFLAGS_RELEASE = \
        -std=gnu99

    libs_path = $$PWD/../dependencies/raspberry/libs

    #Destination repository for our librairy
    DESTDIR = /usr/local/lib

    #Add librairies
    LIBS += -L$$libs_path -lzmq -lczmq -lzyre -lyajl

    #include the pri to copy files to usr/local/libs
    include ("$$PWD/../dependencies/windows/common/pri/mastic-job-copy.pri")
    }

    android_compilation {
        ############ Android ###########
    message("Compilation android scope ...")

    QMAKE_CFLAGS_DEBUG = \
        -std=gnu99

    QMAKE_CFLAGS_RELEASE = \
        -std=gnu99

    # This define is used in "network.c" to use the "ifaddrs.h" for android but only to pass the compilation
    # After we need to use the newest functions : "mtic_start_ip" & "init_actor_ip" instead of "mtic_start" & "init_actor"
    # Because getting the Ip Address dynamically by "ifaddrs.c" doesnt work
    DEFINES +=  ANDROID

    INCLUDEPATH += $$PWD/../dependencies/android/android-ifaddrs-master/ \

    SOURCES += $$PWD/../dependencies/android/android-ifaddrs-master/ifaddrs.c \

    HEADERS += $$PWD/../dependencies/android/android-ifaddrs-master/ifaddrs.h \

    libs_path = $$PWD/../dependencies/android/libs-armeabi-v7a
    LIBS += $$quote(-L$$libs_path/) -lzmq -lczmq -lzyre -lyajl \

    ############ Copy needed in C:\ ############
    #include the pri to copy files to C:\
    include ("$$PWD/../dependencies/windows/common/pri/mastic-job-copy.pri")
    }

    !raspberry_compilation:!android_compilation {
        ############ Linux ###########
    message("Compilation Linux scope ...")

    libzyre_path = $$PWD/zyre/bin/Linux
    libyajl_path = $$PWD/yajl/lloyd-yajl-2.1.0/Linux/lib

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
}
