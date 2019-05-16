INCLUDEPATH += $$PWD


QT += qml quick svg xml sql quick-private core-private gui-private concurrent


SOURCES += \
    $$PWD/ingescapecommon.cpp \
    $$PWD/settings/ingescapesettings.cpp \
    $$PWD/misc/terminationsignalwatcher.cpp \
    $$PWD/misc/ingescapeutils.cpp \
    $$PWD/misc/debugquickinspector.cpp \
    $$PWD/model/enums.cpp \
    $$PWD/model/jsonhelper.cpp \
    $$PWD/model/publishedvaluem.cpp \
    $$PWD/model/agent/agentm.cpp \
    $$PWD/model/agent/definition/definitionm.cpp \
    $$PWD/model/agent/definition/agentiopm.cpp \
    $$PWD/model/agent/definition/outputm.cpp \
    $$PWD/model/agent/mapping/agentmappingm.cpp \
    $$PWD/model/agent/mapping/mappingelementm.cpp \
    $$PWD/model/scenario/actionm.cpp \
    $$PWD/model/scenario/scenariom.cpp \
    $$PWD/model/scenario/timetickm.cpp \
    $$PWD/model/scenario/condition/actionconditionm.cpp \
    $$PWD/model/scenario/condition/conditiononagentm.cpp \
    $$PWD/model/scenario/condition/iopvalueconditionm.cpp \
    $$PWD/model/scenario/effect/actioneffectm.cpp \
    $$PWD/model/scenario/effect/iopvalueeffectm.cpp \
    $$PWD/model/scenario/effect/mappingeffectm.cpp \
    $$PWD/model/scenario/effect/effectonagentm.cpp \
    $$PWD/viewModel/agentsgroupedbydefinitionvm.cpp \
    $$PWD/viewModel/agentsgroupedbynamevm.cpp \
    $$PWD/viewModel/iop/agentiopvm.cpp \
    $$PWD/viewModel/iop/inputvm.cpp \
    $$PWD/viewModel/iop/outputvm.cpp \
    $$PWD/viewModel/iop/parametervm.cpp \
    $$PWD/viewModel/mapping/mappingelementvm.cpp \
    $$PWD/viewModel/scenario/actionvm.cpp \
    $$PWD/viewModel/scenario/actionconditionvm.cpp \
    $$PWD/viewModel/scenario/actioneffectvm.cpp \
    $$PWD/viewModel/scenario/actioninpalettevm.cpp \
    $$PWD/viewModel/scenario/actionexecutionvm.cpp \
    $$PWD/controller/ingescapemodelmanager.cpp \
    $$PWD/controller/abstracttimeactionslinescenarioviewcontroller.cpp \
    $$PWD/sortFilter/abstracttimerangefilter.cpp

HEADERS += \
    $$PWD/ingescapecommon.h \
    $$PWD/ingescape-common_global.h \
    $$PWD/settings/ingescapesettings.h \
    $$PWD/misc/terminationsignalwatcher.h \
    $$PWD/misc/ingescapeutils.h \
    $$PWD/misc/debugquickinspector.h \
    $$PWD/model/enums.h \
    $$PWD/model/jsonhelper.h \
    $$PWD/model/publishedvaluem.h \
    $$PWD/model/agent/agentm.h \
    $$PWD/model/agent/definition/definitionm.h \
    $$PWD/model/agent/definition/agentiopm.h \
    $$PWD/model/agent/definition/outputm.h \
    $$PWD/model/agent/mapping/agentmappingm.h \
    $$PWD/model/agent/mapping/mappingelementm.h \
    $$PWD/model/scenario/actionm.h \
    $$PWD/model/scenario/scenariom.h \
    $$PWD/model/scenario/timetickm.h \
    $$PWD/model/scenario/condition/actionconditionm.h \
    $$PWD/model/scenario/condition/conditiononagentm.h \
    $$PWD/model/scenario/condition/iopvalueconditionm.h \
    $$PWD/model/scenario/effect/actioneffectm.h \
    $$PWD/model/scenario/effect/iopvalueeffectm.h \
    $$PWD/model/scenario/effect/mappingeffectm.h \
    $$PWD/model/scenario/effect/effectonagentm.h \
    $$PWD/viewModel/agentsgroupedbydefinitionvm.h \
    $$PWD/viewModel/agentsgroupedbynamevm.h \
    $$PWD/viewModel/iop/agentiopvm.h \
    $$PWD/viewModel/iop/inputvm.h \
    $$PWD/viewModel/iop/outputvm.h \
    $$PWD/viewModel/iop/parametervm.h \
    $$PWD/viewModel/mapping/mappingelementvm.h \
    $$PWD/viewModel/scenario/actionvm.h \
    $$PWD/viewModel/scenario/actionconditionvm.h \
    $$PWD/viewModel/scenario/actioneffectvm.h \
    $$PWD/viewModel/scenario/actioninpalettevm.h \
    $$PWD/viewModel/scenario/actionexecutionvm.h \
    $$PWD/controller/ingescapemodelmanager.h \
    $$PWD/controller/abstracttimeactionslinescenarioviewcontroller.h \
    $$PWD/sortFilter/abstracttimerangefilter.h


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

DISTFILES +=

RESOURCES += \
    $$PWD/igs_common_qml.qrc
