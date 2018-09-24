#####################################################################
#
# Generic (multi-platform) rules
#
#####################################################################

TEMPLATE = app


# Application version
VERSION_MAJOR = 0
VERSION_MINOR = 0
VERSION_BUILD = 0
VERSION_PATCH = 0

VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_BUILD}.$${VERSION_PATCH}

# To get the version in our C++ code
DEFINES += "VERSION=\\\"$$VERSION\\\""


# Qt modules used by our application
QT += qml quick widgets concurrent svg xml scxml location positioning

CONFIG += c++11 precompiled_header


# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS


# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


# Warnings = error
QMAKE_CXXFLAGS += -Werror

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER  = stable.h

# To build temp files (*.o, moc_*.cpp and qrc_*.cpp) in a temp dir
OBJECTS_DIR = tmp
MOC_DIR = tmp
RCC_DIR = tmp


SOURCES += main.cpp \
    utils/resourcehelpers.cpp \
    utils/qmlsyntaxhighlighter.cpp \
    utils/propertieshelpers.cpp \
    models/playgroundexample.cpp \
    controllers/playgroundcontroller.cpp


HEADERS += \
    stable.h \
    utils/propertieshelpers.h \
    utils/resourcehelpers.h \
    utils/qmlsyntaxhighlighter.h \
    models/playgroundexample.h \
    controllers/playgroundcontroller.h


RESOURCES += qml.qrc


# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = ../ .

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =


# Include IngeScapeQuick
!include(../IngeScapeQuick/IngeScapeQuick.pri) {
    error(Could not load IngeScapeQuick.pri)
}




#####################################################################
#
# OS specific rules
#
#####################################################################


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target



#
# Windows specific rules
#
win32 {
    message(Windows specific rules)

    # Warnings = error
    QMAKE_CXXFLAGS -= -Werror
    QMAKE_CXXFLAGS += /WX


    # Set icon of our executable
    RC_ICONS = icon.ico

    # Set application info
    QMAKE_TARGET_COMPANY = Ingenuity i/o
    QMAKE_TARGET_PRODUCT = $${TARGET}
    QMAKE_TARGET_DESCRIPTION = Playground to code ingeScape agents with QML
    QMAKE_TARGET_COPYRIGHT = Copyright (c) 2018, Ingenuity i/o


    # Release only: copy Qt libs and plugins next to our application to create a standalone application
    CONFIG(release, debug|release) {
        #NB: use a custom directory because OUT_PWD contains cpp (moc_) and obj files
        DESTDIR = $${OUT_PWD}/bin

        # Copy Qt DLLs and required files
        QMAKE_POST_LINK += windeployqt $${DESTDIR}/$${TARGET}.exe -qmldir=$${PWD}

        # TODO: copy ingeScape dependencies
    }

}




#
# Mac and iOS
#
mac {
    message(macOS and iOS specific rules)

    !ios {
        # Release build
        CONFIG(release, debug|release) {
            # Release only: copy Qt libs and plugins inside our application to create a standalone application
            # NB: macdeployqt only runs qmlimportscanner correctly when run from Qt bin directory
            # TODO: find a way to deploy geoservices plugins (osm, etc.)
            QMAKE_POST_LINK += $$quote(cd `dirname $(QMAKE)` && macdeployqt $${OUT_PWD}/$${TARGET}.app -qmldir=$${PWD} $$escape_expand(\n\t))
        }
    }

    # Set icon of our executable
    ICON = icon.icns
}




#
# Unix specific
#
unix:!mac {
    message(Unix specific rules)

    # TODO if needed
}
