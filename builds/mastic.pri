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
        librairies_path = $$PWD/../dependencies\windows\libs\win32\Debug
    }else {
        #configuration RELEASE
        librairies_path = $$PWD/../dependencies\windows\libs\win32\Release
    }

    #Add librairies
    LIBS += -L$$librairies_path -llibzmq -llibczmq -llibzyre -lyajl

    #to get the Ip address into the network.c
    LIBS += -L$$C:/Windows/System32 -lwsock32 -lIPHLPAPI -lws2_32

    INCLUDEPATH += $$PWD/../dependencies/windows/headers/zyre_suite \
                   $$PWD/../dependencies/windows/headers \
                   $$PWD/../dependencies/windows/unix \

    #UNIX functions
    SOURCES += $$PWD/../dependencies/windows/unix/unixfunctions.c \

    HEADERS += $$PWD/../dependencies/windows/unix/unixfunctions.h \
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

    libyajl_path = $$PWD/yajl/yajl-2.1.1/Raspberry
    librairies_path = $$PWD/zyre/bin/Raspberry

    #Add librairies
    LIBS += -L$$librairies_path -lzmq -lczmq -lzyre \
            -L$$libyajl_path/lib -lyajl
    }

    android_compilation {
        ############ Android ###########
    message("Compilation android scope ...")

    # This define is used in "network.c" to use the "ifaddrs.h" for android but only to pass the compilation
    # After we need to use the newest functions : "mtic_start_ip" & "init_actor_ip" instead of "mtic_start" & "init_actor"
    # Because getting the Ip Address dynamically by "ifaddrs.c" doesnt work
    DEFINES +=  ANDROID

    INCLUDEPATH += $$PWD/android-ifaddrs-master/ \

    SOURCES += $$PWD/android-ifaddrs-master/ifaddrs.c \

    HEADERS += $$PWD/android-ifaddrs-master/ifaddrs.h \

    #android_libs_path = $$PWD/../builds/android/libs/armeabi-v7a
    android_libzyre_path = $$PWD/zyre/bin/Android/armeabi-v7a
    android_libyajl_path = $$PWD/yajl/lloyd-yajl-2.1.0/Android/armeabi-v7a

    LIBS += -L$$android_libzyre_path/ -lzmq -lczmq -lzyre \
            -L$$android_libyajl_path/ -lyajl

    ############ Copy needed in C:\ ############
    #NB: Copy includes normally already with windows
    }

    !raspberry_compilation:!android_compilation {
        ############ Linux ###########
    message("Compilation Linux scope ...")

    libzyre_path = $$PWD/zyre/bin/Linux
    libyajl_path = $$PWD/yajl/lloyd-yajl-2.1.0/Linux/lib

    LIBS += -L$$libzyre_path -lzmq -lczmq -lzyre \
            -L$$libyajl_path -lyajl
    }
}


