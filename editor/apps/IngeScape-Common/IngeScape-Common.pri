INCLUDEPATH += $$PWD

# NB.1: quick-private is only required by the patched version of QQuickRepeater
# NB.2: core-private is only required by the patched version of QSortFilterProxyModel
#       that will be useless with Qt 5.6.2
QT += qml quick svg xml sql quick-private core-private concurrent


SOURCES += \
        $$PWD/ingescapecommon.cpp \
    $$PWD/settings/ingescapesettings.cpp \
    $$PWD/misc/terminationsignalwatcher.cpp \
    $$PWD/misc/ingescapeutils.cpp \
    $$PWD/misc/debugquickinspector.cpp

HEADERS += \
        $$PWD/ingescapecommon.h \
        $$PWD/ingescape-common_global.h \
    $$PWD/settings/ingescapesettings.h \
    $$PWD/misc/terminationsignalwatcher.h \
    $$PWD/misc/ingescapeutils.h \
    $$PWD/misc/debugquickinspector.h


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
