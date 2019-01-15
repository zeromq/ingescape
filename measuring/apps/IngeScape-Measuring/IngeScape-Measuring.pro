#####################################################################
#
# Generic (multi-platform) rules
#
#####################################################################

TEMPLATE = app


# Application version
VERSION_MAJOR = 0
VERSION_MINOR = 1
VERSION_BUILD = 0
VERSION_PATCH = 0

VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_BUILD}.$${VERSION_PATCH}

# Qt modules used by our application and/or its dependencies
QT += qml quick quick-private svg xml concurrent sql core-private gui-private

CONFIG += c++11

# Warnings = error
QMAKE_CXXFLAGS += -Werror

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp

RESOURCES += qml.qrc


# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


#####################################################################
#
# Additionnal code to use frameworks (I2Quick, etc.)
#
#####################################################################

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

