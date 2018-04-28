#-------------------------------------------------
#
# Project created by QtCreator 2017-01-05T12:18:11
#
#-------------------------------------------------

TARGET = ingescape

TEMPLATE = lib
#CONFIG += console

DEFINES += INGESCAPE

win32:{
    DEFINES +=  _CRT_SECURE_NO_WARNINGS \
                _CRT_NONSTDC_NO_DEPRECATE \
                WINDOWS
}
#UNCOMMENT THIS LINE TO ANDROID BUILD OR COMMENT FOR ANOTHER UNIX PLATFORM
#CONFIG+=android_compilation

#UNCOMMENT THIS LINE TO RASPBERRY BUILD OR COMMENT FOR ANOTHER UNIX PLATFORM
#CONFIG+=raspberry_compilation


#include ('ingescape-dev.pri')

include('ingescape.pri')

