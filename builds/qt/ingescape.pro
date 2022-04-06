#-------------------------------------------------
#
# Project created by QtCreator 2017-01-05T12:18:11
#
#-------------------------------------------------

TARGET = ingescape

TEMPLATE = lib
CONFIG += plugin

DEFINES += INGESCAPE_EXPORTS

win32:{
    DEFINES +=  _CRT_SECURE_NO_WARNINGS \
                _CRT_NONSTDC_NO_DEPRECATE \
                WINDOWS
}

#UNCOMMENT THIS LINE TO RASPBERRY BUILD OR COMMENT FOR ANOTHER UNIX PLATFORM
#CONFIG+=raspberry_compilation


# To build temp files (*.o, moc_*.cpp and qrc_*.cpp) in a temp dir
OBJECTS_DIR = tmp
MOC_DIR = tmp
RCC_DIR = tmp


#include ('ingescape-dev.pri')

include('ingescape.pri')
