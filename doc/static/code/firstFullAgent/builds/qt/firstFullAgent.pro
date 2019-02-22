QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../src/main.c

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:{
    INCLUDEPATH += $$PWD/../windows/dependencies

    IS64BITSYSTEM = $$(ProgramW6432)

    isEmpty(IS64BITSYSTEM) {
        message(This is 32 bit system)
        INCLUDEPATH += $$(ProgramFiles)\Ingescape\include
        libs_path = $$(ProgramFiles)\Ingescape\lib
    }
    !isEmpty(IS64BITSYSTEM) {
        message(This is 64 bit system)
        contains(QT_ARCH, i386) {
            message("compiling with 32-bit architecture")
            INCLUDEPATH += $$quote("C:\Program Files (x86)\Ingescape\include")
            libs_path = $$quote("C:\Program Files (x86)\Ingescape\lib")
        }else {
            message("compiling with 64-bit architecture")
            INCLUDEPATH += $$(ProgramW6432)\Ingescape\include
            libs_path = $$(ProgramW6432)\Ingescape\lib
        }
    }

    #add librairies
    LIBS += -L$$libs_path -lingescape -lczmq
}

!win32:{
    #include ingescape headers
    INCLUDEPATH += /usr/local/include

    #add librairies
    LIBS += -L/usr/local/lib -lingescape -lczmq
}
