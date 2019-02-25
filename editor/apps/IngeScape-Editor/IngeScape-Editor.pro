#####################################################################
#
# Generic (multi-platform) rules
#
#####################################################################

TEMPLATE = app


# Application version
VERSION_MAJOR = 0
VERSION_MINOR = 9
VERSION_BUILD = 0
VERSION_PATCH = 0

VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_BUILD}.$${VERSION_PATCH}


# Qt modules used by our application and/or its dependencies
QT += qml quick quick-private svg xml concurrent sql core-private gui-private

CONFIG += c++11 precompiled_header

# Warnings = error
QMAKE_CXXFLAGS += -Werror

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER  = stable.h


# To build temp files (*.o, moc_*.cpp and qrc_*.cpp) in a temp dir
OBJECTS_DIR = tmp
MOC_DIR = tmp
RCC_DIR = tmp


SOURCES += main.cpp \
    controller/agentssupervisioncontroller.cpp \
    controller/agentsmappingcontroller.cpp \
    controller/ingescapeeditorcontroller.cpp \
    controller/ingescapemodelmanager.cpp \
    controller/networkcontroller.cpp \
    controller/scenariocontroller.cpp \
    controller/actioneditorcontroller.cpp \
    controller/valueshistorycontroller.cpp \
    controller/abstracttimeactionslinescenarioviewcontroller.cpp \
    misc/collapsiblecolumn.cpp \
    model/agentm.cpp \
    model/iop/agentiopm.cpp \
    model/iop/outputm.cpp \
    model/jsonhelper.cpp \
    model/definitionm.cpp \
    model/mapping/agentmappingm.cpp \
    model/mapping/elementmappingm.cpp \
    model/scenario/condition/actionconditionm.cpp \
    model/scenario/actionm.cpp \
    model/scenario/scenariom.cpp \
    model/scenario/scenariomarkerm.cpp \
    model/scenario/condition/iopvalueconditionm.cpp \
    model/enums.cpp \
    model/publishedvaluem.cpp \
    model/scenario/timetickm.cpp \
    model/scenario/effect/actioneffectm.cpp \
    model/scenario/effect/iopvalueeffectm.cpp \
    model/scenario/effect/mappingeffectm.cpp \
    model/scenario/effect/effectonagentm.cpp \
    sortFilter/valueshistorysortfilter.cpp \
    viewModel/agentinmappingvm.cpp \
    viewModel/scenario/actionvm.cpp \
    viewModel/iop/inputvm.cpp \
    viewModel/iop/outputvm.cpp \
    viewModel/scenario/actionconditionvm.cpp \
    viewModel/scenario/actioneffectvm.cpp \
    viewModel/scenario/actioninpalettevm.cpp \
    viewModel/scenario/actionexecutionvm.cpp \
    viewModel/iop/parametervm.cpp \
    model/hostm.cpp \
    viewModel/iop/agentiopvm.cpp \
    sortFilter/abstracttimerangefilter.cpp \
    controller/hostssupervisioncontroller.cpp \
    viewModel/hostvm.cpp \
    model/recordm.cpp \
    controller/recordssupervisioncontroller.cpp \
    viewModel/recordvm.cpp \
    controller/logstreamcontroller.cpp \
    model/logm.cpp \
    sortFilter/logssortfilter.cpp \
    model/scenario/condition/conditiononagentm.cpp \
    misc/qquickwindowblocktouches.cpp \
    viewModel/agentsgroupedbynamevm.cpp \
    viewModel/agentsgroupedbydefinitionvm.cpp \
    viewModel/link/linkinputvm.cpp \
    viewModel/link/linkoutputvm.cpp \
    viewModel/link/linkvm.cpp \
    viewModel/link/linkconnectorvm.cpp \
    misc/textfielddoublevalidator.cpp \
    misc/numberconstants.cpp \
    viewModel/link/mappingelementvm.cpp

HEADERS += \
    stable.h \
    uthash/libut.h \
    uthash/ringbuf.h \
    uthash/utarray.h \
    uthash/uthash.h \
    uthash/utlist.h \
    uthash/utmm.h \
    uthash/utringbuffer.h \
    uthash/utstring.h \
    uthash/utvector.h \
    controller/agentssupervisioncontroller.h \
    controller/agentsmappingcontroller.h \
    controller/ingescapeeditorcontroller.h \
    controller/ingescapemodelmanager.h \
    controller/networkcontroller.h \
    controller/scenariocontroller.h \
    controller/actioneditorcontroller.h \
    controller/valueshistorycontroller.h \
    controller/abstracttimeactionslinescenarioviewcontroller.h \
    misc/collapsiblecolumn.h \
    model/agentm.h \
    model/iop/agentiopm.h \
    model/iop/outputm.h \
    model/jsonhelper.h \
    model/definitionm.h \
    model/mapping/agentmappingm.h \
    model/mapping/elementmappingm.h \
    model/scenario/condition/actionconditionm.h \
    model/scenario/actionm.h \
    model/scenario/scenariom.h \
    model/scenario/scenariomarkerm.h \
    model/scenario/condition/iopvalueconditionm.h \
    model/enums.h \
    model/publishedvaluem.h \
    model/scenario/timetickm.h \
    model/scenario/effect/actioneffectm.h \
    model/scenario/effect/iopvalueeffectm.h \
    model/scenario/effect/mappingeffectm.h \
    model/scenario/effect/effectonagentm.h \
    sortFilter/valueshistorysortfilter.h \
    viewModel/agentinmappingvm.h \
    viewModel/scenario/actionvm.h \
    viewModel/iop/inputvm.h \
    viewModel/iop/outputvm.h \
    viewModel/scenario/actionconditionvm.h \
    viewModel/scenario/actioneffectvm.h \
    viewModel/scenario/actioninpalettevm.h \
    viewModel/scenario/actionexecutionvm.h \
    viewModel/iop/parametervm.h \
    model/hostm.h \
    viewModel/iop/agentiopvm.h \
    sortFilter/abstracttimerangefilter.h \
    controller/hostssupervisioncontroller.h \
    viewModel/hostvm.h \
    model/recordm.h \
    controller/recordssupervisioncontroller.h \
    viewModel/recordvm.h \
    controller/logstreamcontroller.h \
    model/logm.h \
    sortFilter/logssortfilter.h \
    model/scenario/condition/conditiononagentm.h \
    misc/qquickwindowblocktouches.h \
    viewModel/agentsgroupedbynamevm.h \
    viewModel/agentsgroupedbydefinitionvm.h \
    viewModel/link/linkinputvm.h \
    viewModel/link/linkoutputvm.h \
    viewModel/link/linkvm.h \
    viewModel/link/linkconnectorvm.h \
    misc/textfielddoublevalidator.h \
    misc/numberconstants.h \
    viewModel/link/mappingelementvm.h


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
# Additionnal code to use frameworks (I2Quick, etc.)
#
#####################################################################


# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = ../../frameworks .


# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =


# Add the include directory of I2Quick
INCLUDEPATH += ../../frameworks/I2Quick/include

# Ingescape is not use as library, source code are build like part of editor source code
DEFINES += INGESCAPE
# Include ingescape library
!include(../../../builds/ingescape.pri) {
    error(Could not load ingescape.pri)
}


# Include PGIMCommon library
!include(../Ingescape-Common/Ingescape-Common.pri) {
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


    # Include Gstreamer hearders
    # GST not included ins master branch
#    INCLUDEPATH += /usr/local/include/gstreamer-1.0
#    INCLUDEPATH += /usr/local/include/glib-2.0
#    INCLUDEPATH += /usr/local/lib/glib-2.0/include

    # Compute the LFLAG associated to our frameworks
    LIBS += -L../../frameworks/I2Quick/Mac -lI2Quick

    # Add gstreamer libs
    # GST not included in master branch
#    LIBS += -L/usr/local/lib -lglib-2.0.0
#    LIBS += -L/usr/local/lib -lgstreamer-1.0.0
#    LIBS += -L/usr/local/lib -lgobject-2.0.0
#    LIBS += -L/usr/local/lib -lgstvideo-1.0.0
#    LIBS += -L/usr/local/lib -lgstbase-1.0.0
#    LIBS += -L/usr/local/lib -lgstapp-1.0.0

    # Copy libraries into the MacOS directory of our application
    librariesToCopy.files += ../../frameworks/I2Quick/Mac/libI2Quick.$${QMAKE_EXTENSION_SHLIB}
    librariesToCopy.path = Contents/Frameworks
    QMAKE_BUNDLE_DATA += librariesToCopy
    # NB: libzyre, libzmq, libczmq, libsodium, libyajl will be copied by macdeployqt
    #     because they are installed in a standard directory (/usr/local/lib)


    # Release / debug specific rules
    CONFIG(release, debug|release) {
        # We must call install_name_tool to create a valid link. Otherwise, our application will not found our library
        QMAKE_POST_LINK += $$quote(install_name_tool -change libI2Quick.$${QMAKE_EXTENSION_SHLIB} @executable_path/../Frameworks/libI2Quick.$${QMAKE_EXTENSION_SHLIB} $${OUT_PWD}/$${TARGET}.app/Contents/MacOS/$${TARGET} $$escape_expand(\\n\\t))

        # Release only: copy Qt libs and plugins inside our application to create a standalone application
        # NB: macdeployqt only runs qmlimportscanner correctly when run from Qt bin directory
        QMAKE_POST_LINK += $$quote(cd `dirname $(QMAKE)` && macdeployqt $${OUT_PWD}/$${TARGET}.app -qmldir=$${PWD} $$escape_expand(\n\t))
    }
    else {
        # We must call install_name_tool to create a valid link. Otherwise, our application will not found our library
        # NB: useless in debug mode because Qt Creator can find our I2Quick library
       # QMAKE_POST_LINK += $$quote(install_name_tool -change libI2Quick_debug.$${QMAKE_EXTENSION_SHLIB} @executable_path/libI2Quick_debug.$${QMAKE_EXTENSION_SHLIB} $${OUT_PWD}/$${TARGET}.app/Contents/MacOS/$${TARGET} $$escape_expand(\\n\\t))
    }


    # Set icon of our executable
    ICON = icon_IGS.icns

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


    # Compute the LFLAG associated to our frameworks
    CONFIG(debug, debug|release) {
       LIBS += -L../../frameworks/I2Quick/Win32 -lI2Quickd

       librariesToCopy.files += ../../frameworks/I2Quick/Win32/I2Quickd.$${QMAKE_EXTENSION_SHLIB}
    }
    else {
       # Compute the LFLAG associated to our I2Quick library
       LIBS += -L../../frameworks/I2Quick/Win32 -lI2Quick

       librariesToCopy.files += ../../frameworks/I2Quick/Win32/I2Quick.$${QMAKE_EXTENSION_SHLIB}
    }


    # Copy libraries in our application directory
    librariesToCopy.path = $${DESTDIR}
    INSTALLS += librariesToCopy


    # Release only: copy Qt libs and plugins next to our application to create a standalone application
    CONFIG(release, debug|release) {
        # copy I2Quick (if we don't call make install)
        QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$shell_path(../../frameworks/I2Quick/Win32/I2Quick.$${QMAKE_EXTENSION_SHLIB})) $$quote($$shell_path($${DESTDIR})) $$escape_expand(\n\t)

        #copy ingescape, zyre and friends to make a real standalone .exe
        QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$shell_path(C:\ingescape\libs\release\*.dll)) $$quote($$shell_path($${DESTDIR})) $$escape_expand(\n\t)

        # Copy Qt dlls
        # NB: Some Qt libs must be explictly referenced because their are used by I2Quick.dll and not our .exe
        QMAKE_POST_LINK += windeployqt $${DESTDIR}/$${TARGET}.exe -xml -concurrent -printsupport -sql -qmldir=$${PWD} $$escape_expand(\n\t)
    }
    else {
        # copy I2Quick (if we don't call make install)
        QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$shell_path(../../frameworks/I2Quick/Win32/I2Quickd.$${QMAKE_EXTENSION_SHLIB})) $$quote($$shell_path($${DESTDIR})) $$escape_expand(\n\t)
    }


    # Set icon of our executable
    RC_ICONS = icon_IGS.ico

    # Set application info
    QMAKE_TARGET_COMPANY = Ingenuity i/o
    QMAKE_TARGET_PRODUCT = $${TARGET}
    QMAKE_TARGET_DESCRIPTION = IngeScape-Editor
    QMAKE_TARGET_COPYRIGHT = Copyright (c) 2017-2018, Ingenuity i/o
}






#------------------------
#
# Linux specific
#
#------------------------
unix:!mac {
    message(Linux specific rules)

    # TODO if needed
}
