INCLUDEPATH += $$PWD


QT += qml quick svg xml sql quick-private core-private gui-private concurrent


SOURCES += \
    $$PWD/ingescapecommon.cpp \
    $$PWD/settings/ingescapesettings.cpp \
    $$PWD/misc/terminationsignalwatcher.cpp \
    $$PWD/misc/ingescapeutils.cpp \
    $$PWD/misc/debugquickinspector.cpp \
    $$PWD/model/enums.cpp \
    $$PWD/model/definitionm.cpp \
    $$PWD/model/iop/agentiopm.cpp \
    $$PWD/model/iop/outputm.cpp

HEADERS += \
    $$PWD/ingescapecommon.h \
    $$PWD/ingescape-common_global.h \
    $$PWD/settings/ingescapesettings.h \
    $$PWD/misc/terminationsignalwatcher.h \
    $$PWD/misc/ingescapeutils.h \
    $$PWD/misc/debugquickinspector.h \
    $$PWD/model/enums.h \
    $$PWD/model/definitionm.h \
    $$PWD/model/iop/agentiopm.h \
    $$PWD/model/iop/outputm.h


#RESOURCES += \
#    $$PWD/ingescapecommon_qml.qrc


# This variable only makes sense in a subdirs project.
# It lists the directories to build. If it is absent the build system will attempt to build projects underneath this directory.
# Note that this behavior does not work for the project at the top of a tree, which must use the second notation to build all projects.
#SUBDIRS += \
#    $$PWD/IngeScape-Common.pro


#DISTFILES += \
#    $$PWD/IngeScape-Common.qmltypes \
#    $$PWD/qmldir
