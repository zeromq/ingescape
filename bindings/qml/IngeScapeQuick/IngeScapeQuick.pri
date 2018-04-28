INCLUDEPATH += $$PWD

# Qt modules used by IngeScapeQuick
QT += qml quick


# Defines INGESCAPEQUICK_INCLUDED_VIA_PRI (by default, we don't want to export symbols when IngeScapeQuick is included in another .pro file)
DEFINES += INGESCAPEQUICK_INCLUDED_VIA_PRI


# Include IngeScape library
!include(../../../builds/ingescape.pri) {
    error(Could not load ingescape.pri)
}


HEADERS += \
    $$PWD/ingescapequick_global.h \
    $$PWD/ingescapequick_helpers.h \
    $$PWD/ingescapequickinputspropertymap.h \
    $$PWD/ingescapequickoutputspropertymap.h \
    $$PWD/ingescapequickparameterspropertymap.h \
    $$PWD/IngeScapeQuick.h \
    $$PWD/ingescapequickinputbinding.h \
    $$PWD/ingescapequick_enums.h \
    $$PWD/ingescapequickoutputbinding.h \
    $$PWD/ingescapequickbindingsingleton.h \
    $$PWD/ingescapequickabstractiopbinding.h


SOURCES += \
    $$PWD/ingescapequick_helpers.cpp \
    $$PWD/ingescapequickinputspropertymap.cpp \
    $$PWD/ingescapequickoutputspropertymap.cpp \
    $$PWD/ingescapequickparameterspropertymap.cpp \
    $$PWD/ingescapequick.cpp \
    $$PWD/ingescapequickinputbinding.cpp \
    $$PWD/ingescapequick_enums.cpp \
    $$PWD/ingescapequickoutputbinding.cpp \
    $$PWD/ingescapequickbindingsingleton.cpp \
    $$PWD/ingescapequickabstractiopbinding.cpp
