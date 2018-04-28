#####################################################################
#
# Generic (multi-platform) rules
#
#####################################################################


TEMPLATE = lib
TARGET = IngeScapeQuick

CONFIG += plugin c++11

TARGET = $$qtLibraryTarget($$TARGET)
uri = IngeScapeQuick

# Defines INGESCAPEQUICK_BUILD_LIBRARY to explictly export symbols
DEFINES += INGESCAPEQUICK_BUILD_LIBRARY


# To build temp files (*.o, moc_*.cpp and qrc_*.cpp) in a temp dir
OBJECTS_DIR = tmp
MOC_DIR = tmp
RCC_DIR = tmp


# Include source files through our .pri file
!include(IngeScapeQuick.pri) {
    error(Could not load IngeScapeQuick.pri)
}


# Additional source files required when IngeScapeQuick is used as a shared library or plugin
SOURCES += \
    $$PWD/ingescapequick_plugin.cpp


# Additional header files required when IngeScapeQuick is used as a shared library or plugin
HEADERS += \
    $$PWD/ingescapequick_plugin.h



#
# Files to be included in the dist target
#
DISTFILES = qmldir \
    IngeScapeQuick.qmltypes



#
# Copy our qmldir file
#
!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    copy_qmldir.target = $$OUT_PWD/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}



#####################################################################
#
# OS specific rules
#
#####################################################################


qmldir.files = qmldir
unix {
    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir
}
