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
    $$PWD/masticquick_global.h \
    $$PWD/masticquick_helpers.h \
    $$PWD/masticquickinputspropertymap.h \
    $$PWD/masticquickoutputspropertymap.h \
    $$PWD/masticquickparameterspropertymap.h \
    $$PWD/MasticQuick.h \
    $$PWD/masticquickinputbinding.h \
    $$PWD/masticquickbinding.h \
    $$PWD/masticquick_enums.h


SOURCES += \
    $$PWD/masticquick_helpers.cpp \
    $$PWD/masticquickinputspropertymap.cpp \
    $$PWD/masticquickoutputspropertymap.cpp \
    $$PWD/masticquickparameterspropertymap.cpp \
    $$PWD/masticquick.cpp \
    $$PWD/masticquickinputbinding.cpp \
    $$PWD/masticquickbinding.cpp \
    $$PWD/masticquick_enums.cpp
