

#IngeScape.h
header_source_path = $$shell_path($$clean_path($$absolute_path($$PWD/../../../../src/include/ingescape.h)))
header_dest_path = $$shell_path($$clean_path("C:/ingescape/include/*"))

#Create a command, using the 'cmd' command line and Window's 'xcopy'
copyHeaders.commands = $$quote(cmd /c xcopy /S /Y /I $${header_source_path} $${header_dest_path})

#dll
win32:{
    dependencies_src_path = $$shell_path($$clean_path($$absolute_path( $${libs_path}/*)))
    ingescape_lib_src_path = $$shell_path($$clean_path($$absolute_path( $$DESTDIR/ingescape.*)))

    CONFIG(debug, debug|release){
        dependencies_dest_path = $$shell_path($$clean_path("C:/ingescape/libs/debug/*"))
    }else {
        dependencies_dest_path = $$shell_path($$clean_path("C:/ingescape/libs/release/*"))
    }

    copyDependencies.commands = $$quote(cmd /c xcopy /S /Y /I $${dependencies_src_path} $${dependencies_dest_path})
    copyIngeScapeLib.commands = $$quote( cmd /c xcopy /S /Y /I $${ingescape_lib_src_path} $${dependencies_dest_path} )

    #Add the command to Qt.
    QMAKE_EXTRA_TARGETS += copyHeaders \
                           copyDependencies \
                           copyIngeScapeLib \

    POST_TARGETDEPS += copyHeaders \
                       copyDependencies \
                       copyIngeScapeLib \
}

unix:{

    raspberry_compilation{
        #
        # !! TO COPY YOU NEED TO make install !!
        #

        #Copy all zyre includes
        install_headers.files += $$PWD/../src/include/*.h \
                                 $$PWD/../src/include/uthash #to future developpement of agents
        install_headers.path += $$/usr/local/include/ingescape

        #Copy zyre and friends libs + yajl
        install_libs.files += $$libs_path/*
        install_libs.path += $$DESTDIR

        #Add installation options
        INSTALLS += install_headers
        INSTALLS += install_libs
    }

    android_compilation{
        dependencies_src_path = $$shell_path($$clean_path($$absolute_path( $${libs_path}/lib*)))
        ingescape_lib_src_path = $$shell_path($$clean_path($$absolute_path( $$OUT_PWD/android-build/libs/armeabi-v7a/libingescape.so)))

        message($$ingescape_lib_src_path)

        dependencies_dest_path = $$shell_path($$clean_path("C:/ingescape/libs/android/armeabi-v7a/"))

        copyDependencies.commands = $$quote(cmd /c xcopy /S /Y /I $${dependencies_src_path} $${dependencies_dest_path})
        copyIngeScapeLib.commands = $$quote( cmd /c xcopy /Y $${ingescape_lib_src_path} $${dependencies_dest_path} )

        #Add the command to Qt.
        QMAKE_EXTRA_TARGETS += copyHeaders \
                               copyDependencies \
                               copyIngeScapeLib \

        POST_TARGETDEPS += copyHeaders \
                           copyDependencies \
                           copyIngeScapeLib \
    }
}
