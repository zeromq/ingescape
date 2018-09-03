QT += quick
CONFIG += c++11 precompiled_header

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp

RESOURCES += qml.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =


win32:{
    # Include ingescape headers
    INCLUDEPATH += $$PWD/dependencies/headers
    INCLUDEPATH += $$shell_path($$(IGS_INCLUDE_PATH))

    CONFIG(debug, debug|release){
        libs_path = $$PWD/dependencies/libs/win32/Debug
    }else {
        libs_path = $$PWD/dependencies/libs/win32/Release
    }

    #Add librairies
    LIBS += -L$$libs_path -lingescape -llibzyre -llibczmq -lyajl
    LIBS += -L$$shell_path($$(IGS_LIBS_PATH)) -lingescape -llibzyre -llibczmq -lyajl

    #Windows specific libraries
    LIBS += -L$$C:/Windows/System32 -lwsock32 -lIPHLPAPI -lws2_32
}

!win32:{
    # Include ingescape headers
    INCLUDEPATH += /usr/local/include

    #Add librairies
    LIBS += -L/usr/local/lib -lingescape -lczmq -lzyre -lyajl
}

