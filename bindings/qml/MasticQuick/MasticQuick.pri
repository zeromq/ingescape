INCLUDEPATH += $$PWD

# Qt modules used by MasticQuick
QT += qml quick


# Defines MASTICQUICK_INCLUDED_VIA_PRI (by default, we don't want to export symbols when MasticQuick is included in another .pro file)
DEFINES += MASTICQUICK_INCLUDED_VIA_PRI


# Include Mastic library
!include(../../../builds/mastic.pri) {
    error(Could not load mastic.pri)
}


HEADERS += \
    $$PWD/MasticQuick.h \
    $$PWD/masticquick_global.h \
    $$PWD/masticquick_helpers.h \
    $$PWD/masticquickcontroller.h \
    $$PWD/masticquickinputspropertymap.h \
    $$PWD/masticquickoutputspropertymap.h \
    $$PWD/masticquickparameterspropertymap.h


SOURCES += \
    $$PWD/masticquick.cpp \
    $$PWD/masticquick_helpers.cpp \
    $$PWD/masticquickcontroller.cpp \
    $$PWD/masticquickinputspropertymap.cpp \
    $$PWD/masticquickoutputspropertymap.cpp \
    $$PWD/masticquickparameterspropertymap.cpp
