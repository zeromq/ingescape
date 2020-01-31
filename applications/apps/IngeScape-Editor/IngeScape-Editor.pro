#####################################################################
#
# Generic (multi-platform) rules
#
#####################################################################

TEMPLATE = app


# Application version
VERSION_MAJOR = 0
VERSION_MINOR = 9
VERSION_PATCH = 1
VERSION_BUILD = 0

win32:VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_PATCH}.$${VERSION_BUILD}
else:VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_PATCH}

DEFINES += "VERSION=\\\"$$VERSION\\\""

# Enable function names and line numbers even for release builds
DEFINES += QT_MESSAGELOGCONTEXT

# Qt modules used by our application and/or its dependencies
QT += qml quick quick-private svg xml concurrent core-private gui-private webview

CONFIG += c++11 precompiled_header

# Warnings = error
QMAKE_CXXFLAGS += -Werror

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER  = stable.h


# To build temp files (*.o, moc_*.cpp and qrc_*.cpp) in a temp dir
OBJECTS_DIR = tmp
MOC_DIR = tmp
RCC_DIR = tmp
UI_DIR = tmp


SOURCES += main.cpp \
    controller/agentssupervisioncontroller.cpp \
    controller/agentsmappingcontroller.cpp \
    controller/ingescapeeditorcontroller.cpp \
    controller/networkcontroller.cpp \
    controller/actioneditorcontroller.cpp \
    controller/scenariocontroller.cpp \
    controller/valueshistorycontroller.cpp \
    misc/collapsiblecolumn.cpp \
    model/actionmappingm.cpp \
    sortFilter/agentsinhostsortfilter.cpp \
    sortFilter/valueshistorysortfilter.cpp \
    controller/hostssupervisioncontroller.cpp \
    controller/recordssupervisioncontroller.cpp \
    controller/logstreamcontroller.cpp \
    model/logm.cpp \
    sortFilter/logssortfilter.cpp \
    misc/qquickwindowblocktouches.cpp \
    misc/textfielddoublevalidator.cpp \
    misc/numberconstants.cpp \
    viewModel/agentinmappingvm.cpp \
    viewModel/hostvm.cpp \
    viewModel/recordvm.cpp \
    viewModel/link/linkinputvm.cpp \
    viewModel/link/linkoutputvm.cpp \
    viewModel/link/linkvm.cpp \
    viewModel/link/linkconnectorvm.cpp \
    controller/editormodelmanager.cpp \
    viewModel/mapping/objectinmappingvm.cpp \
    viewModel/mapping/actioninmappingvm.cpp

HEADERS += \
    model/actionmappingm.h \
    controller/scenariocontroller.h \
    sortFilter/agentsinhostsortfilter.h \
    stable.h \
    controller/agentssupervisioncontroller.h \
    controller/agentsmappingcontroller.h \
    controller/ingescapeeditorcontroller.h \
    controller/networkcontroller.h \
    controller/actioneditorcontroller.h \
    controller/valueshistorycontroller.h \
    misc/collapsiblecolumn.h \
    sortFilter/valueshistorysortfilter.h \
    controller/hostssupervisioncontroller.h \
    controller/recordssupervisioncontroller.h \
    controller/logstreamcontroller.h \
    model/logm.h \
    sortFilter/logssortfilter.h \
    misc/qquickwindowblocktouches.h \
    misc/textfielddoublevalidator.h \
    misc/numberconstants.h \
    viewModel/agentinmappingvm.h \
    viewModel/hostvm.h \
    viewModel/recordvm.h \
    viewModel/link/linkinputvm.h \
    viewModel/link/linkoutputvm.h \
    viewModel/link/linkvm.h \
    viewModel/link/linkconnectorvm.h \
    controller/editormodelmanager.h \
    viewModel/mapping/objectinmappingvm.h \
    viewModel/mapping/actioninmappingvm.h


RESOURCES += qml.qrc



# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS


# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


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
INCLUDEPATH += C:\Users\admin\dev\install_dir\include

# Include ingescape library
!include(../../../builds/ingescape.pri) {
    error(Could not load ingescape.pri)
}


# Include PGIMCommon library
!include(../IngeScape-Common/IngeScape-Common.pri) {
    error(Could not load Ingescape-Common.pri)
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
    # NB: libzyre, libzmq, libczmq, libsodium, have to be copied manually because macdeploy won't copy libraries referenced through @rpath
    librariesToCopy.files += ../../frameworks/I2Quick/Mac/libI2Quick.$${QMAKE_EXTENSION_SHLIB} /usr/local/lib/libczmq.dylib /usr/local/lib/libzyre.dylib /usr/local/lib/libsodium.dylib
    librariesToCopy.path = Contents/Frameworks
    QMAKE_BUNDLE_DATA += librariesToCopy


    # Release / debug specific rules
    CONFIG(release, debug|release) {
        # We must call install_name_tool to create a valid link. Otherwise, our application will not find our library
        QMAKE_POST_LINK += $$quote(install_name_tool -change libI2Quick.$${QMAKE_EXTENSION_SHLIB} @executable_path/../Frameworks/libI2Quick.$${QMAKE_EXTENSION_SHLIB} $${OUT_PWD}/$${TARGET}.app/Contents/MacOS/$${TARGET} $$escape_expand(\\n\\t))

        # Release only: copy Qt libs and plugins inside our application to create a standalone application
        # NB: macdeployqt only runs qmlimportscanner correctly when run from Qt bin directory
        QMAKE_POST_LINK += $$quote(cd `dirname $(QMAKE)` && macdeployqt $${OUT_PWD}/$${TARGET}.app -qmldir=$${PWD} $$escape_expand(\n\t))
    }
    else {
        # We must call install_name_tool to create a valid link. Otherwise, our application will not find our library
        # NB: useless in debug mode because Qt Creator can find our I2Quick library
       # QMAKE_POST_LINK += $$quote(install_name_tool -change libI2Quick_debug.$${QMAKE_EXTENSION_SHLIB} @executable_path/libI2Quick_debug.$${QMAKE_EXTENSION_SHLIB} $${OUT_PWD}/$${TARGET}.app/Contents/MacOS/$${TARGET} $$escape_expand(\\n\\t))
    }


    # Set icon of our executable
    ICON = icon_IGS.icns


    # Plist file
    QMAKE_INFO_PLIST = macos-Info.plist
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
        QMAKE_POST_LINK += $$QMAKE_COPY $$shell_quote($$shell_path($${PROGRAM_FILES_SYS}\ingescape\lib\*.dll)) $$shell_quote($$shell_path($${DESTDIR})) $$escape_expand(\n\t)

        # Copy Qt dlls
        # NB: Some Qt libs must be explictly referenced because their are used by I2Quick.dll and not our .exe
        QMAKE_POST_LINK += $$shell_quote($$shell_path($$[QT_INSTALL_BINS]/windeployqt)) $${DESTDIR}/$${TARGET}.exe -xml -concurrent -printsupport -qmldir=$${PWD} $$escape_expand(\n\t)
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
    QMAKE_TARGET_DESCRIPTION = IngeScape-Editor
    QMAKE_TARGET_COPYRIGHT = Copyright (c) 2017-2019, Ingenuity i/o
}






#------------------------
#
# Linux specific
#
#------------------------
unix:!mac {
    message(Linux specific rules)

    QMAKE_CXXFLAGS -= -Werror

    # Compute the LFLAG associated to our frameworks
    LIBS += -L../../frameworks/I2Quick/Unix -lI2Quick

    # Copy libraries into the build directory of our application
    librariesToCopy.files += ../../frameworks/I2Quick/Unix/libI2Quick.$${QMAKE_EXTENSION_SHLIB}
    librariesToCopy.path = $${DESTDIR}
    QMAKE_BUNDLE_DATA += librariesToCopy
}

# Just for search and replace and avoid error with CMake not updated
DISTFILES += \
    ../../../builds/cmake/modules/IngescapeEditorHelper.cmake \
    ../../../builds/cmake/modules/IngescapeHelper.cmake \
    ../../../builds/cmake/modules/Windeploy.cmake \
    ../../../builds/cmake/modules/apps/FileAssociation.nsh \
    ../../../builds/cmake/modules/apps/NSIS.template.in \
    CMakeLists.txt
