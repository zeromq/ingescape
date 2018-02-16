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

    INCLUDEPATH += $$PWD/../dependencies/windows/unix \
                   $$PWD/../dependencies/windows/headers \
                   $$PWD/../dependencies/windows/zyre_suite \

    #Add librairies
    LIBS += -L$$libs_path -llibzyre -llibczmq -lyajl

    #To get the Ip address into the network.c
    LIBS += -L$$C:/Windows/System32 -lwsock32 -lIPHLPAPI -lws2_32

    #include the pri to copy files to C:\
    include ("$$PWD/../dependencies/windows/common/pri/mastic-job-copy.pri")
}

macx:{
    message("Compilation macx scope ...")

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
    raspberry_compilation {
        ############ Raspberry ###########
    message("Compilation raspberry scope ...")

    libyajl_path = $$PWD/yajl/yajl-2.1.1/Raspberry
    libzyre_path = $$PWD/zyre/bin/Raspberry

    #Destination repository for our librairy
    DESTDIR = /usr/local/lib

    #Add librairies
    LIBS += -L$$libzyre_path -lzmq -lczmq -lzyre \
            -L$$libyajl_path/lib -lyajl

    #Copy all zyre includes
    install_headers.files += $$PWD/../src/include/*.h \
                             $$PWD/../src/include/uthash
    install_headers.path += $$/usr/local/include/mastic

    #Copy zyre and friends libs
    install_zyre_libs.files += $$libzyre_path/*
    install_zyre_libs.path += $$DESTDIR

    #Copy yajl libs
    install_yajl_libs.files += $$libyajl_path/lib/*
    install_yajl_libs.path += $$DESTDIR

    #Add installation options
    INSTALLS += install_headers
    INSTALLS += install_zyre_libs
    INSTALLS += install_yajl_libs
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

    #TODO : gérer les espaces dans les chemins donc copie à la main dans le C et link temporaire
    #temp to manage the space in the path
#    android_libzyre_path = "C:\mastic\lib\android\armeabi-v7a"
#    android_libyajl_path = "C:\mastic\lib\android\armeabi-v7a"

    LIBS += $$quote(-L$$android_libzyre_path/) -lzmq -lczmq -lzyre \
            $$quote(-L$$android_libyajl_path/) -lyajl

    ############ Copy needed in C:\ ############
    #NB: Copy includes normally already with windows

    #Add the make step 'install' to copy the .a & .so files
    install_libs.path += "C:/mastic/lib/android/armeabi-v7a"
    install_libs.files += $$android_libzyre_path/* \
                          $$android_libyajl_path/* \
                          $$OUT_PWD/*.so

    #Add installation options
    INSTALLS += install_libs \

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

#--------- COMMON ---------#

##Add headers from dependencies
zyre_include_path = $$PWD/../dependencies/windows/headers/zyre_suite
yajl_include_path = $$PWD/../dependencies/windows/headers/

INCLUDEPATH += $$zyre_include_path \
               $$yajl_include_path \

DEPENDPATH += $$zyre_include_path \
              $$yajl_include_path \


