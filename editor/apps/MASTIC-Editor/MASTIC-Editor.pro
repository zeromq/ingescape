#####################################################################
#
# Generic (multi-platform) rules
#
#####################################################################

TEMPLATE = app

QT += qml quick quick-private svg xml concurrent core-private gui-private
CONFIG += c++11 precompiled_header

# Warnings = error
QMAKE_CXXFLAGS += -Werror

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER  = stable.h


SOURCES += main.cpp \
    model/agentm.cpp \
    viewModel/agentvm.cpp \
    model/iop/agentiopm.cpp \
    model/jsonhelper.cpp \
    viewModel/iop/agentiopvm.cpp \
    controller/agentssupervisioncontroller.cpp \
    controller/agentsmappingcontroller.cpp \
    controller/masticeditorcontroller.cpp \
    controller/masticmodelmanager.cpp \
    controller/networkcontroller.cpp \
    misc/masticeditorsettings.cpp \
    misc/masticeditorutils.cpp \
    viewModel/linkvm.cpp \
    model/definitionm.cpp \
    model/agentmappingm.cpp \
    viewModel/clonedagentvm.cpp


HEADERS += \
    model/agentm.h \
    viewModel/agentvm.h \
    stable.h \
    model/iop/agentiopm.h \
    model/jsonhelper.h \
    viewModel/iop/agentiopvm.h \
    controller/agentssupervisioncontroller.h \
    controller/agentsmappingcontroller.h \
    controller/masticeditorcontroller.h \
    controller/masticmodelmanager.h \
    controller/networkcontroller.h \
    misc/masticeditorsettings.h \
    misc/masticeditorutils.h \
    uthash/libut.h \
    uthash/ringbuf.h \
    uthash/utarray.h \
    uthash/uthash.h \
    uthash/utlist.h \
    uthash/utmm.h \
    uthash/utringbuffer.h \
    uthash/utstring.h \
    uthash/utvector.h \
    viewModel/linkvm.h \
    model/definitionm.h \
    model/agentmappingm.h \
    viewModel/clonedagentvm.h


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
QML_IMPORT_PATH = ../../frameworks


# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =


# Add the include directory of I2Quick
INCLUDEPATH += ../../frameworks/I2Quick/include


# Include MASTIC library
!include(../../../dependencies/mastic_embedded.pri) {
    error(Could not load mastic_embedded.pri)
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
    message(OSX specific rules)


    # Compute the LFLAG associated to our frameworks
    LIBS += -L../../frameworks/I2Quick/Mac -lI2Quick


    # Copy libraries into the MacOS directory of our application
    librariesToCopy.files += ../../frameworks/I2Quick/Mac/libI2Quick.$${QMAKE_EXTENSION_SHLIB}
    librariesToCopy.path = Contents/MacOS
    QMAKE_BUNDLE_DATA += librariesToCopy


    # Release / debug specific rules
    CONFIG(release, debug|release) {
        # We must call install_name_tool to create a valid link. Otherwise, our application will not found our library
        QMAKE_POST_LINK += $$quote(install_name_tool -change libI2Quick.$${QMAKE_EXTENSION_SHLIB} @executable_path/libI2Quick.$${QMAKE_EXTENSION_SHLIB} $${OUT_PWD}/$${TARGET}.app/Contents/MacOS/$${TARGET} $$escape_expand(\\n\\t))

        # Release only: copy Qt libs and plugins inside our application to create a standalone application
        # NB: macdeployqt only runs qmlimportscanner correctly when run from Qt bin directory
        QMAKE_POST_LINK += $$quote(cd `dirname $(QMAKE)` && macdeployqt $${OUT_PWD}/$${TARGET}.app -qmldir=$${PWD} $$escape_expand(\n\t))
    }
    else {
        # We must call install_name_tool to create a valid link. Otherwise, our application will not found our library
        # NB: useless in debug mode because Qt Creator can find our I2Quick library
       # QMAKE_POST_LINK += $$quote(install_name_tool -change libI2Quick_debug.$${QMAKE_EXTENSION_SHLIB} @executable_path/libI2Quick_debug.$${QMAKE_EXTENSION_SHLIB} $${OUT_PWD}/$${TARGET}.app/Contents/MacOS/$${TARGET} $$escape_expand(\\n\\t))
    }


    # Set icons
    ICON = icon.icns
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
    QMAKE_CXXFLAGS += /WX

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

        # Copy Qt dlls
        # NB: Some Qt libs must be explictly referenced because their are used by I2Quick.dll and not our .exe
        QMAKE_POST_LINK += windeployqt $${DESTDIR}/$${TARGET}.exe -xml -concurrent -printsupport -qmldir=$${PWD} $$escape_expand(\n\t)
    }
    else {
        # copy I2Quick (if we don't call make install)
        QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$shell_path(../../frameworks/I2Quick/Win32/I2Quickd.$${QMAKE_EXTENSION_SHLIB})) $$quote($$shell_path($${DESTDIR})) $$escape_expand(\n\t)
    }


    # Set icons
    RC_ICONS = icon.ico

    # Set application info
    VERSION = 0.0.0.0
    QMAKE_TARGET_COMPANY = Ingenuity i/o
    QMAKE_TARGET_PRODUCT = $${TARGET}
    QMAKE_TARGET_DESCRIPTION = MASTIC-EDITOR
    QMAKE_TARGET_COPYRIGHT = Copyright (c) 2017, Ingenuity i/o
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


