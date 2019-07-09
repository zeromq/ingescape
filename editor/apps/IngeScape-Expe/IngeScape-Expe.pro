#####################################################################
#
# Generic (multi-platform) rules
#
#####################################################################

TEMPLATE = app


# Application version
VERSION_MAJOR = 0
VERSION_MINOR = 1
VERSION_PATCH = 0
VERSION_BUILD = 0


win32:VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_PATCH}.$${VERSION_BUILD}
else:VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_PATCH}

DEFINES += "VERSION=\\\"$$VERSION\\\""

# Qt modules used by our application and/or its dependencies
QT += qml quick quick-private svg xml concurrent sql core-private gui-private

CONFIG += c++11 precompiled_header

# Warnings = error
QMAKE_CXXFLAGS += -Werror

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER  = stable.h


# To build temp files (*.o, moc_*.cpp and qrc_*.cpp) in a temp dir
OBJECTS_DIR = tmp
MOC_DIR = tmp
RCC_DIR = tmp


SOURCES += \
        controller/expemodelmanager.cpp \
        controller/ingescapeexpecontroller.cpp \
        controller/networkcontroller.cpp \
        main.cpp \
        model/platformm.cpp


HEADERS += \
    controller/expemodelmanager.h \
    controller/ingescapeexpecontroller.h \
    controller/networkcontroller.h \
    model/platformm.h \
    stable.h


RESOURCES += qml.qrc


# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


#####################################################################
#
# Additionnal code to use frameworks (I2Quick, IngeScape-Common, etc.)
#
#####################################################################

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = ../../frameworks .

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =


# Add the include directory of I2Quick
INCLUDEPATH += ../../frameworks/I2Quick/include

# Include ingescape library
!include(../../../builds/ingescape.pri) {
    error(Could not load ingescape.pri)
}

# Include IngeScape-Common library
!include(../IngeScape-Common/IngeScape-Common.pri) {
    error(Could not load IngeScape-Common.pri)
}


#####################################################################
#
# OS specific rules
#
#####################################################################


#------------------------
#
# OSX specific rules
#
#------------------------
mac {
    message(macOS and iOS specific rules)

    # Compute the LFLAG associated to our frameworks
    LIBS += -L../../frameworks/I2Quick/Mac -lI2Quick


    # Copy libraries into the MacOS directory of our application
    librariesToCopy.files += ../../frameworks/I2Quick/Mac/libI2Quick.$${QMAKE_EXTENSION_SHLIB}
    librariesToCopy.path = Contents/Frameworks
    QMAKE_BUNDLE_DATA += librariesToCopy
    # NB: libzyre, libzmq, libczmq, libsodium, libyajl will be copied by macdeployqt
    #     because they are installed in a standard directory (/usr/local/lib)


    # Release / debug specific rules
    CONFIG(release, debug|release) {
        # We must call install_name_tool to create a valid link. Otherwise, our application will not found our library
        QMAKE_POST_LINK += $$quote(install_name_tool -change libI2Quick.$${QMAKE_EXTENSION_SHLIB} @executable_path/../Frameworks/libI2Quick.$${QMAKE_EXTENSION_SHLIB} $${OUT_PWD}/$${TARGET}.app/Contents/MacOS/$${TARGET} $$escape_expand(\\n\\t))

        # Release only: copy Qt libs and plugins inside our application to create a standalone application
        # NB: macdeployqt only runs qmlimportscanner correctly when run from Qt bin directory
        QMAKE_POST_LINK += $$quote(cd `dirname $(QMAKE)` && macdeployqt $${OUT_PWD}/$${TARGET}.app -qmldir=$${PWD} $$escape_expand(\n\t))
    }
    else {
        # We must call install_name_tool to create a valid link. Otherwise, our application will not found our library
        # NB: useless in debug mode because Qt Creator can find our I2Quick library
       # QMAKE_POST_LINK += $$quote(install_name_tool -change libI2Quick_debug.$${QMAKE_EXTENSION_SHLIB} @executable_path/libI2Quick_debug.$${QMAKE_EXTENSION_SHLIB} $${OUT_PWD}/$${TARGET}.app/Contents/MacOS/$${TARGET} $$escape_expand(\\n\\t))
    }


    # Set icon of our executable
    ICON = icon_IGS.icns

}


#------------------------
#
# Windows specific rules
#
#------------------------
win32 {
    message(Windows specific rules)

    # Warnings = error
    QMAKE_CXXFLAGS -= -Werror
#    QMAKE_CXXFLAGS += /WX

    # Custom DESTDIR to avoid useless files (pch, etc.)
    DESTDIR = $${OUT_PWD}/bin

    # Detect 32 or 64 bits
    contains(QMAKE_HOST.arch, x86_64) {
        ARCHITECTURE = 64
    } else {
        ARCHITECTURE = 32
    }

    # Compute the LFLAG associated to our frameworks
    CONFIG(debug, debug|release) {
       LIBS += -L../../frameworks/I2Quick/Win$${ARCHITECTURE} -lI2Quickd

       librariesToCopy.files += ../../frameworks/I2Quick/Win$${ARCHITECTURE}/I2Quickd.$${QMAKE_EXTENSION_SHLIB}
    }
    else {
       # Compute the LFLAG associated to our I2Quick library
       LIBS += -L../../frameworks/I2Quick/Win$${ARCHITECTURE} -lI2Quick

       librariesToCopy.files += ../../frameworks/I2Quick/Win$${ARCHITECTURE}/I2Quick.$${QMAKE_EXTENSION_SHLIB}
    }


    # Copy libraries in our application directory
    librariesToCopy.path = $${DESTDIR}
    INSTALLS += librariesToCopy


    # Release only: copy Qt libs and plugins next to our application to create a standalone application
    CONFIG(release, debug|release) {
        # copy I2Quick (if we don't call make install)
        QMAKE_POST_LINK += $$QMAKE_COPY $$shell_quote($$shell_path(../../frameworks/I2Quick/Win$${ARCHITECTURE}/I2Quick.$${QMAKE_EXTENSION_SHLIB})) $$shell_quote($$shell_path($${DESTDIR})) $$escape_expand(\n\t)

        IS64BITSYSTEM = $$(ProgramW6432)
        # Get Program Files folder, given x86/x64 architecture
        isEmpty(IS64BITSYSTEM) {
            PROGRAM_FILES_SYS = $$(ProgramFiles)
        } else {
            contains(QMAKE_HOST.arch, x86_64) {
                PROGRAM_FILES_SYS = $$(ProgramW6432)
            } else {
                PROGRAM_FILES_SYS = $$(ProgramFiles)
            }
        }

        #copy ingescape, zyre and friends to make a real standalone .exe
        #copy ingescape, zyre and friends to make a real standalone .exe
        QMAKE_POST_LINK += $$QMAKE_COPY $$shell_quote($$shell_path($${PROGRAM_FILES_SYS}\ingescape\lib\*.dll)) $$shell_quote($$shell_path($${DESTDIR})) $$escape_expand(\n\t)

        # Copy Qt dlls
        # NB: Some Qt libs must be explictly referenced because their are used by I2Quick.dll and not our .exe
        QMAKE_POST_LINK += $$shell_quote($$shell_path($$[QT_INSTALL_BINS]/windeployqt)) $${DESTDIR}/$${TARGET}.exe -xml -concurrent -printsupport -sql -qmldir=$${PWD} $$escape_expand(\n\t)
    }
    else {
        # copy I2Quick (if we don't call make install)
        QMAKE_POST_LINK += $$QMAKE_COPY $$shell_quote($$shell_path(../../frameworks/I2Quick/Win$${ARCHITECTURE}/I2Quickd.$${QMAKE_EXTENSION_SHLIB})) $$shell_quote($$shell_path($${DESTDIR})) $$escape_expand(\n\t)
    }


    # Set icon of our executable
    RC_ICONS = icon_IGS.ico

    # Set application info
    QMAKE_TARGET_COMPANY = Ingenuity i/o
    QMAKE_TARGET_PRODUCT = $${TARGET}
    QMAKE_TARGET_DESCRIPTION = IngeScape-Expe
    QMAKE_TARGET_COPYRIGHT = Copyright (c) 2019, Ingenuity i/o
}






#------------------------
#
# Linux specific
#
#------------------------
unix:!mac {
    message(Linux specific rules)

    # TODO if needed
}

